#include "../src/extract_keys.h"
#include "../src/keylist.h"
#include "../src/literal.h"
#include "../src/select.h"
#include "../src/size_least.h"
#include "../src/traits.h"
#include "../src/utils.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <optional>
#include <string_view>

#include <utility>

namespace avakar {

namespace _atom {

template <typename V>
struct visit_table_entry
{
	V fnidx;
	V val;
};

template <
	typename F,
	keylist auto kl,
	typename In,
	typename Sn = std::index_sequence<>,
	typename Un = std::index_sequence<>
	>
struct build_select;

template <typename F, keylist auto kl, std::size_t i0, std::size_t... in, std::size_t... sn, std::size_t... un>
requires (!std::invocable<F, atom<select<kl, 1>{{i0}}>>)
struct build_select<F, kl, std::index_sequence<i0, in...>, std::index_sequence<sn...>, std::index_sequence<un...>>
	: build_select<F, kl, std::index_sequence<in...>, std::index_sequence<sn...>, std::index_sequence<un..., i0>>
{
};

template <typename F, keylist auto kl, std::size_t i0, std::size_t... in, std::size_t... sn, std::size_t... un>
requires (std::invocable<F, atom<select<kl, 1>{{i0}}>>)
struct build_select<F, kl, std::index_sequence<i0, in...>, std::index_sequence<sn...>, std::index_sequence<un...>>
	: build_select<F, kl, std::index_sequence<in...>, std::index_sequence<sn..., i0>, std::index_sequence<un...>>
{
};

template <typename F, keylist auto kl, std::size_t... sn, typename Un>
struct build_select<F, kl, std::index_sequence<>, std::index_sequence<sn...>, Un>
{
	using selected = std::index_sequence<sn...>;
	using unselected = Un;

	static constexpr auto make_select() noexcept
	{
		return select<kl, sizeof...(sn)>{{sn...}};
	}

	template <typename V, std::size_t N>
	static constexpr void fill_table(std::array<visit_table_entry<V>, N> & tbl, V fnidx) noexcept
	{
		V i = 0;
		((tbl[sn] = { fnidx, i++ }), ...);
	}
};

template <keylist auto ks, typename V, V I, typename In, typename... Fn>
struct build_visit_table;

template <keylist auto ks, typename V, V I, typename In>
struct build_visit_table<ks, V, I, In>
{
	using table_type = std::array<visit_table_entry<V>, size(keylist_tag{}, ks)>;
	static constexpr bool is_complete = In::size() == 0;

	static constexpr void fill_table(table_type &) noexcept
	{
	}
};

template <keylist auto ks, typename V, V I, typename In, typename F0, typename... Fn>
struct build_visit_table<ks, V, I, In, F0, Fn...>
{
	using table_type = std::array<visit_table_entry<V>, size(keylist_tag{}, ks)>;

	using BS = build_select<F0, ks, In>;
	using _next_t = build_visit_table<ks, V, I + 1, typename BS::unselected, Fn...>;

	static constexpr bool is_complete = _next_t::is_complete;

	static constexpr void fill_table(table_type & tbl) noexcept
	{
		BS::fill_table(tbl, I);
		_next_t::fill_table(tbl);
	}
};

}

template <_atom::keylist auto... vn>
struct atom
{
	static constexpr auto keys = _atom::extract_keys<vn...>();
	static_assert(!_atom::has_duplicates(keys), "The list of keys has duplicates");

	using value_type = typename _atom::size_least<keys.size()>::type;

	template <_atom::atom_literal A>
	constexpr atom(A) noexcept
	requires (_atom::index_of(keys, _atom::to_string<A>()) != keys.size())
		: _v((value_type)_atom::index_of(keys, _atom::to_string<A>()))
	{
	}

	template <auto... bn>
	constexpr atom(atom<bn...> x) noexcept
	requires (_atom::is_subset(keys, atom<bn...>::keys))
		: _v(_conv_table<bn...>[x.value()])
	{
	}

	static constexpr atom from_value(value_type v) noexcept
	{
		assert(v < keys.size());
		return atom(v);
	}

	static constexpr std::optional<atom> from_string(std::string_view s) noexcept
	{
		auto it = std::upper_bound(keys.begin(), keys.end(), s);
		if (it == keys.begin())
			return std::nullopt;

		--it;
		if (*it != s)
			return std::nullopt;

		return atom::from_value((value_type)(it - keys.begin()));
	}

	constexpr value_type value() const noexcept
	{
		return _v;
	}

	constexpr std::string_view to_string() const noexcept
	{
		return keys[_v];
	}

	template <any_atom B>
	constexpr bool is() const noexcept
	{
		return _to_table<B>[_v] != 0;
	}

	template <any_atom B>
	constexpr std::optional<B> to() const noexcept
	{
		auto new_value = _to_table<B>[_v];
		if (new_value == 0)
			return std::nullopt;
		else
			return B::from_value((typename B::value_type)(new_value - 1));
	}

	template <typename... Fn>
	using _visit_builder = _atom::build_visit_table<&keys, value_type, 0, std::make_index_sequence<keys.size()>, Fn...>;

	template <std::size_t... in, typename F0, typename... Fn>
	static decltype(auto) _visit(std::size_t fi, std::size_t v, std::index_sequence<in...>, F0 && f0, Fn &&... fn)
	{
		using X = _atom::build_select<F0, &keys, std::index_sequence<in...>>;

		if constexpr (sizeof...(Fn) != 0 && X::unselected::size() != 0)
		{
			if (fi != 0)
				return _visit(fi - 1, v, typename X::unselected{}, (Fn &&)fn...);
		}

		using T = atom<X::make_select()>;
		return ((F0 &&)f0)(T::from_value((typename T::value_type)v));
	}

	template <typename... Fn>
	static constexpr auto _make_visit_table() noexcept
	{
		std::array<_atom::visit_table_entry<value_type>, keys.size()> tbl{};
		_visit_builder<Fn...>::fill_table(tbl);
		return tbl;
	}

	template <typename... Fn>
	static constexpr auto _visit_table = _make_visit_table<Fn...>();

	template <typename F0, typename... Fn>
	requires (_visit_builder<F0, Fn...>::is_complete)
	decltype(auto) visit(F0 && f0, Fn &&... fn) const
	{
		auto const & tbl = _visit_table<F0, Fn...>[_v];
		return _visit(tbl.fnidx, tbl.val, std::make_index_sequence<keys.size()>{}, (F0 &&)f0, (Fn &&)fn...);
	}

	friend constexpr auto operator<=>(atom lhs, atom rhs) noexcept = default;

private:
	explicit constexpr atom(value_type v) noexcept
		: _v(v)
	{
	}

	template <auto... bn>
	static constexpr auto _make_conv_table() noexcept
	{
		using B = atom<bn...>;
		std::array<value_type, B::keys.size()> r{};

		std::size_t j = 0;
		for (std::size_t i = 0; i != keys.size() && j != B::keys.size(); ++i)
		{
			if (keys[i] == B::keys[j])
				r[j++] = (value_type)i;
		}

		return r;
	}

	template <auto... bn>
	static constexpr auto _conv_table = _make_conv_table<bn...>();

	template <any_atom B>
	static constexpr auto _make_to_table() noexcept
	{
		using V = typename _atom::size_least<B::keys.size() + 1>::type;

		std::array<V, keys.size()> r{};

		std::size_t i = 0;
		std::size_t j = 0;
		while (i != keys.size() && j != B::keys.size())
		{
			auto c = keys[i] <=> B::keys[j];
			if (c > 0)
				++j;
			else if (c == 0)
				r[i++] = (V)(++j);
			else
				++i;
		}

		return r;
	}

	template <any_atom B>
	static constexpr auto _to_table = _make_to_table<B>();

	value_type _v;
};

}

#pragma once

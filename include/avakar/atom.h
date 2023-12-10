#include "../src/extract_keys.h"
#include "../src/keylist.h"
#include "../src/literal.h"
#include "../src/size_least.h"
#include "../src/traits.h"
#include "../src/utils.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <optional>
#include <string_view>

namespace avakar {

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

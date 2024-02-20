#include "keylist.h"
#include <cstddef>
#include <string_view>
#include <type_traits>

namespace avakar {

namespace _atom {

template <typename X>
concept atom_literal
	= keylist<X>
	&& std::is_empty_v<X>
	&& (size(keylist_tag{}, X{}) == 1);



template <atom_literal A>
constexpr std::string_view to_string() noexcept
{
	return at(keylist_tag{}, A{}, 0);
}

}

template <std::size_t N>
struct s
{
	constexpr s(char const (&v)[N]) noexcept
		: str{}
	{
		char const * p = v;
		char * o = str;
		while (*p)
			*o++ = *p++;
	}

	constexpr std::string_view to_string() const noexcept
	{
		return { str, N - 1 };
	}

	char str[N-1];
};

template <s x>
struct a
{
	friend constexpr std::size_t size(_atom::keylist_tag, a) noexcept
	{
		return 1;
	}

	friend constexpr std::string_view at(_atom::keylist_tag, a, std::size_t) noexcept
	{
		return x.to_string();
	}

	constexpr static std::string_view to_string() noexcept
	{
		return x.to_string();
	}
};

inline namespace literals {

template <s x>
constexpr a<x> operator""_a() noexcept
{
	return {};
}

}

}

#pragma once

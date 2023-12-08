#include <cstddef>
#include <string_view>

namespace avakar {

template <std::size_t N>
struct s
{
	constexpr s(char const (&v)[N]) noexcept
		: _s{}
	{
		char const * p = v;
		char * o = _s;
		while (*p)
			*o++ = *p++;
	}

	constexpr std::string_view to_string() const noexcept
	{
		return { _s, N - 1 };
	}

	char _s[N];
};

template <s x>
struct a
{
	static constexpr std::string_view to_string() noexcept
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

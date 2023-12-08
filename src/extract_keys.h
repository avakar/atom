#include "concepts.h"
#include "literal.h"
#include <algorithm>
#include <array>
#include <string_view>

namespace avakar::_atom {

template <atom_literal auto x>
consteval std::size_t key_size() noexcept
{
	return 1;
}

template <atom_list auto const * x>
consteval std::size_t key_size() noexcept
{
	return std::size(*x);
}

template <atom_literal auto x>
consteval void extract_key(std::string_view *& p) noexcept
{
	*p++ = x.to_string();
}

template <atom_list auto const * x>
consteval void extract_key(std::string_view *& p) noexcept
{
	constexpr std::size_t size = key_size<x>();
	for (std::size_t i = 0; i != size; ++i)
		*p++ = (*x)[i];
}

template <auto... xn>
consteval auto extract_keys() noexcept
{
	constexpr std::size_t size = (key_size<xn>() + ... + 0);
	std::array<std::string_view, size> r{};

	std::string_view * p = r.data();
	(extract_key<xn>(p), ...);

	std::sort(r.begin(), r.end());
	return r;
}

template <std::size_t N>
consteval bool has_duplicates(std::array<std::string_view, N> const & x) noexcept
{
	for (std::size_t i = 1; i < N; ++i)
	{
		if (x[i-1] == x[i])
			return true;
	}

	return false;
}

}

#pragma once

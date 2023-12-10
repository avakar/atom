#include "keylist.h"
#include <algorithm>
#include <array>
#include <string_view>

namespace avakar::_atom {

template <keylist T>
constexpr void extract_keys(std::string_view *& out, T const & x) noexcept
{
	std::size_t n = size(keylist_tag{}, x);
	for (std::size_t i = 0; i != n; ++i)
		*out++ = at(keylist_tag{}, x, i);
}

template <auto x>
struct unfuck
{
	// This works around a msvc bug that occurs when binding a template argument
	// to a reference. Instead of binding to the argument, we instead copy the
	// argumnent into an object with static lifetime, then bind to it instead.
	// For more details about the bug see:
	// https://developercommunity.visualstudio.com/t/Invalid-code-generatedICE-when-binding-/10537736
	static constexpr auto value = x;
};

template <keylist auto... xn>
constexpr auto extract_keys() noexcept
{
	constexpr std::size_t N = (size(keylist_tag{}, unfuck<xn>::value) + ... + 0);
	std::array<std::string_view, N> r{};

	std::string_view * p = r.data();
	(extract_keys(p, unfuck<xn>::value), ...);

	std::sort(r.begin(), r.end());
	return r;
}

}

#pragma once

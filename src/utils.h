#include <array>
#include <cstddef>
#include <string_view>

namespace avakar::_atom {

template <std::size_t N>
consteval std::size_t index_of(std::array<std::string_view, N> const & keys, std::string_view k) noexcept
{
	for (std::size_t i = 0; i != keys.size(); ++i)
	{
		if (keys[i] == k)
			return i;
	}

	return keys.size();
}

template <std::size_t N, std::size_t M>
consteval bool is_subset(
	std::array<std::string_view, N> const & set,
	std::array<std::string_view, M> const & subset) noexcept
{
	std::size_t i = 0;
	std::size_t j = 0;
	while (i != set.size() && j != subset.size())
	{
		auto c = set[i] <=> subset[j];
		if (c > 0)
			return false;

		if (c == 0)
			++j;
		++i;
	}
	return true;
}

template <std::size_t N>
constexpr bool has_duplicates(std::array<std::string_view, N> const & x) noexcept
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

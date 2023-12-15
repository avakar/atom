#include "keylist.h"
#include <array>
#include <cstddef>
#include <string_view>

namespace avakar::_atom {

template <keylist auto kl, std::size_t N>
struct select
{
	friend constexpr std::size_t size(keylist_tag, select) noexcept
	{
		return N;
	}

	friend constexpr std::string_view at(keylist_tag tag, select s, std::size_t idx) noexcept
	{
		return at(tag, unfuck<kl>::value, s.indexes[idx]);
	}

	std::array<std::size_t, N> indexes;
};

}

#pragma once

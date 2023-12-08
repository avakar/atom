#include <cstddef>
#include <cstdint>

namespace avakar::_atom {

template <std::size_t N>
struct size_least
{
	using type = std::size_t;
};

template <std::size_t N>
requires (N <= 0x100)
struct size_least<N>
{
	using type = std::uint8_t;
};

template <std::size_t N>
requires (N > 0x100 && N <= 0x10000)
struct size_least<N>
{
	using type = std::uint16_t;
};

template <std::size_t N>
requires (N > 0x10000 && N <= 0xffffffff)
struct size_least<N>
{
	using type = std::uint32_t;
};

}

#pragma once

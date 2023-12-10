#include <concepts>
#include <cstddef>
#include <string_view>

namespace avakar::_atom {

struct keylist_tag {};

template <typename X>
concept keylist
	= requires(X const & xc, std::size_t idx)
{
	{ size(keylist_tag{}, xc) } noexcept -> std::same_as<std::size_t>;
	{ at(keylist_tag{}, xc, idx) } noexcept -> std::same_as<std::string_view>;
};



template <typename X>
concept _string_listish
	= requires(X const & xc, std::size_t idx)
{
	{ std::size(xc) } noexcept -> std::convertible_to<std::size_t>;
	{ xc[idx] } noexcept -> std::convertible_to<std::string_view>;
};

template <_string_listish T>
constexpr std::size_t size(keylist_tag, T const * sl) noexcept
{
	return std::size(*sl);
}

template <_string_listish T>
constexpr std::string_view at(keylist_tag, T const * sl, std::size_t idx) noexcept
{
	return (*sl)[idx];
}

}

#pragma once

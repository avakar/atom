#include <concepts>
#include <cstddef>
#include <string_view>
#include <type_traits>

namespace avakar {

template <typename A>
concept atom_literal
	= std::is_empty_v<A>
	&& std::is_default_constructible_v<A>
	&& requires
{
	{ A::to_string() } noexcept -> std::same_as<std::string_view>;
	std::integral_constant<std::size_t, A::to_string().size()>{};
};

template <typename X>
concept atom_list
	= requires(X const & xc, std::size_t idx)
{
	{ std::size(xc) } noexcept -> std::convertible_to<std::size_t>;
	{ xc[idx] } noexcept -> std::convertible_to<std::string_view>;
};

template <typename X>
concept atomish
	= std::destructible<X>
	&& std::unsigned_integral<typename X::value_type>
	&& requires
{
	{ X::keys.size() } noexcept -> std::same_as<std::size_t>;
	{ X::keys[0] } noexcept -> std::convertible_to<std::string_view>;
	{ X::from_value((uint8_t)0) } noexcept -> std::same_as<X>;
};

}

#pragma once

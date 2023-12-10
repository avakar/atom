#include "keylist.h"

namespace avakar {

template <_atom::keylist auto... vn>
struct atom;



template <typename T>
struct is_atom
	: std::false_type
{
};

template <auto... vn>
struct is_atom<atom<vn...>>
	: std::true_type
{
};

template <typename T>
inline constexpr bool is_atom_v = is_atom<T>::value;



template <typename T>
concept any_atom = is_atom_v<T>;

};

#pragma once

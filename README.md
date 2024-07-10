# atom

Magic enums for C++20.

## Introduction

For each key `x`, there's a unique *atom* of the form `"x"_a`. Atoms can be
stored in objects of type `atom<...>`, where the set of representable atoms is
specified through template arguments.

```cpp
#include <avakar/atom.h>
using namespace avakar;

using bw_t = atom<"black"_a, "white"_a>;

bw_t x = "black"_a;  // ok
x = "white"_a;       // ok
x = "pink"_a;        // error
```

Yes, atoms are enums with extra steps.

## Magic

You can convert an atom to a string via `atom::to_string()`.

```cpp
x = "black"_a;
assert(x.to_string() == "black");
```

Conversely, to convert a string to an atom, use the static function
`atom::from_string()`. The function returns `std::optional` containing the
corresponding atom, or `nullopt` if the atom is not in the type.

```cpp
assert(bw_t::from_string("black") == "black"_a);
assert(bw_t::from_string("pink") == std::nullopt);
```

## Reflection

You can access the list of keys of an atom type with `atom::keys`. The static
member is a sorted array of `string_view`.

```cpp
assert(bw_t::keys.size() == 2);
assert(bw_t::keys[0] == "black"sv);
assert(bw_t::keys[1] == "white"sv);
```

To enumerate atoms instead of keys, call `atom::iota()`. It returns a view (as
in `ranges::view`) of the type's atoms (in the sorted order).

```cpp
for (bw_t a: bw_t::iota())
    assert(a == "black"_a || a == "white"_a);
```

## Subtyping

You can assign an atom type to another as long as the set of atoms in the former
is a superset of the atoms in the latter. In the following example, `bw_t` can
be assigned to `color_t`.

```cpp
using color_t = atom<"black"_a, "white"_a, "red"_a, "green"_a, "blue"_a>;

bw_t bw = "black"_a;
color_t color = bw;
```

Inverse assignment, `bw = color;` is not allowed. You can use `atom::to()` to
attempt the conversion anyway, the function returns `nullopt` if the atom cannot
be represented in the target type.

```cpp
color = "green"_a;
assert(color.to<bw_t>() == std::nullopt);

color = "white"_a;
assert(color.to<bw_t>() == "white"_a);
```

If you only want to check for `nullopt`, you can instead use `atom::is()`.

```cpp
color = "white"_a;
assert(color.is<bw_t>());
```

## Large atom types

Instead of specifying all atoms in the template argument list, you can instead
pass a pointer to an array of strings.

```cpp
constexpr char const * cga_keys[] = {
    "black", "blue", "green", "cyan",
    "red", "magenta", "brown", "light gray",
    "dark gray", "light blue", "light green", "light cyan",
    "light red", "light magenta", "yellow", "white",
};

using cga_t = atom<&cga_keys>;
```

You can combine atoms and pointers to string arrays arbitrarily.

```cpp
using grayscale_t = atom<&bw_t::keys, "light gray"_a, "dark gray"_a>;
static_assert(grayscale_t::keys.size() == 4);
```

## CMake integration

Add the library as a submodule of yours and include it via FetchContent.

```
include(FetchContent)
FetchContent_Declare(avakar.atom SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/atom")
FetchContent_MakeAvailable(avakar.atom)
```

Then, link to `avakar::atom` target.

```
target_link_dependency(my_target PUBLIC avakar::atom)
```

## Debugging

Objects of `atom` type contain a single integer. To see the associated string in
the debugger, you have to refer to the `keys` string list associated with the
type. E.g. to examine an object `x` of atom type, you should ask the debugger
for `x.keys[x._v]`.

For Visual Studio debugger, a .natvis file is part of this package. If you link
against the CMake target, the .natvis file will be automatically linked to your
executable.

When specifying the atoms directly in the template argument list, the length of
the name of the class symbol will soon get out of hand, ultimately resulting in
truncations and debugging issues. It is highly recommended that lists containing
more than a couple of atoms instead be put in a string array.

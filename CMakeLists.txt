cmake_minimum_required(VERSION 3.19)
project(avakar.atom)

include(FetchContent)
FetchContent_Declare(avakar.mutest SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/deps/mutest")
FetchContent_MakeAvailable(avakar.mutest)

add_library(avakar.atom INTERFACE
	include/avakar/atom.h
	src/atom.natvis
	src/extract_keys.h
	src/keylist.h
	src/literal.h
	src/size_least.h
	src/traits.h
	src/utils.h
	)
add_library(avakar::atom ALIAS avakar.atom)

target_include_directories(avakar.atom INTERFACE include)
target_compile_features(avakar.atom INTERFACE cxx_std_20)

if (MSVC)
	set_target_properties(avakar.atom PROPERTIES INTERFACE_LINK_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/atom.natvis")
	target_link_options(
		avakar.atom INTERFACE
		"/natvis:${CMAKE_CURRENT_SOURCE_DIR}/src/atom.natvis"
		)
endif()

add_mutest(test/test.cpp avakar::atom)

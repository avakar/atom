#include <avakar/atom.h>
#include <avakar/mutest.h>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <vector>

using avakar::atom;
using namespace avakar::literals;

using bw = atom<"black"_a, "white"_a>;
using rgb = atom<&bw::keys, "red"_a, "green"_a, "blue"_a>;

mutest_case("keys should be sorted")
{
	chk bw::keys.size() == 2;
	chk bw::keys[0] == "black";
	chk bw::keys[1] == "white";

	chk rgb::keys.size() == 5;
	chk rgb::keys[0] == "black";
	chk rgb::keys[1] == "blue";
	chk rgb::keys[2] == "green";
	chk rgb::keys[3] == "red";
	chk rgb::keys[4] == "white";
}

mutest_case("atoms can be iterated over")
{
	std::vector<bw> all;
	for (auto a: bw::iota())
		all.push_back(a);
	chk all.size() == 2;
	chk all[0] == "black"_a;
	chk all[1] == "white"_a;
}

mutest_case("atom::iota() returns a valid range")
{
	chk std::ranges::size(bw::iota()) == 2;

	std::vector<rgb> all;
	std::ranges::copy(rgb::iota(), std::back_inserter(all));
	chk all == std::vector<rgb>{"black"_a, "blue"_a, "green"_a, "red"_a, "white"_a};
}

mutest_case("successful atom::from_string")
{
	auto x = rgb::from_string("green");
	chk x.has_value();
	chk x->value() == 2;
}

mutest_case("failing atom::from_string")
{
	auto x = rgb::from_string("pink");
	chk !x.has_value();
}

mutest_case("equality compare")
{
	rgb x = "red"_a;
	chk x == "red"_a;
	chk x != "green"_a;
}

mutest_case("relational compare")
{
	rgb x = "red"_a;
	chk x == "red"_a;
	chk x > "green"_a;
	chk x < "white"_a;
}

mutest_case("atom::from_value")
{
	rgb x = rgb::from_value(2);
	chk x == "green"_a;
	chk x != "red"_a;
}

mutest_case("copy")
{
	rgb x = "green"_a;
	rgb y = x;
	chk x == y;
	chk y == "green"_a;
}

mutest_case("assing subtype")
{
	bw x = "white"_a;
	rgb y = x;
	chk y == "white"_a;
}

mutest_case("inverse assign")
{
	chk !std::is_assignable_v<bw, rgb>;
}

mutest_case("atom::to_string")
{
	rgb x = "green"_a;
	chk x.to_string() == "green";
}

constexpr char const * byte_names[] = {
	"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0a", "0b", "0c", "0d", "0e", "0f",
	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "1a", "1b", "1c", "1d", "1e", "1f",
	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29", "2a", "2b", "2c", "2d", "2e", "2f",
	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39", "3a", "3b", "3c", "3d", "3e", "3f",
	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49", "4a", "4b", "4c", "4d", "4e", "4f",
	"50", "51", "52", "53", "54", "55", "56", "57", "58", "59", "5a", "5b", "5c", "5d", "5e", "5f",
	"60", "61", "62", "63", "64", "65", "66", "67", "68", "69", "6a", "6b", "6c", "6d", "6e", "6f",
	"70", "71", "72", "73", "74", "75", "76", "77", "78", "79", "7a", "7b", "7c", "7d", "7e", "7f",
	"80", "81", "82", "83", "84", "85", "86", "87", "88", "89", "8a", "8b", "8c", "8d", "8e", "8f",
	"90", "91", "92", "93", "94", "95", "96", "97", "98", "99", "9a", "9b", "9c", "9d", "9e", "9f",
	"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af",
	"b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", "b8", "b9", "ba", "bb", "bc", "bd", "be", "bf",
	"c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7", "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf",
	"d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "da", "db", "dc", "dd", "de", "df",
	"e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7", "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef",
	"f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7", "f8", "f9", "fa", "fb", "fc", "fd", "fe", "ff",
};

using bytes = atom<&byte_names>;
using bytes100 = atom<&byte_names, "100"_a>;

mutest_case("size should adapt")
{
	chk sizeof(rgb) == 1;
	chk sizeof(bytes) == 1;
	chk sizeof(bytes100) == 2;
}

mutest_case("atom::is")
{
	rgb x = "green"_a;
	chk !x.is<bw>();
	chk x.is<rgb>();
}

mutest_case("atom::to fail")
{
	rgb x = "green"_a;
	auto y = x.to<bw>();
	chk y == std::nullopt;
}

mutest_case("atom::to succ")
{
	rgb x = "white"_a;
	auto y = x.to<bw>();
	chk y == "white"_a;
}

mutest_case("atom::to fail large")
{
	bytes100 x = "100"_a;
	auto y = x.to<bytes>();
	chk !y.has_value();
}

mutest_case("atom::to succ large")
{
	bytes100 x = "ff"_a;
	auto y = x.to<bytes>();
	chk y.has_value();
	chk *y == "ff"_a;
}

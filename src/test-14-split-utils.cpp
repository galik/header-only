//
// Copyright (c) 2016 Galik <galik.bool@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <string>
#include <vector>
#include <hol/string_utils.h>
#include <hol/experimental/utility_splitters.h>

namespace hol {
	using namespace header_only_library::string_utils;
}

TEST_CASE("strings be splitted", "string_utils")
{
	auto split_copy = hol::get_split_copier();

	auto empty = std::vector<std::string>{};
	auto x = std::vector<std::string>{"x"};
	auto a_b = std::vector<std::string>{"a", "b"};

	auto res = [](std::initializer_list<std::string> ilist)
	{
		return std::vector<std::string>(ilist);
	};

	SECTION("folding split matches")
	{
		split_copy.fold_matches();

		REQUIRE(split_copy.at_delim("", ' ') == res({}));
		REQUIRE(split_copy.at_delim("x", ' ') == x);
		REQUIRE(split_copy.at_delim(" x", ' ') == x);
		REQUIRE(split_copy.at_delim("x ", ' ') == x);
		REQUIRE(split_copy.at_delim(" x ", ' ') == x);
		REQUIRE(split_copy.at_delim("x  ", ' ') == x);
		REQUIRE(split_copy.at_delim("  x", ' ') == x);
		REQUIRE(split_copy.at_delim("  x  ", ' ') == x);
		REQUIRE(split_copy.at_delim("a b", ' ') == a_b);
		REQUIRE(split_copy.at_delim(" a b", ' ') == a_b);
		REQUIRE(split_copy.at_delim("a b ", ' ') == a_b);
		REQUIRE(split_copy.at_delim("  a b", ' ') == a_b);
		REQUIRE(split_copy.at_delim("a b  ", ' ') == a_b);
		REQUIRE(split_copy.at_delim("a  b", ' ') == a_b);
	}

	SECTION("non-folding split matches")
	{
		split_copy.dont_fold_matches();

		REQUIRE(split_copy.at_delim("", ' ') == empty);
		REQUIRE(split_copy.at_delim("x", ' ') == x);
		REQUIRE(split_copy.at_delim(" x", ' ') == x);
		REQUIRE(split_copy.at_delim("x ", ' ') == x);
		REQUIRE(split_copy.at_delim(" x ", ' ') == x);
		REQUIRE(split_copy.at_delim("x  ", ' ') == x);
		REQUIRE(split_copy.at_delim("  x", ' ') == x);
		REQUIRE(split_copy.at_delim("  x  ", ' ') == x);
		REQUIRE(split_copy.at_delim("a b", ' ') == a_b);
		REQUIRE(split_copy.at_delim(" a b", ' ') == a_b);
		REQUIRE(split_copy.at_delim("a b ", ' ') == a_b);
		REQUIRE(split_copy.at_delim("  a b", ' ') == a_b);
		REQUIRE(split_copy.at_delim("a b  ", ' ') == a_b);
		REQUIRE(split_copy.at_delim("a  b", ' ') == a_b);
	}

	SECTION("Definitive Split Algorithm")
	{
		REQUIRE(hol::split("", " ") == empty);
		REQUIRE(hol::split("x", " ") == x);
		REQUIRE(hol::split(" x", " ") == x);
		REQUIRE(hol::split("x ", " ") == x);
		REQUIRE(hol::split(" x ", " ") == x);
		REQUIRE(hol::split("x  ", " ") == x);
		REQUIRE(hol::split("  x", " ") == x);
		REQUIRE(hol::split("  x  ", " ") == x);
		REQUIRE(hol::split("a b", " ") == a_b);
		REQUIRE(hol::split(" a b", " ") == a_b);
		REQUIRE(hol::split("a b ", " ") == a_b);
		REQUIRE(hol::split("  a b", " ") == a_b);
		REQUIRE(hol::split("a b  ", " ") == a_b);
		REQUIRE(hol::split("a  b", " ") == a_b);
	}

//	delim: ' ' "" {}
//	delim: ' ' "x" a_b);
//	delim: ' ' " x" {"x"}
//	delim: ' ' "x " {"x"}
//	delim: ' ' " x " {"x"}
//	delim: ' ' "x  " {"x"}
//	delim: ' ' "  x" {"x"}
//	delim: ' ' "  x  " {"x"}
//	delim: ' ' "a b" {"a" "b"}
//	delim: ' ' " a b" {"a" "b"}
//	delim: ' ' "a b " {"a" "b"}
//	delim: ' ' "  a b" {"a" "b"}
//	delim: ' ' "a b  " {"a" "b"}
//	delim: ' ' "a  b" {"a" "b"}
//	delim: ' ' "aaa bbb" {"aaa" "bbb"}
//	delim: ' ' " aaa bbb" {"aaa" "bbb"}
//	delim: ' ' "aaa bbb " {"aaa" "bbb"}
//	delim: ' ' "  aaa bbb" {"aaa" "bbb"}
//	delim: ' ' "aaa bbb  " {"aaa" "bbb"}
//	delim: ' ' "aaa  bbb" {"aaa" "bbb"}

}

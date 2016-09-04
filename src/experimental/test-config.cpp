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
#include <hol/experimental/config.h>
#include <hol/string_utils.h>
#include <hol/experimental/utility_splitters.h>

using str_vec = std::vector<std::string>;

auto split_copy = hol::get_split_copier();

TEST_CASE("config testing", "config")
{
	SECTION("folding split matches")
	{
		std::string config_data = R"~(
		key01: val1
		key01: val2

		key02: 1
		key02: 2
		)~";
		std::istringstream iss(config_data);
		hol::Config cfg{iss};

		REQUIRE(cfg.get("key01") == "val1");
//		REQUIRE(cfg.get<str_vec>("key01") == str_vec{"val1", "val2"});
	}

	SECTION("non-folding split matches")
	{
		split_copy.dont_fold_matches();

//		REQUIRE(split_copy.at_delim("", ' ') == empty);
//		REQUIRE(split_copy.at_delim("x", ' ') == x);
//		REQUIRE(split_copy.at_delim(" x", ' ') == x);
//		REQUIRE(split_copy.at_delim("x ", ' ') == x);
//		REQUIRE(split_copy.at_delim(" x ", ' ') == x);
//		REQUIRE(split_copy.at_delim("x  ", ' ') == x);
//		REQUIRE(split_copy.at_delim("  x", ' ') == x);
//		REQUIRE(split_copy.at_delim("  x  ", ' ') == x);
//		REQUIRE(split_copy.at_delim("a b", ' ') == a_b);
//		REQUIRE(split_copy.at_delim(" a b", ' ') == a_b);
//		REQUIRE(split_copy.at_delim("a b ", ' ') == a_b);
//		REQUIRE(split_copy.at_delim("  a b", ' ') == a_b);
//		REQUIRE(split_copy.at_delim("a b  ", ' ') == a_b);
//		REQUIRE(split_copy.at_delim("a  b", ' ') == a_b);
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

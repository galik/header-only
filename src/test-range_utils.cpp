//
// Copyright (c) 2017 Galik <galik.bool@gmail.com>
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

//#include <algorithm>
#include <string>
//#include <vector>

//#include "test.h"
#include "hol/bug.h"
//#include "hol/random_numbers.h"
#include "hol/range_utils.h"
//#include "hol/unicode_utils.h"

namespace hol{
//	using namespace header_only_library::random_numbers;
	using namespace header_only_library::range_utils;
//	using namespace header_only_library::unicode_utils;
}

//using namespace std::literals;

TEST_CASE("String operations", "[]")
{
	SECTION("r1.size() == r2.size()")
	{
		std::string s1 = "abcdef";
		std::string s2 = "ABCDEF";

		auto r1 = hol::make_range(s1);
		auto r2 = hol::make_range(s2);

		hol::copy(r1, r2);

		REQUIRE(s2 == "abcdef");
	}

	SECTION("r1.size() < r2.size()")
	{
		std::string s1 = "abc";
		std::string s2 = "ABCDEF";

		auto r1 = hol::make_range(s1);
		auto r2 = hol::make_range(s2);

		hol::copy(r1, r2);

		REQUIRE(s2 == "abcDEF");
	}

	SECTION("r1.size() > r2.size()")
	{
		std::string s1 = "abcdef";
		std::string s2 = "ABC";

		auto r1 = hol::make_range(s1);
		auto r2 = hol::make_range(s2);

		hol::copy(r1, r2);

		REQUIRE(s2 == "abc");
	}
}

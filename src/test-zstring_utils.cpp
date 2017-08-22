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

#include <algorithm>
#include <string>
#include <vector>

#include "test.h"
#include "hol/bug.h"
#include "hol/random_numbers.h"
#include "hol/string_utils.h"
#include "hol/unicode_utils.h"
#include "../../hol/zstring_utils.h"

namespace hol {
	using namespace header_only_library::random_numbers;
	using namespace header_only_library::unicode_utils;
	using namespace header_only_library::zstring_utils;
}

namespace gen {
	using namespace header_only_library::generic;
} // namespace gen

TEST_CASE("ZString Utils", "[comparing]")
{
	SECTION("zstrings_are_equal")
	{
		{
			REQUIRE(hol::equal_to("", ""));
			REQUIRE(hol::equal_to("a", "a"));
			REQUIRE(hol::equal_to("ab", "ab"));
			REQUIRE_FALSE(hol::equal_to("a", ""));
			REQUIRE_FALSE(hol::equal_to("", "a"));
			REQUIRE_FALSE(hol::equal_to("a", "b"));
			REQUIRE_FALSE(hol::equal_to("ab", "ac"));
		}
	}
}

TEST_CASE("Generic Utils", "")
{
	SECTION("strcpy")
	{
		std::array<char, 10> a1;
		std::array<char, 10> a2;
		std::array<char, 10> a3;

		for(auto n = 0ULL; i < 1000; ++n)
		{
			std::generate_n(std::begin(a1), std::end(a1), []{ hol::random_number('A', 'Z'); });

			std::strcpy(a2.data(), a1.data());
			gen::strcpy((a3.data(), a1.data());

			REQUIRE(a2 == a3);
		}
	}
}




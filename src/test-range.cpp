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

#include "hol/bug.h"
#include "hol/range.h"

namespace hol{
	using namespace header_only_library::range;
}

//using namespace std::literals;
TEST_CASE("Making ranges", "[]")
{
	SECTION("Container")
	{
		std::vector<int> v{0, 1, 2};
		auto r = hol::make_range(v);

		REQUIRE(r[0] == v[0]);
		REQUIRE(r[1] == v[1]);
		REQUIRE(r[2] == v[2]);

		r[1] = 9;
		REQUIRE(v[1] == 9);
	}

	SECTION("Container const")
	{
		std::vector<int> const v{0, 1, 2};
		auto r = hol::make_range(v);

		REQUIRE(r[0] == v[0]);
		REQUIRE(r[1] == v[1]);
		REQUIRE(r[2] == v[2]);
	}
}

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
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#define HOL_RANGE_THROW_ON_EXCEPTION
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

		REQUIRE(r.size() == v.size());
		REQUIRE(r[0] == v[0]);
		REQUIRE(r[1] == v[1]);
		REQUIRE(r[2] == v[2]);

		REQUIRE(std::accumulate(std::begin(r), std::end(r), 0) == 3);

		r[1] = 9;
		REQUIRE(v[1] == 9);

		REQUIRE_THROWS(r[3]);
	}

	SECTION("Container const")
	{
		std::vector<int> const v{0, 1, 2};
		auto r = hol::make_range(v);

		REQUIRE(r.size() == v.size());
		REQUIRE(r[0] == v[0]);
		REQUIRE(r[1] == v[1]);
		REQUIRE(r[2] == v[2]);

		REQUIRE(std::accumulate(std::begin(r), std::end(r), 0) == 3);

		REQUIRE_THROWS(r[3]);
	}

	SECTION("Array")
	{
		int v[] = {0, 1, 2};
		auto r = hol::make_range(v);

		REQUIRE(r.size() == 3);
		REQUIRE(r[0] == v[0]);
		REQUIRE(r[1] == v[1]);
		REQUIRE(r[2] == v[2]);

		REQUIRE(std::accumulate(std::begin(r), std::end(r), 0) == 3);

		r[1] = 9;
		REQUIRE(v[1] == 9);

		REQUIRE_THROWS(r[3]);
	}

	SECTION("Array const")
	{
		int const v[] = {0, 1, 2};
		auto r = hol::make_range(v);

		REQUIRE(r.size() == 3);
		REQUIRE(r[0] == v[0]);
		REQUIRE(r[1] == v[1]);
		REQUIRE(r[2] == v[2]);

		REQUIRE(std::accumulate(std::begin(r), std::end(r), 0) == 3);

		REQUIRE_THROWS(r[3]);
	}

	SECTION("Buffer (size)")
	{
		std::vector<int> v{0, 1, 2};
		auto r = hol::make_range(v.data(), v.size());

		REQUIRE(r.size() == 3);
		REQUIRE(r[0] == v[0]);
		REQUIRE(r[1] == v[1]);
		REQUIRE(r[2] == v[2]);

		REQUIRE(std::accumulate(std::begin(r), std::end(r), 0) == 3);

		r[1] = 9;
		REQUIRE(v[1] == 9);

		REQUIRE_THROWS(r[3]);
	}

	SECTION("Buffer const (size)")
	{
		std::vector<int> const v{0, 1, 2};
		auto r = hol::make_range(v.data(), v.size());

		REQUIRE(r.size() == 3);
		REQUIRE(r[0] == v[0]);
		REQUIRE(r[1] == v[1]);
		REQUIRE(r[2] == v[2]);

		REQUIRE(std::accumulate(std::begin(r), std::end(r), 0) == 3);

		REQUIRE_THROWS(r[3]);
	}

	SECTION("Buffer (pointer)")
	{
		std::vector<int> v{0, 1, 2};
		auto r = hol::make_range(v.data(), v.data() + v.size());

		REQUIRE(r.size() == 3);
		REQUIRE(r[0] == v[0]);
		REQUIRE(r[1] == v[1]);
		REQUIRE(r[2] == v[2]);

		REQUIRE(std::accumulate(std::begin(r), std::end(r), 0) == 3);

		r[1] = 9;
		REQUIRE(v[1] == 9);

		REQUIRE_THROWS(r[3]);
	}

	SECTION("Buffer const (pointer)")
	{
		std::vector<int> const v{0, 1, 2};
		auto r = hol::make_range(v.data(), v.data() + v.size());

		REQUIRE(r.size() == 3);
		REQUIRE(r[0] == v[0]);
		REQUIRE(r[1] == v[1]);
		REQUIRE(r[2] == v[2]);

		REQUIRE(std::accumulate(std::begin(r), std::end(r), 0) == 3);

		REQUIRE_THROWS(r[3]);
	}
}

TEST_CASE("Free functions", "")
{
	SECTION("non const")
	{
		std::vector<int> v = {0, 1, 2};
		hol::range<int> r = hol::make_range(v);

		REQUIRE(hol::front(r) == 0);
		REQUIRE(hol::at(r, 1) == 1);
		REQUIRE(hol::back(r)  == 2);

		REQUIRE(hol::data(r) == v.data());
		REQUIRE(hol::size(r) == 3);
		REQUIRE(hol::length(r) == 3);

		hol::range<int> r2 = r;
		hol::clear(r2);
		REQUIRE(hol::empty(r2));

		std::vector<int> v2 = {5, 6};
		r2 = hol::make_range(v2);

		bug_var(r.size());
		bug_var(r2.size());

		REQUIRE_THROWS(hol::copy(r, r2, 4));

		hol::copy(r, r2, 1);

		REQUIRE(r2[0] == 1);
		REQUIRE(r2[1] == 2);
		REQUIRE_THROWS(r2[2]);
	}

	SECTION("const")
	{
		bug_scope("const");
		std::vector<int> const v = {0, 1, 2};
		hol::range<int const> r = hol::make_range(v);

		REQUIRE(hol::front(r) == 0);
		REQUIRE(hol::at(r, 1) == 1);
		REQUIRE(hol::back(r)  == 2);

		REQUIRE(hol::data(r) == v.data());
		REQUIRE(hol::size(r) == 3);
		REQUIRE(hol::length(r) == 3);

		hol::range<int const> re = r;
		hol::clear(re);
		REQUIRE(hol::empty(re));

		std::vector<int> v2 = {5, 6};
		hol::range<int> r2 = hol::make_range(v2);

		bug_var(r.size());
		bug_var(r2.size());

		REQUIRE_THROWS(hol::copy(r, r2, 4));

		hol::copy(r, r2, 1);

		REQUIRE(r2[0] == 1);
		REQUIRE(r2[1] == 2);
		REQUIRE_THROWS(r2[2]);
	}
}








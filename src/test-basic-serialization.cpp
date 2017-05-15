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

#include <regex>
#include <cctype>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <algorithm>

#include "test.h"
#include "hol/random_numbers.h"
#include <hol/basic_serialization.h>

TEST_CASE("Basic Serialization [txt]", "[basic types]")
{
	using namespace hol::basic_serialization::txt;

	SECTION("positive ints")
	{
		short s = 1;
		int i = 2;
		long l = 3;
		long long ll = 4;

		std::stringstream ss;

		serialize(ss, s);
		REQUIRE(ss.str() == " 1");

		serialize(ss, i);
		REQUIRE(ss.str() == " 1 2");

		serialize(ss, l);
		REQUIRE(ss.str() == " 1 2 3");

		serialize(ss, ll);
		REQUIRE(ss.str() == " 1 2 3 4");

		REQUIRE(deserialize(ss, ll));
		REQUIRE(ll == 1);

		REQUIRE(deserialize(ss, l));
		REQUIRE(l == 2);

		REQUIRE(deserialize(ss, i));
		REQUIRE(i == 3);

		REQUIRE(deserialize(ss, s));
		REQUIRE(s == 4);
	}

	SECTION("negative ints")
	{
		short s = -1;
		int i = -2;
		long l = -3;
		long long ll = -4;

		std::stringstream ss;

		serialize(ss, s);
		REQUIRE(ss.str() == " -1");

		serialize(ss, i);
		REQUIRE(ss.str() == " -1 -2");

		serialize(ss, l);
		REQUIRE(ss.str() == " -1 -2 -3");

		serialize(ss, ll);
		REQUIRE(ss.str() == " -1 -2 -3 -4");

		deserialize(ss, ll);
		REQUIRE(ll == -1);

		deserialize(ss, l);
		REQUIRE(l == -2);

		deserialize(ss, i);
		REQUIRE(i == -3);

		deserialize(ss, s);
		REQUIRE(s == -4);
	}

	SECTION("floats")
	{
		float f = 0.3f;
		double d = 0.5;

		std::stringstream ss;

		serialize(ss, f);
		REQUIRE(ss.str() == " 0.3");

		serialize(ss, d);
		REQUIRE(ss.str() == " 0.3 0.5");
	}

	SECTION("std::string")
	{
		std::string s = "serialize me";

		std::stringstream ss;

		serialize(ss, s);
		REQUIRE(ss.str() == " 12 serialize me");

		deserialize(ss, s);
		REQUIRE(s == "serialize me");

		ss.str("");
		s = "";

		serialize(ss, s);
		REQUIRE(ss.str() == " 0 ");

		deserialize(ss, s);
		REQUIRE(s.empty());
	}

	SECTION("std::pair")
	{
		std::pair<std::string, int> p;

		std::stringstream ss;

		p = {"P", 3};

		serialize(ss, p);
		REQUIRE(ss.str() == " 1 P 3");

		std::pair<const std::string, int> cp {"CP", 9};

		ss.str("");

		serialize(ss, cp);
		REQUIRE(ss.str() == " 2 CP 9");
	}

	SECTION("std::map")
	{
		std::map<std::string, int> m;

		std::stringstream ss;

		m["A"] = 2;

		serialize(ss, m);
		REQUIRE(ss.str() == " 1 1 A 2");

		m.clear();

		REQUIRE(deserialize(ss, m));
		REQUIRE(m.find("A") != m.end());
		REQUIRE(m["A"] == 2);
	}
}








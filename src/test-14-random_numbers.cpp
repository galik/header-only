//
// Copyright (c) 2018 Galik <galik.bool@gmail.com>
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
#include <vector>

//#include "test.h"
#include "hol/bug.h"
#include "hol/random_numbers.h"

namespace hol{
	using namespace header_only_library::random_numbers;
}

TEST_CASE("types", "[]")
{
	SECTION("short")
	{
		std::mt19937 mt{1};
		std::uniform_int_distribution<short> pick
		{
			std::numeric_limits<short>::lowest(),
			std::numeric_limits<short>::max()
		};
		hol::random_reseed(1);
		for(auto n = 0; n < 1000000; ++n)
			REQUIRE(hol::random_number<short>() == pick(mt));
	}

	SECTION("int")
	{
		std::mt19937 mt{1};
		std::uniform_int_distribution<int> pick
		{
			std::numeric_limits<int>::lowest(),
			std::numeric_limits<int>::max()
		};
		hol::random_reseed(1);
		for(auto n = 0; n < 1000000; ++n)
			REQUIRE(hol::random_number<int>() == pick(mt));
	}

	SECTION("long")
	{
		std::mt19937 mt{1};
		std::uniform_int_distribution<long> pick
		{
			std::numeric_limits<long>::lowest(),
			std::numeric_limits<long>::max()
		};
		hol::random_reseed(1);
		for(auto n = 0; n < 1000000; ++n)
			REQUIRE(hol::random_number<long>() == pick(mt));
	}

	SECTION("long long")
	{
		std::mt19937 mt{1};
		std::uniform_int_distribution<long long> pick
		{
			std::numeric_limits<long long>::lowest(),
			std::numeric_limits<long long>::max()
		};
		hol::random_reseed(1);
		for(auto n = 0; n < 1000000; ++n)
			REQUIRE(hol::random_number<long long>() == pick(mt));
	}

	SECTION("unsigned short")
	{
		std::mt19937 mt{1};
		std::uniform_int_distribution<unsigned short> pick
		{
			std::numeric_limits<unsigned short>::lowest(),
			std::numeric_limits<unsigned short>::max()
		};
		hol::random_reseed(1);
		for(auto n = 0; n < 1000000; ++n)
			REQUIRE(hol::random_number<unsigned short>() == pick(mt));
	}

	SECTION("unsigned int")
	{
		std::mt19937 mt{1};
		std::uniform_int_distribution<unsigned int> pick
		{
			std::numeric_limits<unsigned int>::lowest(),
			std::numeric_limits<unsigned int>::max()
		};
		hol::random_reseed(1);
		for(auto n = 0; n < 1000000; ++n)
			REQUIRE(hol::random_number<unsigned int>() == pick(mt));
	}

	SECTION("unsigned long")
	{
		std::mt19937 mt{1};
		std::uniform_int_distribution<unsigned long> pick
		{
			std::numeric_limits<unsigned long>::lowest(),
			std::numeric_limits<unsigned long>::max()
		};
		hol::random_reseed(1);
		for(auto n = 0; n < 1000000; ++n)
			REQUIRE(hol::random_number<unsigned long>() == pick(mt));
	}

	SECTION("unsigned long long")
	{
		std::mt19937 mt{1};
		std::uniform_int_distribution<unsigned long long> pick
		{
			std::numeric_limits<unsigned long long>::lowest(),
			std::numeric_limits<unsigned long long>::max()
		};
		hol::random_reseed(1);
		for(auto n = 0; n < 1000000; ++n)
			REQUIRE(hol::random_number<unsigned long long>() == pick(mt));
	}
}

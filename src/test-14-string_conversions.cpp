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
//#include <vector>

//#include "test.h"
#include "hol/bug.h"
#include "hol/random_numbers.h"
#include "hol/string_utils.h"
//#include "hol/unicode_utils.h"

namespace hol{
	using namespace header_only_library::random_numbers;
	using namespace header_only_library::string_utils;
//	using namespace header_only_library::unicode_utils;
}

//using namespace std::literals;

template<typename Numeric>
using limits = std::numeric_limits<Numeric>;

long long int const max_short = static_cast<long long int>(limits<short>::max());
long long int const min_short = static_cast<long long int>(limits<short>::min());

long long int const max_int = static_cast<long long int>(limits<int>::max());
long long int const min_int = static_cast<long long int>(limits<int>::min());

long long int const max_long = static_cast<long long int>(limits<long>::max());
long long int const min_long = static_cast<long long int>(limits<long>::min());

long long int const max_long_long = static_cast<long long int>(limits<long long>::max());
long long int const min_long_long = static_cast<long long int>(limits<long long>::min());

unsigned long long int const max_unsigned_short = static_cast<unsigned long long int>(limits<unsigned short>::max());
unsigned long long int const min_unsigned_short = static_cast<unsigned long long int>(limits<unsigned short>::min());

unsigned long long int const max_unsigned_int = static_cast<unsigned long long int>(limits<unsigned int>::max());
unsigned long long int const min_unsigned_int = static_cast<unsigned long long int>(limits<unsigned int>::min());

unsigned long long int const max_unsigned_long = static_cast<unsigned long long int>(limits<unsigned long>::max());
unsigned long long int const min_unsigned_long = static_cast<unsigned long long int>(limits<unsigned long>::min());

unsigned long long int const max_unsigned_long_long = static_cast<unsigned long long int>(limits<unsigned long long>::max());
unsigned long long int const min_unsigned_long_long = static_cast<unsigned long long int>(limits<unsigned long long>::min());

template<typename T>
std::vector<T> random_buffer(std::size_t n, T min = limits<T>::lowest(), T max = limits<T>::max())
{
	std::vector<T> v;
	std::generate_n(std::back_inserter(v), n, [&]{ return hol::random_number(min, max); });
	return v;
}

TEST_CASE("Signed number conversion utils", "[signed]")
{
	SECTION("short")
	{
		short i = 0;
		REQUIRE(hol::s_to_i(std::to_string(max_short), i));
		REQUIRE(i == max_short);

		i = 0;
		REQUIRE(hol::s_to_i(std::to_string(min_short), i));
		REQUIRE(i == min_short);

		i = 0;
		REQUIRE_FALSE(hol::s_to_i(std::to_string(max_short + 1), i));
		REQUIRE(i == 0);

		i = 0;
		REQUIRE_FALSE(hol::s_to_i(std::to_string(min_short - 1), i));
		REQUIRE(i == 0);
	}

	SECTION("int")
	{
		int i = 0;
		REQUIRE(hol::s_to_i(std::to_string(max_int), i));
		REQUIRE(i == max_int);

		i = 0;
		REQUIRE(hol::s_to_i(std::to_string(min_int), i));
		REQUIRE(i == min_int);

		i = 0;
		REQUIRE_FALSE(hol::s_to_i(std::to_string(max_int + 1), i));
		REQUIRE(i == 0);

		i = 0;
		REQUIRE_FALSE(hol::s_to_i(std::to_string(min_int - 1), i));
		REQUIRE(i == 0);
	}

	SECTION("long")
	{
		long i = 0;
		REQUIRE(hol::s_to_i(std::to_string(max_long), i));
		REQUIRE(i == max_long);

		i = 0;
		REQUIRE(hol::s_to_i(std::to_string(min_long), i));
		REQUIRE(i == min_long);

		if(sizeof(long) < sizeof(long long))
		{
			i = 0;
			REQUIRE_FALSE(hol::s_to_i(std::to_string(max_long + 1), i));
			REQUIRE(i == 0);

			i = 0;
			REQUIRE_FALSE(hol::s_to_i(std::to_string(min_long - 1), i));
			REQUIRE(i == 0);
		}
	}

	if(sizeof(long) < sizeof(long long))
	{
		SECTION("long")
		{
			long long i = 0;
			REQUIRE(hol::s_to_i(std::to_string(max_long_long), i));
			REQUIRE(i == max_long_long);

			i = 0;
			REQUIRE(hol::s_to_i(std::to_string(min_long_long), i));
			REQUIRE(i == min_long_long);
		}
	}
}


TEST_CASE("Unsigned number conversion utils", "[unsigned]")
{
	SECTION("unsigned short")
	{
		unsigned short i = 0;
		REQUIRE(hol::s_to_u(std::to_string(max_unsigned_short), i));
		REQUIRE(i == max_unsigned_short);

		i = 0;
		REQUIRE(hol::s_to_u(std::to_string(min_unsigned_short), i));
		REQUIRE(i == min_unsigned_short);

		i = 0;
		REQUIRE_FALSE(hol::s_to_u(std::to_string(max_unsigned_short + 1), i));
		REQUIRE(i == 0);

		i = 0;
		REQUIRE_FALSE(hol::s_to_u(std::to_string(min_unsigned_short - 1), i));
		REQUIRE(i == 0);
	}

	SECTION("unsigned int")
	{
		unsigned int i = 0;
		REQUIRE(hol::s_to_u(std::to_string(max_unsigned_int), i));
		REQUIRE(i == max_unsigned_int);

		i = 0;
		REQUIRE(hol::s_to_u(std::to_string(min_unsigned_int), i));
		REQUIRE(i == min_unsigned_int);

		i = 0;
		REQUIRE_FALSE(hol::s_to_u(std::to_string(max_unsigned_int + 1), i));
		REQUIRE(i == 0);

		i = 0;
		REQUIRE_FALSE(hol::s_to_u(std::to_string(min_unsigned_int - 1), i));
		REQUIRE(i == 0);
	}

	SECTION("unsigned long")
	{
		unsigned long i = 0;
		REQUIRE(hol::s_to_u(std::to_string(max_unsigned_long), i));
		REQUIRE(i == max_unsigned_long);

		i = 0;
		REQUIRE(hol::s_to_u(std::to_string(min_unsigned_long), i));
		REQUIRE(i == min_unsigned_long);

		if(sizeof(unsigned long) < sizeof(unsigned long long))
		{
			i = 0;
			REQUIRE_FALSE(hol::s_to_u(std::to_string(max_unsigned_long + 1), i));
			REQUIRE(i == 0);

			i = 0;
			REQUIRE_FALSE(hol::s_to_u(std::to_string(min_unsigned_long - 1), i));
			REQUIRE(i == 0);
		}
	}

	if(sizeof(unsigned long) < sizeof(unsigned long long))
	{
		SECTION("unsigned long")
		{
			unsigned long long i = 0;
			REQUIRE(hol::s_to_u(std::to_string(max_unsigned_long_long), i));
			REQUIRE(i == max_unsigned_long_long);

			i = 0;
			REQUIRE(hol::s_to_u(std::to_string(min_unsigned_long_long), i));
			REQUIRE(i == min_unsigned_long_long);
		}
	}
}

constexpr std::size_t const N = 1000ULL;

TEST_CASE("Random testing", "[random]")
{
	SECTION("short")
	{
		using int_type = short;

		auto v = random_buffer<int_type>(N);

		for(auto i: v)
		{
			auto s = std::to_string(i);
			int_type n;
			REQUIRE(hol::s_to_i(s, n));
			REQUIRE(i == n);
		}
	}

	SECTION("int")
	{
		using int_type = int;

		auto v = random_buffer<int_type>(N);

		for(auto i: v)
		{
			auto s = std::to_string(i);
			int_type n;
			REQUIRE(hol::s_to_i(s, n));
			REQUIRE(i == n);
		}
	}

	SECTION("long")
	{
		using int_type = long;

		auto v = random_buffer<int_type>(N);

		for(auto i: v)
		{
			auto s = std::to_string(i);
			int_type n;
			REQUIRE(hol::s_to_i(s, n));
			REQUIRE(i == n);
		}
	}

	SECTION("long long")
	{
		using int_type = long long;

		auto v = random_buffer<int_type>(N);

		for(auto i: v)
		{
			auto s = std::to_string(i);
			int_type n;
			REQUIRE(hol::s_to_i(s, n));
			REQUIRE(i == n);
		}
	}

	SECTION("unsigned short")
	{
		using int_type = unsigned short;

		auto v = random_buffer<int_type>(N);

		for(auto i: v)
		{
			auto s = std::to_string(i);
			int_type n;
			REQUIRE(hol::s_to_u(s, n));
			REQUIRE(i == n);
		}
	}

	SECTION("unsigned int")
	{
		using int_type = unsigned int;

		auto v = random_buffer<int_type>(N);

		for(auto i: v)
		{
			auto s = std::to_string(i);
			int_type n;
			REQUIRE(hol::s_to_u(s, n));
			REQUIRE(i == n);
		}
	}

	SECTION("unsigned long")
	{
		using int_type = unsigned long;

		auto v = random_buffer<int_type>(N);

		for(auto i: v)
		{
			auto s = std::to_string(i);
			int_type n;
			REQUIRE(hol::s_to_u(s, n));
			REQUIRE(i == n);
		}
	}

	SECTION("unsigned long long")
	{
		using int_type = unsigned long long;

		auto v = random_buffer<int_type>(N);

		for(auto i: v)
		{
			auto s = std::to_string(i);
			int_type n;
			REQUIRE(hol::s_to_u(s, n));
			REQUIRE(i == n);
		}
	}
}

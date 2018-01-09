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

#include <hol/bug.h>
#include <hol/random_numbers.h>
#include <hol/endian_utils.h>

namespace hol{
	using namespace header_only_library::random_numbers;
	using namespace header_only_library::endian_utils;
}

TEST_CASE("endianness", "[]")
{
	SECTION("enquire")
	{
		int i = 1;
		hol::endian e = ((char*)&i) ? hol::endian::little : hol::endian::big;

		REQUIRE(e == hol::get_endianness());

		std::uint32_t n = 0x01020304;

		if(hol::is_big_endian())
		{
			REQUIRE(e == hol::endian::big);
			REQUIRE(hol::discover_endianness(n, 0x01020304U) == hol::endian::big);
		}
		else if(hol::is_little_endian())
		{
			REQUIRE(e == hol::endian::little);
			REQUIRE(hol::discover_endianness(n, 0x01020304U) == hol::endian::little);
		}
	}

	SECTION("manipulate")
	{
		std::uint32_t n = 0x01020304;
		REQUIRE(hol::swap_endianness(n) == 0x04030201);

		if(hol::is_big_endian())
		{
			REQUIRE(hol::from_big_endian(n) == 0x01020304);
			REQUIRE(hol::from_little_endian(n) == 0x04030201);
			REQUIRE(hol::from_endian(hol::endian::big, n) == 0x01020304);
			REQUIRE(hol::from_endian(hol::endian::little, n) == 0x04030201);
		}
		else if(hol::is_little_endian())
		{
			REQUIRE(hol::from_little_endian(n) == 0x01020304);
			REQUIRE(hol::from_big_endian(n) == 0x04030201);
			REQUIRE(hol::from_endian(hol::endian::little, n) == 0x01020304);
			REQUIRE(hol::from_endian(hol::endian::big, n) == 0x04030201);
		}
	}

	SECTION("network encoding/decoding")
	{
		std::uint32_t n = 0x01020304;
		char buf[sizeof(n)];

		char const* end = hol::encode_network_byte_order(n, buf);
		REQUIRE(end == buf + sizeof(buf));

		if(hol::is_big_endian())
		{
			char fix[sizeof(n)] = {0x04, 0x03, 0x02, 0x01};
			REQUIRE(std::equal(std::begin(buf), std::end(buf), std::begin(fix), std::end(fix)));
		}
		else if(hol::is_little_endian())
		{
			char fix[sizeof(n)] = {0x01, 0x02, 0x03, 0x04};
			REQUIRE(std::equal(std::begin(buf), std::end(buf), std::begin(fix), std::end(fix)));
		}

		std::uint32_t o;
		end = hol::decode_network_byte_order(buf, o);
		REQUIRE(end == buf + sizeof(buf));
		REQUIRE(n == o);
	}
}


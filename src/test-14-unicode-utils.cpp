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

#include "hol/unicode_utils.h"

namespace hol {
	using namespace header_only_library::unicode_utils;
}

TEST_CASE("Unicode Conversions", "unicode_utils")
{
	SECTION("to_utf8")
	{
		REQUIRE(hol::ucs2_to_utf8(u"\u4F60\u597D") == u8"\xE4\xBD\xA0\xE5\xA5\xBD");
		REQUIRE(hol::ucs4_to_utf8(U"\U0001d11e") == u8"\xF0\x9D\x84\x9E");
		REQUIRE(hol::utf16_to_utf8(u"\U0001d11e") == u8"\xF0\x9D\x84\x9E");
		REQUIRE(hol::utf32_to_utf8(U"\U00004F60\U0000597D") == u8"\xE4\xBD\xA0\xE5\xA5\xBD");
		REQUIRE(hol::ws_to_utf8(L"\u4F60\u597D") == u8"\xE4\xBD\xA0\xE5\xA5\xBD");
	}
	SECTION("from_utf8")
	{
		REQUIRE(hol::utf8_to_ucs2(u8"\xE4\xBD\xA0\xE5\xA5\xBD") == u"\u4F60\u597D");
		REQUIRE(hol::utf8_to_ucs4(u8"\xF0\x9D\x84\x9E") == U"\U0001d11e");
		REQUIRE(hol::utf8_to_utf16(u8"\xF0\x9D\x84\x9E") == u"\U0001d11e");
		REQUIRE(hol::utf8_to_utf32(u8"\xE4\xBD\xA0\xE5\xA5\xBD") == U"\U00004F60\U0000597D");
		REQUIRE(hol::utf8_to_ws(u8"\xE4\xBD\xA0\xE5\xA5\xBD") == L"\u4F60\u597D");
	}
}



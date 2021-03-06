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
#include <vector>

//#include "test.h"
#include "hol/bug.h"
#include "hol/random_numbers.h"
#include "hol/rope.h"
#include "hol/unicode_utils.h"

namespace hol {
	using namespace header_only_library::random_numbers;
	using namespace header_only_library::rope_utils;
	using namespace header_only_library::unicode_utils;
}

using namespace std::literals;
using namespace hol::literals;

std::string random_text(std::size_t n)
{
	std::string s;
	s.reserve(n);
	std::generate_n(std::back_inserter(s), n, []{ return hol::random_number(unsigned('A'), unsigned('Z')); });
	return s;
}

TEST_CASE("String operations", "[]")
{
	SECTION("r1.size() == r2.size()")
	{
		hol::rope s1 = "abcdef";
		hol::rope s2 = "abcdef";

		REQUIRE(s1 == s2);
		REQUIRE_FALSE(s1 != s2);
	}

	SECTION("chopstix")
	{
		auto text = random_text(1000);

//		hol::random_reseed(1);

		hol::rope r = text;
		std::string s = text;

		for(auto i = 0; i < 1000; ++i)
		{
			switch(hol::random_number(3))
			{
				case 0:
				{
					auto b = hol::random_number(s.size() - 1);
					auto e = hol::random_number(b, hol::random_number(100UL));
					if(e > s.size() - 1)
						e = s.size() - 1;

					r = r.substr(b, e - b);
					s = s.substr(b, e - b);
					break;
				}
				case 1:
				{
					auto text = random_text(hol::random_number(100));
					r.append(std::begin(text), std::end(text));
					s.append(std::begin(text), std::end(text));
					break;
				}
				case 2:
				{
					auto text = random_text(hol::random_number(100));
					auto pos = hol::random_number(s.size() - 1);

//					r.insert(std::next(std::begin(s), pos), std::begin(text), std::end(text));
					s.insert(std::next(std::begin(s), pos), std::begin(text), std::end(text));
					break;
				}
				case 3:
					break;
			}
		}

//		REQUIRE(s1 == s2);
//		REQUIRE_FALSE(s1 != s2);
	}

}

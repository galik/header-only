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

#include <string>
#include <cassert>
#include <random>
#include <iostream>
#include <algorithm>

#include <hol/bug.h>
#include <hol/random_utils.h>
#include <hol/string_span_utils.h>

using namespace hol::random_utils;

std::ostream& operator<<(std::ostream& os, gsl::string_span<> s)
{
	return os << std::string(s.data(), s.size());
}

bool eqtest(gsl::string_span<> ss, std::string const& t, hol::offset_type o, hol::offset_type o1, std::size_t o2)
{
	bool ret = (o1 == ss.size() && o2 == std::string::npos) || std::size_t(o1) == o2;

	if(!ret)
	{
		std::cout << "ERROR     : " << ss << " " << t << '\n';
		std::cout << "span pos  : " << o1 << '\n';
		std::cout << "string pos: " << o2 << '\n';
		std::cout << "s.size()  : " << ss.size() << '\n';
		std::cout << "t.size()  : " << t.size() << '\n';
		std::cout << "offset    : " << o << '\n';
	}

	return ret;
}

std::string random_text(std::size_t min, std::size_t max)
{
	char const letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	auto size = rnd::random_number(min, max);

	std::string s;
	s.reserve(size);

	while(size--)
		s += letters[rnd::random_number(sizeof(letters) - 2)];

	return s;
}

TEST_CASE("string_span string function tests", "string_span_utils")
{
	SECTION("find functions with default offset")
	{
		for(auto times = 100000; times--;)
		{
			std::string s = random_text(0, 16);
			std::string t = random_text(0, 4);
			std::string c = random_text(1, 1);

			gsl::string_span<> ss{s};
			gsl::string_span<> ts{t};

			REQUIRE(eqtest(ss, t, 0, hol::find(ss, c[0]), s.find(c[0])));
			REQUIRE(eqtest(ss, t, 0, hol::find_first_of(ss, ts), s.find_first_of(t)));
			REQUIRE(eqtest(ss, t, 0, hol::find_first_not_of(ss, ts), s.find_first_not_of(t)));
			REQUIRE(eqtest(ss, t, 0, hol::find_last_of(ss, ts), s.find_last_of(t)));
			REQUIRE(eqtest(ss, t, 0, hol::find_last_not_of(ss, ts), s.find_last_not_of(t)));
		}
	}

	SECTION("find functions with offset")
	{
		for(auto times = 100000; times--;)
		{
			std::string s = random_text(0, 16);
			std::string t = random_text(0, 4);
			std::string c = random_text(1, 1);

			auto o = s.empty() ? 0 : rnd::random_number(s.size() - 1);

			gsl::string_span<> ss{s};
			gsl::string_span<> ts{t};

			REQUIRE(eqtest(ss, t, o, hol::find(ss, c[0], o), s.find(c[0], o)));
			REQUIRE(eqtest(ss, t, o, hol::find_first_of(ss, ts, o), s.find_first_of(t, o)));
			REQUIRE(eqtest(ss, t, o, hol::find_first_not_of(ss, ts, o), s.find_first_not_of(t, o)));
			REQUIRE(eqtest(ss, t, o, hol::find_last_of(ss, ts, o), s.find_last_of(t, o)));
			REQUIRE(eqtest(ss, t, o, hol::find_last_not_of(ss, ts, o), s.find_last_not_of(t, o)));
		}
	}
}


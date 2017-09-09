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

//#include "test.h"
#include "hol/bug.h"
#include "hol/random_numbers.h"
#include "hol/range_utils.h"
#include "hol/srange_utils.h"
//#include "hol/unicode_utils.h"

namespace hol{
	using namespace header_only_library::random_numbers;
	using namespace header_only_library::range;
//	using namespace header_only_library::unicode_utils;
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

TEST_CASE("String operations", "[]")
{
	SECTION("r1.size() == r2.size()")
	{
		std::string s1 = "abcdef";
		std::string s2 = "ABCDEF";

		auto r1 = hol::make_range(s1);
		auto r2 = hol::make_range(s2);

		hol::copy(r1, r2);

		REQUIRE(s2 == "abcdef");
	}

	SECTION("r1.size() < r2.size()")
	{
		std::string s1 = "abc";
		std::string s2 = "ABCDEF";

		auto r1 = hol::make_range(s1);
		auto r2 = hol::make_range(s2);

		hol::copy(r1, r2);

		REQUIRE(s2 == "abcDEF");
	}

	SECTION("r1.size() > r2.size()")
	{
		std::string s1 = "abcdef";
		std::string s2 = "ABC";

		auto r1 = hol::make_range(s1);
		auto r2 = hol::make_range(s2);

		hol::copy(r1, r2);

		REQUIRE(s2 == "abc");
	}
}

std::ostream& operator<<(std::ostream& os, hol::srange sr)
{
	return os << sr.string();
}

TEST_CASE("Stream operations", "[]")
{
	SECTION("getline")
	{
		char s[] = "\none\n\ntwo\n\n\nthree\n\n";
		{
			hol::srange r = hol::make_srange(s);
			hol::range_istream ris(r);

			std::vector<hol::srange> v;
			for(hol::srange r; hol::getline(ris, r);)
				v.push_back(r);

			bug_cnt(v);
		}
		{
			std::istringstream ris(s);

			std::vector<std::string> v;
			for(std::string r; std::getline(ris, r);)
				v.push_back(r);

			bug_cnt(v);
		}
	}
}

TEST_CASE("Regex operations", "[]")
{
	SECTION("match")
	{
		std::string s = "[some stuff]";
		std::regex  e{R"~(\[(\w+)\s+(\w+)\])~"};

		std::vector<std::string> answers =
		{
			"[some stuff]",
			"some",
			"stuff",
		};

		auto matches = hol::regex_match(hol::make_range(s), e);

		REQUIRE(matches.size() == answers.size());

		for(auto i = 0U; i < matches.size(); ++i)
			REQUIRE(std::equal(std::begin(matches[i]), std::end(matches[i]),
				std::begin(answers[i]), std::end(answers[i])));
	}

	SECTION("search")
	{
		std::string s = " [some1 stuff1] [some2 stuff2] [some3 stuff3] ";
		std::regex  e{R"~(\[(\w+)\s+(\w+)\])~"};

		std::vector<std::string> answers =
		{
			"[some1 stuff1]",
			"some1",
			"stuff1",
		};

		auto matches = hol::regex_search(hol::make_range(s), e);

		REQUIRE(matches.size() == answers.size());

		for(auto i = 0U; i < matches.size(); ++i)
			REQUIRE(std::equal(std::begin(matches[i]), std::end(matches[i]),
				std::begin(answers[i]), std::end(answers[i])));
	}

	SECTION("search_all")
	{
		std::string s = " [some1 stuff1] [some2 stuff2] [some3 stuff3] ";
		std::regex  e{R"~(\[(\w+)\s+(\w+)\])~"};

		std::vector<std::vector<std::string>> answers =
		{
			{
				"[some1 stuff1]",
				"some1",
				"stuff1",
			},
			{
				"[some2 stuff2]",
				"some2",
				"stuff2",
			},
			{
				"[some3 stuff3]",
				"some3",
				"stuff3",
			},
		};

		auto matches = hol::regex_search_all(hol::make_range(s), e);

		REQUIRE(matches.size() == answers.size());

		for(auto i = 0U; i < matches.size(); ++i)
		{
			REQUIRE(matches[i].size() == answers[i].size());

			for(auto j = 0U; j < matches[i].size(); ++j)
			{
				REQUIRE(std::equal(std::begin(matches[i][j]), std::end(matches[i][j]),
					std::begin(answers[i][j]), std::end(answers[i][j])));
			}
		}
	}
}

TEST_CASE("Algorithms", "[]")
{
	SECTION("all_of")
	{
		std::string s1 = "aaaaaaa";
		std::string s2 = "aabaaaa";

		auto r1 = hol::make_range(s1);
		auto r2 = hol::make_range(s2);

		REQUIRE(hol::all_of(r1, [](auto c){ return c == 'a'; }));
		REQUIRE_FALSE(hol::all_of(r2, [](auto c){ return c == 'a'; }));
	}

	SECTION("any_of")
	{
		std::string s = "aaaaaab";

		auto r = hol::make_range(s);

		REQUIRE(hol::any_of(r, [](auto c){ return c == 'a'; }));
		REQUIRE(hol::any_of(r, [](auto c){ return c == 'b'; }));
		REQUIRE_FALSE(hol::any_of(r, [](auto c){ return c == 'c'; }));
	}

	SECTION("none_of")
	{
		std::string s = "ababab";

		auto r = hol::make_range(s);

		REQUIRE_FALSE(hol::none_of(r, [](auto c){ return c == 'a'; }));
		REQUIRE_FALSE(hol::none_of(r, [](auto c){ return c == 'b'; }));
		REQUIRE(hol::none_of(r, [](auto c){ return c == 'c'; }));
	}

	SECTION("for_each")
	{
		std::string s = u8"abcde";

		auto r = hol::make_range(s);

		hol::for_each(r, [](char& c){ c += 1; });

		REQUIRE(s == u8"bcdef");
	}

	SECTION("count")
	{
		std::vector<int> v;
		std::generate_n(std::back_inserter(v), 10,
			[]{ return hol::random_number(100); });

		auto r = hol::make_range(v);

		REQUIRE(hol::count(r, v[0]) == std::count(std::begin(v), std::end(v), v[0]));
	}

	SECTION("count_if")
	{
		std::vector<int> v;
		std::generate_n(std::back_inserter(v), 10,
			[]{ return hol::random_number(100); });

		auto r = hol::make_range(v);
		auto p = [](auto i){ return i > 50; };

		REQUIRE(hol::count_if(r, p) == std::count_if(std::begin(v), std::end(v), p));
	}

	SECTION("mismatch")
	{
		std::vector<int> v1 = {0, 1, 2, 3};
		std::vector<int> v2 = {0, 1, 9};

		auto r1 = hol::make_range(v1);
		auto r2 = hol::make_range(v2);

		auto p = hol::mismatch(r1, std::begin(r2));

		REQUIRE(*p.first == 2);
		REQUIRE(*p.second == 9);

		p = hol::mismatch(r1, std::begin(r2), [](auto& e1,auto& e2){ return e1 == e2; });

		REQUIRE(*p.first == 2);
		REQUIRE(*p.second == 9);

		p = hol::mismatch(r1, r2);

		REQUIRE(*p.first == 2);
		REQUIRE(*p.second == 9);

		p = hol::mismatch(r1, r2, [](auto& e1,auto& e2){ return e1 == e2; });

		REQUIRE(*p.first == 2);
		REQUIRE(*p.second == 9);
	}
}

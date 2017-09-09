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
#include <string>
#include <vector>

#include "test.h"
#include "hol/bug.h"
#include "hol/random_numbers.h"
#include "hol/string_utils.h"
#include "hol/unicode_utils.h"
#include "hol/zstring_utils.h"

namespace hol {
	using namespace header_only_library::random_numbers;
	using namespace header_only_library::unicode_utils;
	using namespace header_only_library::zstring_utils;
}

namespace gen {
	using namespace header_only_library::zstring_utils::generic;
} // namespace gen

auto const N = 10000ULL;

TEST_CASE("ZString Utils", "[comparing]")
{
	SECTION("zstrings_are_equal")
	{
		REQUIRE(hol::equal_to("", ""));
		REQUIRE(hol::equal_to("a", "a"));
		REQUIRE(hol::equal_to("ab", "ab"));
		REQUIRE_FALSE(hol::equal_to("a", ""));
		REQUIRE_FALSE(hol::equal_to("", "a"));
		REQUIRE_FALSE(hol::equal_to("a", "b"));
		REQUIRE_FALSE(hol::equal_to("ab", "ac"));
	}
}

TEST_CASE("Generic Utils", "")
{
	SECTION("strcpy")
	{
		std::array<char, 10> a1 = {};
		std::array<char, 10> a2 = {};
		std::array<char, 10> a3 = {};

		for(auto i = 0ULL; i < N; ++i)
		{
			std::generate_n(std::begin(a1), a1.size(), []{ return hol::random_number(int('A'), int('Z')); });
			a1.back() = '\0';
			std::strcpy(a2.data(), a1.data());
			gen::strcpy(a3.data(), a1.data());

			REQUIRE(a2 == a3);
		}
	}

	SECTION("strncpy")
	{
		std::array<char, 10> a1 = {};
		std::array<char, 10> a2 = {};
		std::array<char, 10> a3 = {};

		for(auto i = 0ULL; i < N; ++i)
		{
			auto n = hol::random_number<std::size_t>(0, 9);
			std::generate_n(std::begin(a1), a1.size(), []{ return hol::random_number(int('A'), int('Z')); });
			a1.back() = '\0';

			std::strncpy(a2.data(), a1.data(), n);
			gen::strncpy(a3.data(), a1.data(), n);

			REQUIRE(a2 == a3);
		}
	}

	SECTION("strcat")
	{
		char const* ss[] = {"", "a", "bc"};

		char b1[100] = {};
		char b2[100] = {};

		char const* s = "";

		for(auto i = 0ULL; i < N; ++i)
		{
			b1[0] = {};
			b2[0] = {};

			while(std::strlen(b1) + std::strlen(s) < sizeof(b1)
			&&    std::strlen(b2) + std::strlen(s) < sizeof(b2))
			{
				s = hol::random_element(ss);

				std::strcat(b1, s);
				gen::strcat(b2, s);
			}

			REQUIRE(std::string(b1) == std::string(b2));
		}
	}

	SECTION("strncat")
	{
		char const* ss[] = {"", "a", "bc" "defghi"};

		char b1[100] = {};
		char b2[100] = {};

		char const* s = "";

		for(auto i = 0ULL; i < N; ++i)
		{
			b1[0] = {};
			b2[0] = {};

			while(std::strlen(b1) + std::strlen(s) < sizeof(b1)
			&&    std::strlen(b2) + std::strlen(s) < sizeof(b2))
			{

				s = hol::random_element(ss);

				auto n = hol::random_number(std::strlen(s));

				std::strncat(b1, s, n);
				gen::strncat(b2, s, n);
			}

			REQUIRE(std::string(b1) == std::string(b2));
		}
	}

	SECTION("strcmp")
	{
		std::array<char, 10> a1 = {};
		std::array<char, 10> a2 = {};

		for(auto i = 0ULL; i < N; ++i)
		{
			std::generate_n(std::begin(a1), a1.size(), []{ return hol::random_number(int('A'), int('Z')); });
			hol::random_element(a1) = '\0';

			std::generate_n(std::begin(a2), a2.size(), []{ return hol::random_number(int('A'), int('Z')); });
			hol::random_element(a2) = '\0';

			int i1 = std::strcmp(a1.data(), a2.data());
			int i2 = gen::strcmp(a1.data(), a2.data());

			int b1 = i1 > 0 ? 1 : (i1 < 0 ? -1 : 0);
			int b2 = i2 > 0 ? 1 : (i2 < 0 ? -1 : 0);

			REQUIRE(b1 == b2);
		}
	}

	SECTION("strncmp")
	{
		std::array<char, 10> a1 = {};
		std::array<char, 10> a2 = {};

		for(auto i = 0ULL; i < N; ++i)
		{
			std::generate_n(std::begin(a1), a1.size(), []{ return hol::random_number(int('A'), int('Z')); });
			hol::random_element(a1) = '\0';

			std::generate_n(std::begin(a2), a2.size(), []{ return hol::random_number(int('A'), int('Z')); });
			hol::random_element(a2) = '\0';

			auto n = hol::random_number((std::strlen(a1.data()) + std::strlen(a2.data())) / 2U);

			int i1 = std::strncmp(a1.data(), a2.data(), n);
			int i2 = gen::strncmp(a1.data(), a2.data(), n);

			int b1 = i1 > 0 ? 1 : (i1 < 0 ? -1 : 0);
			int b2 = i2 > 0 ? 1 : (i2 < 0 ? -1 : 0);

			REQUIRE(b1 == b2);
		}
	}

	SECTION("strchr")
	{
		char const* s = "abc";
		char c = 'b';

		auto p1 = std::strchr(s, c);
		auto p2 = gen::strchr(s, c);

		REQUIRE(p1 == p2);

		c = '\0';

		p1 = std::strchr(s, c);
		p2 = gen::strchr(s, c);

		REQUIRE(p1 == p2);

		char b[] = "abc";
		c = 'a';

		p1 = std::strchr(b, c);
		p2 = gen::strchr(b, c);

		REQUIRE(p1 == p2);
	}

	SECTION("strrchr")
	{
		char const* s = "abc";
		char c = 'b';

		auto p1 = std::strrchr(s, c);
		auto p2 = gen::strrchr(s, c);

		REQUIRE(p1 == p2);

		c = '\0';

		p1 = std::strrchr(s, c);
		p2 = gen::strrchr(s, c);

		REQUIRE(p1 == p2);

		char b[] = "abc";
		c = 'a';

		p1 = std::strrchr(b, c);
		p2 = gen::strrchr(b, c);

		REQUIRE(p1 == p2);
	}

	SECTION("strspn")
	{
		auto s1 = "abcdef";
		auto s2 = "bac";

		auto p1 = std::strspn(s1, s2);
		auto p2 = gen::strspn(s1, s2);

		REQUIRE(p1 == p2);
	}

	SECTION("strcspn")
	{
		auto s1 = "abcdef";
		auto s2 = "fed";

		auto p1 = std::strcspn(s1, s2);
		auto p2 = gen::strcspn(s1, s2);

		REQUIRE(p1 == p2);
	}

	SECTION("strpbrk")
	{
		char const* s = "abcdef";
		char const* brk = "cde";

		auto p1 = std::strpbrk(s, brk);
		auto p2 = gen::strpbrk(s, brk);

		REQUIRE(p1 == p2);

		brk = "\0";

		p1 = std::strpbrk(s, brk);
		p2 = gen::strpbrk(s, brk);

		REQUIRE(p1 == p2);

		char b[] = "abcdef";
		brk = "a";

		p1 = std::strpbrk(b, brk);
		p2 = gen::strpbrk(b, brk);

		REQUIRE(p1 == p2);
	}

	SECTION("strstr")
	{
		std::string const chars = "abcdefghijklmnopqrstuvwxyz";

		for(auto i = 0ULL; i < N; ++i)
		{
			auto size = hol::random_number(1, 20);

			std::string s1;
			s1.reserve(size);

			while(size--)
				s1 += hol::random_element(chars);

			size = hol::random_number(1, 3);

			std::string s2;
			s2.reserve(size);

			while(size--)
				s2 += hol::random_element(chars);

			auto p1 = std::strstr(s1.data(), s2.data());
			auto p2 = gen::strstr(s1.data(), s2.data());

			REQUIRE(p1 == p2);
		}
	}

	SECTION("strtok")
	{
		bug_scope("strtok");

		std::string const text = "abcXdefXYghiZjkl";
		std::string const delims = "XYZ";

		std::string s;
		std::vector<std::string> v1;

		s = text;
		if(auto p = std::strtok(&s[0], delims.c_str()))
		{
			v1.emplace_back(p);
			while(p = std::strtok(nullptr, delims.c_str()))
				v1.emplace_back(p);
		}

		bug_cnt(v1);

		std::vector<std::string> v2;

		s = text;
		if(auto p = gen::strtok(&s[0], delims.c_str()))
		{
			v2.emplace_back(p);
			while(p = gen::strtok((char*)nullptr, delims.c_str()))
				v2.emplace_back(p);
		}

		bug_cnt(v2);

		REQUIRE(v1 == v2);
	}
}




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

#include <algorithm>
#include <string>
#include <vector>

#include "test.h"
#include "hol/bug.h"
#include "hol/random_numbers.h"
#include "hol/string_utils.h"
#include "hol/unicode_utils.h"

namespace hol{
	using namespace header_only_library::random_numbers;
	using namespace header_only_library::string_utils;
	using namespace header_only_library::unicode_utils;
}

using namespace std::literals;

void unicode_convert(std::string const& s, std::string& u) { u = s; }
void unicode_convert(std::string const& s, std::wstring& u) { u = hol::utf8_to_ws(s); }
void unicode_convert(std::string const& s, std::u16string& u) { u = hol::utf8_to_utf16(s); }
void unicode_convert(std::string const& s, std::u32string& u) { u = hol::utf8_to_utf32(s); }

template<typename String>
String random_unique_string(std::size_t n)
{
	std::string s;
	while(s.size() < n)
	{
		s += char(hol::random_number(int('A'), int('Z')));
		std::sort(std::begin(s), std::end(s));
		s.erase(std::unique(std::begin(s), std::end(s)), std::end(s));
	}

	std::shuffle(std::begin(s), std::end(s), hol::random_generator());

	String u;
	unicode_convert(s, u);

	return u;
}

TEST_CASE("Trimming Utils", "[trimming]")
{
	SECTION("trim_left_mute")
	{
		{
			auto const t = ""s;
			auto s = ""s;
			auto const x = ""s;
			auto const o = s;
			auto const& r = hol::trim_left_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s == o);
			REQUIRE(&s == &r);
		}

		{
			auto const t = ""s;
			auto s = "{}"s;
			auto const x = "{}"s;
			auto const o = s;
			auto const& r = hol::trim_left_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s == o);
			REQUIRE(&s == &r);
		}

		{
			auto const t = "{}"s;
			auto s = ""s;
			auto const x = ""s;
			auto const o = s;
			auto const& r = hol::trim_left_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s == o);
			REQUIRE(&s == &r);
		}

		{
			auto const t = "{"s;
			auto s = "{{{}}}"s;
			auto const x = "}}}"s;
			auto const o = s;
			auto const& r = hol::trim_left_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s != o);
			REQUIRE(&s == &r);
		}

		{
			auto const t = "{"s;
			auto s = "{ {} }"s;
			auto const x = " {} }"s;
			auto const o = s;
			auto const& r = hol::trim_left_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s != o);
			REQUIRE(&s == &r);
		}

		{
			auto const t = "{"s;
			auto s = " {{}} "s;
			auto const x = " {{}} "s;
			auto const o = s;
			auto const& r = hol::trim_left_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s == o);
			REQUIRE(&s == &r);
		}
	}

	SECTION("trim_right_mute")
	{
		{
			auto const t = ""s;
			auto s = ""s;
			auto const x = ""s;
			auto const o = s;
			auto const& r = hol::trim_right_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s == o);
			REQUIRE(&s == &r);
		}

		{
			auto const t = ""s;
			auto s = "{}"s;
			auto const x = "{}"s;
			auto const o = s;
			auto const& r = hol::trim_right_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s == o);
			REQUIRE(&s == &r);
		}

		{
			auto const t = "{}"s;
			auto s = ""s;
			auto const x = ""s;
			auto const o = s;
			auto const& r = hol::trim_right_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s == o);
			REQUIRE(&s == &r);
		}

		{
			auto const t = "}"s;
			auto s = "{{{}}}"s;
			auto const x = "{{{"s;
			auto const o = s;
			auto const& r = hol::trim_right_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s != o);
			REQUIRE(&s == &r);
		}

		{
			auto const t = "}"s;
			auto s = "{ {} }"s;
			auto const x = "{ {} "s;
			auto const o = s;
			auto const& r = hol::trim_right_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s != o);
			REQUIRE(&s == &r);
		}

		{
			auto const t = "}"s;
			auto s = " {{}} "s;
			auto const x = " {{}} "s;
			auto const o = s;
			auto const& r = hol::trim_right_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s == o);
			REQUIRE(&s == &r);
		}
	}

	SECTION("trim_mute")
	{
		{
			auto const t = ""s;
			auto s = ""s;
			auto const x = ""s;
			auto const o = s;
			auto const& r = hol::trim_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s == o);
			REQUIRE(&s == &r);
		}

		{
			auto const t = ""s;
			auto s = "{}"s;
			auto const x = "{}"s;
			auto const o = s;
			auto const& r = hol::trim_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s == o);
			REQUIRE(&s == &r);
		}

		{
			auto const t = "{}"s;
			auto s = ""s;
			auto const x = ""s;
			auto const o = s;
			auto const& r = hol::trim_mute(s, t);
			REQUIRE(r == x);
			REQUIRE(s == o);
			REQUIRE(&s == &r);
		}
	}
//		T "{}"
//		B "{{{}}}" ""
//		B "{ {} }" " {} "
//		B " {{}} " " {{}} "

}

TEST_CASE("Output Utils", "[output_separator]")
{
	std::ostringstream oss;

	SECTION("default constructed")
	{
		hol::output_separator sep;
		oss.str("");

		for(int i = 0; i < 3; ++i)
			oss << sep << i;

		REQUIRE(oss.str() == "0 1 2");
	}

	SECTION("constructor single param")
	{
		hol::output_separator sep{", "};
		oss.str("");

		for(int i = 0; i < 3; ++i)
			oss << sep << i;

		REQUIRE(oss.str() == "0, 1, 2");
	}

//	SECTION("constructor two param")
//	{
//		hol::output_separator sep{"[", ", "};
//		oss.str("");
//
//		for(int i = 0; i < 3; ++i)
//			oss << sep << i;
//
//		sep.reset("] [", ", ");
//
//		for(int i = 0; i < 3; ++i)
//			oss << sep << i;
//
//		bug_var(oss.str());
//
//		REQUIRE(oss.str() == "[0, 1, 2");
//	}
}


TEST_CASE("Splitting Utils", "[split ref types]")
{
	SECTION("split_refs")
	{
		{
			std::string s("");
			std::string t("||");
			std::vector<std::string> const x = {};

			auto const r = hol::split_refs(s, t);

			REQUIRE(r == x);
		}

		{
			std::string s("||");
			std::string t("||");
			std::vector<std::string> const x = {"", ""};

			auto const r = hol::split_refs(s, t);

			REQUIRE(r == x);
		}

		{
			std::string s("");
			std::string t("||");
			std::vector<std::string> const x = {};

			auto const r = hol::split_refs(s, t);

			REQUIRE(r == x);
		}

		{
			std::string s("some||text||to||split");
			std::string t("||");
			std::vector<std::string> const x = {"some", "text", "to", "split"};

			auto const r = hol::split_refs(s, t);

			REQUIRE(r == x);
		}

		{
			std::string s("||text");
			std::string t("||");
			std::vector<std::string> const x = {"", "text"};

			auto const r = hol::split_refs(s, t);

			REQUIRE(r == x);
		}

		{
			std::string s("text||");
			std::string t("||");
			std::vector<std::string> const x = {"text", ""};

			auto const r = hol::split_refs(s, t);

			REQUIRE(r == x);
		}

		{
			std::string s("||||");
			std::string t("||");
			std::vector<std::string> const x = {"", "", ""};

			auto const r = hol::split_refs(s, t);

			REQUIRE(r == x);
		}
	}
}




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

#include <iomanip>
#include <iostream>

#include "test.h"
#include "hol/string_view_stream.h"

using namespace hol;
using namespace std::literals::string_literals;

const std::string json_data =
R"~(
{
    "glossary": {
        "title": "example glossary",
		"GlossDiv": {
            "title": "S",
			"GlossList": {
                "GlossEntry": {
                    "ID": "SGML",
					"SortAs": "SGML",
					"GlossTerm": "Standard Generalized Markup Language",
					"Acronym": "SGML",
					"Abbrev": "ISO 8879:1986",
					"GlossDef": {
                        "para": "A meta-markup language, used to create markup languages such as DocBook.",
						"GlossSeeAlso": ["GML", "XML"]
                    },
					"GlossSee": "markup"
                }
            }
        }
    }
}
)~";

string_view_stream& read_to(string_view_stream& svs, ex::string_view& sv, char delim = '\n')
{
	sv = svs.get_string_view();
	auto size = sv.size();
	sv.remove_prefix(svs.tellg());
	svs.ignore(std::numeric_limits<std::streamsize>::max(), delim);
	sv.remove_suffix(size - svs.tellg());
	return svs;
}

string_view_stream& read_to_one_of(string_view_stream& svs, ex::string_view& sv, ex::string_view tokens)
{
//	sv = svs.get_string_view();
//
//	auto size = sv.size();
//	sv.remove_prefix(svs.tellg());
//
//	for(auto i = sv.begin(); i != sv.end() && tokens.find(*i) = ex::string_view::npos; ++i) {}
//
//	sv.remove_suffix(size - svs.tellg());

	return svs;
}

string_view_stream& read_enclosed(string_view_stream& svs
	, ex::string_view& sv
		, char open = '{', char close = '}')
{
	if(!read_to(svs, sv, open))
		return svs;

	auto depth = 0U;

	if(!read_to(svs, sv, close))
	{
		svs.setstate(std::ios::failbit);
		return svs;
	}

	return svs;
}

int main()
{
	string_view_stream svs(json_data);

//	svs.seekg(10);

//	std::string s;
//	while(svs >> s)
//		OUT(s);

//	bug_var(svs.tellg());

	ex::string_view sv;
	while(read_to(svs, sv))
		OUT(sv);

}


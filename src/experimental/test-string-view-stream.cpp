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
#include <algorithm>

#include "test.h"
#include "hol/experimental/string_view_stream.h"

using namespace hol;
using namespace std::literals::string_literals;

string_view_stream& read_to(string_view_stream& svs, ex::string_view& sv, char delim = '\n')
{
	sv = svs.string_view();
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

namespace hol {
//string_view_stream& getline(string_view_stream& svs, ex::string_view& sv, char delim = '\n')
//{
//	sv = svs.string_view();
//	auto size = sv.size();
//	sv.remove_prefix(svs.tellg());
//	svs.ignore(std::numeric_limits<std::streamsize>::max(), delim);
//	sv.remove_suffix(size - svs.tellg());
//	return svs;
//}
} // hol

const std::string test_data =
R"~(
the quick brown fox
1, 3, 5, 7, 9
2.5 9.7 -100.9
ahdgrus
jumped over the lazy dog
)~";

//bool getline(hol::string_view& svs, hol::string_view& sv, char delim = '\n')
//{
//	if(svs.empty())
//		return false;
//
//	size_t pos = 0;
//	for(; pos < svs.size() && svs[pos] != delim; ++pos) {}
//
//	sv = svs.substr(0, pos);
//	svs = svs.substr(std::min(pos + 1, svs.size()));
//
//	return true;
//}

//bool operator>>(hol::string_view& svs, hol::string_view& sv)
//{
//	return getline(svs, sv, ' ');
//}


hol::string_view& getline(hol::string_view& svs, hol::string_view& sv, char delim = '\n')
{
	if(!svs.data() || svs.empty())
		return (svs = hol::string_view(nullptr, 0));

	size_t pos = 0;
	for(; pos < svs.size() && svs[pos] != delim; ++pos) {}

	sv = svs.substr(0, pos);
	svs = svs.substr(std::min(pos + 1, svs.size()));

	return svs;
}

hol::string_view& operator>>(hol::string_view& svs, hol::string_view& sv)
{
	return getline(svs, sv, ' ');
}

//hol::string_view& operator>>(hol::string_view& svs, int& sv)
//{
//
//	return getline(svs, sv, ' ');
//}

//bool operator>>(hol::string_view& svs, decltype(std::ws()))
//{
//	return getline(svs, sv, ' ');
//}

int main()
{
	try
	{

//		string_view sv = string_view(nullptr, 0);
//
//		bug_var((void*)sv.data());
//		bug_var(sv);
//		bug_var((bool)sv.data());
//		bug_var("yay");
//
//
//		return 0;

		hol::string_view svs = test_data;

		int i;
		double d;
		std::string s;
		hol::string_view sv;

		if(!getline(svs, sv).data())
			hol_throw_runtime_error("fail readline");

		if(!getline(svs, sv).data())
			hol_throw_runtime_error("fail readline");

		OUT("sv: " << sv);

//		string_view_stream svs2(sv);
//		string_view sv2;
//		while(svs2 >> sv2)
//			OUT("sv2: " << sv2);

		string_view sv2;
		while((sv >> sv2).data())
			OUT("sv2: " << sv2);

	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	catch(...)
	{
		std::cerr << "unknown exception" << '\n';
	}
}


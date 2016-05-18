#ifndef HOL_ITERATORS_H
#define HOL_ITERATORS_H
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

#include <regex>

namespace hol {

struct sregex_matcher
{
	const std::regex e;
	std::sregex_iterator i;

public:
	sregex_matcher(const std::string& s, const std::regex& e)
	: e(e), i(s.begin(), s.end(), e) {}
	sregex_matcher(const std::string& s, const std::string& exp)
	: e(exp, std::regex_constants::optimize), i(s.begin(), s.end(), e) {}

	sregex_matcher(
		std::string::const_iterator begin
		, std::string::const_iterator end
		, const std::regex& e)
	: e(e), i(begin, end, e) {}
	sregex_matcher(
		std::string::const_iterator begin
		, std::string::const_iterator end
		, const std::string& exp)
	: e(exp, std::regex_constants::optimize), i(begin, end, e) {}

	auto begin() const { return i; }
	auto end() const { return std::sregex_iterator(); }
};

struct cregex_matcher
{
	const std::regex e;
	std::cregex_iterator i;

public:
	cregex_matcher(const std::string& s, const std::regex& e)
	: e(e), i(s.data(), s.data() + s.size(), e) {}
	cregex_matcher(const std::string& s, const char* exp)
	: e(exp, std::regex_constants::optimize), i(s.data(), s.data() + s.size(), e) {}

	cregex_matcher(const char* begin, const char* end, const std::regex& e)
	: e(e), i(begin, end, e) {}
	cregex_matcher(const char* begin, const char* end, const char* exp)
	: e(exp, std::regex_constants::optimize), i(begin, end, e) {}


	auto begin() const { return i; }
	auto end() const { return std::cregex_iterator(); }
};

} // hol

#endif // HOL_ITERATORS_H

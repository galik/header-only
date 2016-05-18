#ifndef HOL_BUG_H
#define HOL_BUG_H
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

/**
 * USAGE:
 *
 * Compiling with NDEBUG these macros produce no output.
 *
 * Otherwise:
 *
 * bug("send text and variables to std::cout" << var1);
 *
 * int i = 0;
 * bug_var(i); // output value of i
 *
 * std::vector<int> v {1, 2, 3};
 *
 * bug_cnt(v); // output values in container v using bug()
 *
 * void func()
 * {
 *     bug_fun(); // print out the name of the function and exit point
 *
 *     // ...
 * }
 *
 */

#include <cerrno>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <functional>

#include "stl.h"

namespace hol {

inline
auto& get_edit_bug_fun()
{
	static std::function<std::string(std::string)> edit_bug_fun = [](std::string s){return s;};
	return edit_bug_fun;
}

#ifdef NDEBUG
#define bug(m) do{}while(0)
#define bug_var(v) do{}while(0)
#define bug_cnt(c) do{}while(0)
#define bug_itr(c) do{}while(0)

#define hol_throw_exception(exc, message) \
	throw exc(message)

#define hol_throw_runtime_error(msg) do{ \
	std::ostringstream oss; \
	oss << msg; \
	throw std::runtime_error(oss.str());}while(0)

#define hol_throw_errno() hol_throw_runtime_error(std::strerror(errno));

struct scope_bomb{};

#define bug_fun() hol::scope_bomb scope_bomb_inst

#else

#define bug(m) do{std::cout << m << std::endl;}while(0)
#define bug_var(v) do{std::cout << #v ": " << std::boolalpha << v << std::endl;}while(0)

#define bug_cnt(c) \
	do{ \
		std::cout << #c ": " << std::size(c) << std::endl; \
		int i=0; \
		for(auto&& v_c:c) \
			{bug((i<100?" ":"") << (i<10?" ":"") << i << ": " << v_c);++i;} \
	}while(0)

#define bug_itr(c, b, e) \
	do{ \
		bug(#c ": " << c.size()); \
		int n=0; \
		for(auto i = b; i != e; ++i) \
			{bug((*i<100?" ":"") << (*i<10?" ":"") << n << ": " << *i);++n;} \
	}while(0)

#define hol_throw_exception(exc, msg) do{ \
std::ostringstream oss; \
oss << __FILE__ << ":" << __LINE__ << ":1 error: " << msg; \
throw exc(oss.str());}while(0)

#define hol_throw_runtime_error(msg) do{ \
	std::ostringstream oss; \
	oss << __FILE__ << ":" << __LINE__ << ":1 error: " << msg; \
	throw std::runtime_error(oss.str());}while(0)

#define hol_throw_errno() hol_throw_runtime_error(std::strerror(errno));

struct scope_bomb
{
	std::string m;
	scope_bomb(const std::string& m)
	       : m(m) { bug("--> " << m); }
	~scope_bomb() { bug("<-- " << m); }
};
#define bug_fun() hol::scope_bomb scope_bomb_inst(hol::get_edit_bug_fun()(__PRETTY_FUNCTION__))

#endif

class errno_error
: public std::runtime_error
{
public:
	errno_error(): std::runtime_error(std::strerror(errno)) {}
};

} // hol

#endif // HOL_BUG_H

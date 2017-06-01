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

#ifdef __GNUC__
#define FUNC_MACRO __PRETTY_FUNCTION__
#else
#define FUNC_MACRO __func__
#endif

namespace header_only_library {

inline
std::function<std::string(std::string)>& get_edit_bug_fun()
{
	static std::function<std::string(std::string)> edit_bug_fun = [](std::string s){return s;};
	return edit_bug_fun;
}

#ifdef NDEBUG
#define bug(m) do{}while(0)
#define bug_var(v) do{}while(0)
#define bug_cnt(c) do{}while(0)
#define bug_itr(c, b, e) do{}while(0)

struct scope_bomb{};

#define bug_fun()
#define bug_scope(m)

#else

namespace private_details {

template<typename C>
constexpr std::size_t size(C&& c)
{
    return std::forward<C>(c).size();
}

template<typename T, std::size_t N>
constexpr std::size_t size(const T(&)[N]) noexcept
{
    return N;
}

} // namespace private_details

#define bug(m) do{std::ostringstream __o;__o<<m<<'\n';std::cout<<__o.str()<<std::flush;}while(0)
#define bug_var(v) bug(#v ": " << std::boolalpha << v)

#define bug_cnt(c) \
	do{ \
		std::ostringstream __o; \
		__o << #c ": " << header_only_library::private_details::size(c) << '\n'; \
		int __i=0; \
		for(auto&& v_c: c) \
			{__o << (__i<100?" ":"") << (__i<10?" ":"") << __i << ": " << v_c << '\n';++__i;} \
		std::cout << __o.str() << std::flush; \
	}while(0)

#define bug_itr(c, b, e) \
	do{ \
		std::ostringstream o; \
		o << #c ": " << std::distance(b, e) << '\n'; \
		int n=0; \
		for(auto i = (b); i != (e); ++i) \
			{o << " " << (n<100?" ":"") << (n<10?" ":"") << n << ": " << *i << '\n';++n;} \
		std::cout << o.str() << std::flush; \
	}while(0)

struct scope_bomb
{
	std::string m;
	scope_bomb(const std::string& m)
	       : m(m) { bug("--> " << m); }
	~scope_bomb() { bug("<-- " << m); }
};

#define bug_fun() header_only_library::scope_bomb scope_bomb_inst(header_only_library::get_edit_bug_fun()(FUNC_MACRO))
#define bug_scope(m) header_only_library::scope_bomb scope_bomb_inst(m)
#endif

} // header_only_library

#endif // HOL_BUG_H

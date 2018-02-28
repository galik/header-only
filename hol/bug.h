#ifndef HEADER_ONLY_LIBRARY_BUG_H
#define HEADER_ONLY_LIBRARY_BUG_H
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

// BACKTRACE
#ifdef __unix__
#include <cxxabi.h>
#include <csignal>
#include <ucontext.h>
#include <execinfo.h>
#endif // __unix__

#ifdef HOL_FUNC_MACRO_PRETTY
#    ifdef __GNUC__
#        define HOL_FUNC_MACRO __PRETTY_FUNCTION__
#    else
#        define HOL_FUNC_MACRO __func__
#    endif
#else
#    define HOL_FUNC_MACRO __func__
#endif // HOL_FUNC_MACRO_PRETTY

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
constexpr std::size_t size(C const& c) noexcept
{
    return c.size();
}

template<typename T, std::size_t N>
constexpr std::size_t size(const T(&)[N]) noexcept
{
    return N;
}

} // namespace private_details

#define bug(m) do{std::ostringstream o;o<<m<<'\n';std::cout<<o.str();}while(0)
#define bug_var(v) bug(#v ": " << std::boolalpha << v)

#define bug_cnt(c) \
	do{ \
		std::ostringstream o; \
		o << #c ": " << header_only_library::private_details::size(c) << '\n'; \
		int i = 0; \
		for(auto&& v_c: c) \
			{o << (i<100?" ":"") << (i<10?" ":"") << i << ": " << v_c << '\n';++i;} \
		std::cout << o.str(); \
	}while(0)

#define bug_itr(c, b, e) \
	do{ \
		std::ostringstream o; \
		o << #c ": " << std::distance(b, e) << '\n'; \
		int n=0; \
		for(auto i = (b); i != (e); ++i) \
			{o << " " << (n<100?" ":"") << (n<10?" ":"") << n << ": " << *i << '\n';++n;} \
		std::cout << o.str(); \
	}while(0)

struct scope_bomb
{
	std::string m;
	scope_bomb(const std::string& m)
	       : m(m) { bug("--> " << m); }
	~scope_bomb() { bug("<-- " << m); }
};

#define bug_fun() header_only_library::scope_bomb scope_bomb_inst(header_only_library::get_edit_bug_fun()(HOL_FUNC_MACRO))
#define bug_scope(m) header_only_library::scope_bomb scope_bomb_inst(m)
#endif

struct bug_type
{
	bug_type() { bug_fun(); }
	bug_type(bug_type&&) { bug_fun(); }
	bug_type(bug_type const&) { bug_fun(); }
	~bug_type() { bug_fun(); }

	bug_type& operator=(bug_type&&) { bug_fun(); return *this; }
	bug_type& operator=(bug_type const&) { bug_fun(); return *this; }
};

// BACKTRACE LINUX

#ifdef __unix__

inline
void signal_received(int sig, siginfo_t* info, void* context)
{
	auto uc = reinterpret_cast<ucontext_t*>(context);

	void* trace[16];
	char** messages = nullptr;

	std::cout << "Signal " << sig << '\n';

	if(sig == SIGSEGV)
	{
//		std::cout << "  Address violation at: " << info->si_addr << '\n';
		std::cout << "  From function       : " << (void*)uc->uc_mcontext.gregs[REG_RIP] << '\n';
	}

	int trace_size = backtrace(trace, 16);

	messages = backtrace_symbols(trace, trace_size);

	std::cout << "[bt] Execution path:" << '\n';
	for(int i = 2; i < trace_size; ++i)
	{
		// [bt] testcpp-debug(_Z15signal_receivediP9siginfo_tPv+0x3d6) [0x411f11]
		if(auto pos = std::strchr(messages[i], '('))
		{
			if(auto end = std::strchr(++pos, '+'))
			{
				std::string mangled(pos, end);
				int status;
				if(auto name = abi::__cxa_demangle(mangled.c_str(), 0, 0, &status))
				{
					mangled = name;
					std::free(name);
				}

				std::cout << "[bt] " << mangled << '\n';
				continue;
			}
		}

		std::cout << "[bt] " << messages[i] << '\n';
	}

	std::exit(0);
}

inline
void set_backtrace()
{
//	struct sigaction sa;
//	sa.sa_sigaction = signal_received;
//	sigemptyset(&sa.sa_mask);
//	sa.sa_flags = SA_SIGINFO|SA_RESTART;
//	sigaction(SIGINT, &sa, nullptr);
//	sigaction(SIGSEGV, &sa, nullptr);
}

#endif // __unix__

} // header_only_library

#endif // HEADER_ONLY_LIBRARY_BUG_H

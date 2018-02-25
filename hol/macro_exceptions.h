#ifndef HOL_MACRO_EXCEPTIONS_H
#define HOL_MACRO_EXCEPTIONS_H
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

#include <cerrno>
#include <sstream>
#include <stdexcept>
#include <cstring>

#ifdef NDEBUG
#   define hol_throw_exception(e, m) do{std::ostringstream o;o<<m;throw e(o.str());}while(0)
#else
#   define hol_throw_exception(e, m) do{ \
	std::ostringstream o; \
	o << __FILE__ << ":" << __LINE__ << ":error: " << m; \
	throw e(o.str());}while(0)
#endif // NDEBUG

#define hol_throw_runtime_error(m) hol_throw_exception(std::runtime_error, m)
#define hol_throw_errno() hol_throw_runtime_error(std::strerror(errno))
#define hol_throw_errno_msg(m) hol_throw_runtime_error(m << ": " << std::strerror(errno))

#ifdef __linux__
#include <string>
#include <vector>
#ifndef NDEBUG
#include <cxxabi.h>
#include <csignal>
#include <ucontext.h>
#include <execinfo.h>

namespace header_only_library {
namespace macro_exceptions {

class tracable_runtime_error
: public std::runtime_error
{
public:
	tracable_runtime_error(): std::runtime_error(std::string()) {}
	tracable_runtime_error(std::string const& msg): std::runtime_error(msg)
	{
		make_backtrace();
	}

	std::vector<std::string> const& backtrace() const { return bt; }

	std::vector<std::string> make_backtrace()
	{
		void* trace[32];

		int trace_size = ::backtrace(trace, sizeof(trace) / sizeof(trace[0]));

		char** messages = backtrace_symbols(trace, trace_size);

		for(int i = 2; i < trace_size - 2; ++i)
		{
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

					std::string pre(messages[i], pos);
					std::string post(end);
					bt.push_back(pre + mangled + post);
					continue;
				}
			}

			bt.push_back(messages[i]);
		}

		std::free(messages);

		return bt;
	}

private:
	std::vector<std::string> bt;
};

} // namespace macro_exceptions
} // namespace header_only_library

#else // NDEBUG

namespace header_only_library {
namespace macro_exceptions {

class tracable_runtime_error
: public std::runtime_error
{
public:
	tracable_runtime_error(): std::runtime_error(std::string()) {}
	tracable_runtime_error(char const* msg): std::runtime_error(msg) {}
	tracable_runtime_error(std::string const& msg): std::runtime_error(msg) {}
	std::vector<std::string> const& backtrace() const
	{
		static const std::vector<std::string> bt;
		return bt;
	}

};

} // namespace macro_exceptions
} // namespace header_only_library

#endif // NDEBUG
#define hol_throw_tracable_error(m) \
	hol_throw_exception(header_only_library::macro_exceptions::tracable_runtime_error, m)
#endif // __linux__

#endif // HOL_MACRO_EXCEPTIONS_H

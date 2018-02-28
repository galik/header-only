#ifndef HEADER_ONLY_LIBRARY_TIME_UTILE_H
#define HEADER_ONLY_LIBRARY_TIME_UTILE_H
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

#include <ctime>
#include <mutex>

namespace header_only_library {
namespace time_utils {

#if defined(__unix__)
#elif defined(_MSC_VER)
#else
inline std::mutex& ctime_mutex()
{
	static std::mutex mtx;
	return mtx;
}
#endif

/**
 * Thread-safe cross platform localtime function
 * @param timer
 * @return
 */
inline std::tm safe_localtime(std::time_t timer)
{
	std::tm bt {};
#if defined(__unix__)
	localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
	localtime_s(&bt, &timer);
#else
	std::lock_guard<std::mutex> lock(ctime_mutex());
	bt = *std::localtime(&timer);
#endif
	return bt;
}

/**
 * Thread-safe cross platform gmtime function
 * @param timer
 * @return
 */
inline std::tm safe_gmtime(std::time_t timer)
{
	std::tm bt {};
#if defined(__unix__)
	gmtime_r(&timer, &bt);
#elif defined(_MSC_VER)
	gmtime_s(&bt, &timer);
#else
	std::lock_guard<std::mutex> lock(ctime_mutex());
	bt = *std::gmtime(&timer);
#endif
	return bt;
}

inline std::time_t safe_mktime(std::tm& bt)
{
	return std::mktime(&bt);
}

/**
 * Thread-safe cross platform ctime function
 * @param timer
 * @return
 */
inline std::string safe_ctime(std::time_t timer)
{
#if defined(__unix__)
	char buf[26];
	ctime_r(&timer, buf);
	return {buf};
#else
	std::lock_guard<std::mutex> lock(ctime_mutex());
	return ctime(&timer);
#endif
}

// default = "YYYY-MM-DD HH:MM:SS"
inline
std::string time_stamp_at(std::time_t when, const std::string& fmt = "%F %T")
{
	auto bt = safe_localtime(when);
	char buf[64];
	return {buf, std::strftime(buf, sizeof(buf), fmt.c_str(), &bt)};
}

inline
std::string time_stamp_now(const std::string& fmt = "%F %T")
{
	return time_stamp_at(std::time(0), fmt);
}

[[deprecated]]
inline
std::string time_stamp(const std::string& fmt = "%F %T")
{
	return time_stamp_now(fmt);
}

} // time_utils
} // header_only_library

#endif // HEADER_ONLY_LIBRARY_TIME_UTILE_H

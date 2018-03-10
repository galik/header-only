#ifndef HEADER_ONLY_LIBRARY_BYTE_UTILS_H
#define HEADER_ONLY_LIBRARY_BYTE_UTILS_H
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

#include <array>
#include <cstddef>
#include <istream>
#include <string>
#include <vector>

#if __cplusplus < 201703
#error "This library requires C++17 or later."
#endif

namespace header_only_library {
namespace byte_utils {

using byte = std::byte;
template<std::size_t N>
using byte_buffer = std::array<byte, N>;
using byte_string = std::vector<byte>;

inline
std::istream& read(std::istream& is, byte* buf, std::size_t len)
	{ return is.read((char*)buf, len); }

inline
std::ostream& write(std::ostream& os, byte const* buf, std::size_t len)
	{ return os.write((char const*)buf, len); }

inline
std::istream& getline(std::istream& is, byte_string& line, byte delim = byte('\n'))
{
	std::string s;
	std::getline(is, s, char(delim));
	line = std::vector<byte>((byte*)s.data(), (byte*)s.data() + s.size());
	return is;
}

inline
std::istream& get(std::istream& is, byte& b)
	{ return is.get((char&)b); }

std::istream& get(std::istream& is,  byte* b, std::streamsize count)
	{ return is.get((char*)b, count); }

std::istream& get(std::istream& is,  byte* b, std::streamsize count, byte delim)
	{ return is.get((char*)b, count, char(delim)); }

std::istream& get(std::istream& is,  std::streambuf& buf)
	{ return is.get(buf); }

std::istream& get(std::istream& is,  std::streambuf& buf, byte delim)
	{ return is.get(buf, char(delim)); }

} // namespace byte_utils
} // namespace header_only_library

inline
std::istream& operator>>(std::istream& is, std::byte& b)
	{ char c; is >> c; b = std::byte(c); return is; }

inline
std::ostream& operator<<(std::ostream& os, std::byte b)
	{ return os << char(b); }

inline
auto operator+(
	header_only_library::byte_utils::byte_string a,
	header_only_library::byte_utils::byte_string b)
{
	header_only_library::byte_utils::byte_string s;
	s.reserve(std::size(a) + std::size(b));
	s.insert(std::end(s), std::begin(a), std::end(a));
	s.insert(std::end(s), std::begin(b), std::end(b));
	return s;
}

#endif // HEADER_ONLY_LIBRARY_BYTE_UTILS_H

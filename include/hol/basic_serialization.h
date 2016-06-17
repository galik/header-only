#ifndef HOL_BASIC_SERIALIZATION_H
#define HOL_BASIC_SERIALIZATION_H
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

#include <string>
#include <istream>
#include <ostream>

namespace hol { namespace basic_serialization {

namespace txt {

template<typename T>
std::ostream& serialize(std::ostream& os, T const& v)
{
	return os << ' ' << v;
}

std::ostream& serialize(std::ostream& os, std::string const& s)
{
	return (serialize(os, s.size()) << ' ').write(s.data(), s.size());
}

template<typename T>
std::istream& deserialize(std::istream& is, T& v)
{
	return is >> v;
}

std::istream& deserialize(std::istream& is, std::string& s)
{
	std::string::size_type n;

	if(!deserialize(is, n))
		return is;

	s.resize(n);
	return is.read(&s[0], n);
}

} // txt

namespace bin {

template<typename T>
std::ostream& serialize(std::ostream& os, T const& v)
{
	return os.write((char*)&v, sizeof(v));
}

std::ostream& serialize(std::ostream& os, std::string const& s)
{
	return (serialize(os, s.size())).write(s.data(), s.size());
}

template<typename T>
std::istream& deserialize(std::istream& is, T& v)
{
	return is.read((char*)&v, sizeof(v));
}

std::istream& deserialize(std::istream& is, std::string& s)
{
	std::string::size_type n;

	if(!deserialize(is, n))
		return is;

	s.resize(n);
	return is.read(&s[0], n);
}

} // bin

}} // hol::basic_serialization

#endif // HOL_BASIC_SERIALIZATION_H

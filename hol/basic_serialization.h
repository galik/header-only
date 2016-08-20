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

/**
 * Basic Serialization Library
 *
 * Example usage:
 *

 #include <hol/bug.h>
 #include <hol/basic_serialization.h>

class MyType
{
	int value;
	double factor;
	std::string type;

public:
	MyType()
	: value(0), factor(0.0), type("none") {}
	MyType(int value, double factor, const std::string& type)
	: value(value), factor(factor), type(type) {}

	// Serialized output
	friend std::ostream& operator<<(std::ostream& os, const MyType& m)
	{
		using namespace hol::basic_serialization::txt;
		serialize(os, m.value);
		serialize(os, m.factor);
		serialize(os, m.type);
		return os;
	}

	// Serialized input
	friend std::istream& operator>>(std::istream& is, MyType& m)
	{
		using namespace hol::basic_serialization::txt;
		deserialize(is, m.value);
		deserialize(is, m.factor);
		deserialize(is, m.type);
		return is;
	}
};

int main()
{
	std::vector<MyType> v {{ 1, 2.7, "one" }, { 4, 5.1, "two" }, { 3, 0.6, "three" }};

	{
		using namespace hol::basic_serialization::txt;
		bug_cnt(v);
	}

	std::stringstream ss;

	{
		using namespace hol::basic_serialization::txt;
		ss << v;
	}

	bug_var(ss.str());

	std::vector<MyType> v2;

	{
		using namespace hol::basic_serialization::txt;
		ss >> v2;
	}

	{
		using namespace hol::basic_serialization::txt;
		bug_cnt(v2);
	}

}
 *
 */

#include <string>
#include <istream>
#include <ostream>

#include <list>
#include <vector>

#include "bug.h"

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
	is >> v;
	return is;
}

std::istream& deserialize(std::istream& is, std::string& s)
{
	std::size_t n {};
	if(!deserialize(is, n))
		return is;

	s.resize(n);
	return is.ignore().read(&s[0], n);
}

template<typename Container>
std::ostream& serialize_container(std::ostream& os, Container& c)
{
	std::size_t n = c.size();
	serialize(os, n);
	for(auto const& v: c)
		os << v;
	return os;
}

template<typename Container>
std::istream& deserialize_container(std::istream& is, Container& c)
{
	std::size_t n {};

	if(!(is >> n))
		return is;

	c.clear();
	std::insert_iterator<Container> ii(c, c.end());

	typename Container::value_type v;

	while(n--)
		if(is >> v)
			*ii++ = v;

	return is;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> const& c)
{
	return serialize_container(os, c);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, std::list<T> const& c)
{
	return serialize_container(os, c);
}

template<typename T>
std::istream& operator>>(std::istream& is, std::vector<T>& c)
{
	return deserialize_container(is, c);
}

template<typename T>
std::istream& operator>>(std::istream& is, std::list<T>& c)
{
	return deserialize_container(is, c);
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
	std::size_t n {};
	if(!deserialize(is, n))
		return is;

	s.resize(n);
	return is.read(&s[0], n);
}

template<typename Container>
std::ostream& serialize_container(std::ostream& os, Container& c)
{
	std::size_t n = c.size();
	serialize(os, n);
	for(auto const& v: c)
		os << v;
	return os;
}

template<typename Container>
std::istream& deserialize_container(std::istream& is, Container& c)
{
	std::size_t n {};
	if(!deserialize(is, n))
		return is;

	c.clear();
	std::insert_iterator<Container> ii(c, c.end());

	typename Container::value_type v;
	while(n--)
		if(is >> v)
			*ii++ = v;
	return is;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> const& c)
{
	return serialize_container(os, c);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, std::list<T> const& c)
{
	return serialize_container(os, c);
}

template<typename T>
std::istream& operator>>(std::istream& is, std::vector<T>& c)
{
	return deserialize_container(is, c);
}

template<typename T>
std::istream& operator>>(std::istream& is, std::list<T>& c)
{
	return deserialize_container(is, c);
}

} // bin

}} // hol::basic_serialization

#endif // HOL_BASIC_SERIALIZATION_H

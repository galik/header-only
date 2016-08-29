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

// -- Current Version -------------------------------
#define HOL_BASIC_SERIALIZATION_VERSION_MAJ 0
#define HOL_BASIC_SERIALIZATION_VERSION_MIN 1
#define HOL_BASIC_SERIALIZATION_VERSION_FIX 0

#define HOL_BASIC_SERIALIZATION_VERSION \
( \
	HOL_BASIC_SERIALIZATION_VERSION_MAJ * 10000 \
	+ HOL_BASIC_SERIALIZATION_VERSION_MIN * 100 \
	+ HOL_BASIC_SERIALIZATION_VERSION_FIX \
)
// --------------------------------------------------

// set default library version
#ifndef HOL_USEING_BASIC_SERIALIZATION_VERSION
#define HOL_USEING_BASIC_SERIALIZATION_VERSION HOL_BASIC_SERIALIZATION_VERSION
#endif

// version 0.1.0
#if HOL_USEING_BASIC_SERIALIZATION_VERSION == 100

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
#include <sstream>
#include <stdexcept>

#include <set>
#include <list>
#include <vector>

#include "bug.h"

#ifdef NDEBUG
#define hol_basic_serialization_throw(m) \
	do{std::ostringstream o;o<<m;throw std::runtime_error(o.str());}while(0)
#else
#define hol_basic_serialization_throw(m) \
	do{ \
		std::ostringstream o; \
		o<<m<<" in "<<__FILE__<<" at: "<<__LINE__; \
		throw std::runtime_error(o.str()); \
	}while(0)
#endif

namespace hol { namespace basic_serialization {

namespace detail {

struct serialized_ptr
{
	void* ptr;
	std::size_t id;

	serialized_ptr(void* ptr, std::size_t id): ptr(ptr), id(id) {}

	bool operator<(serialized_ptr const& r) const { return ptr < r.ptr; }

//	friend std::ostream& operator<<(std::ostream& s, serialized_ptr const& r)
//	{
//		return s << '{' << r.ptr << ", " << r.id << '}';
//	}
};

class serialization_context
{
	std::size_t n = 0;
	std::set<serialized_ptr> m;

public:

	void start() { clear(); }
	void clear() { m.clear(); n = 0; }

	/**
	 *
	 * @param ptr
	 * @param pos
	 * @return true - a previous pointer was found else false
	 */
	bool push(void* ptr, std::size_t& id)
	{
		auto found = std::find_if(m.begin(), m.end(),
			[ptr](serialized_ptr const& r){return r.ptr == ptr;});

		if(found != m.end())
		{
			id = found->id;
			return true;
		}

		id = m.emplace(ptr, n++).first->id;
		return false;
	}

	void push(void* ptr)
	{

		if(!m.emplace(ptr, n++).second)
			hol_basic_serialization_throw
				("pushing same pointer twice");
	}

	bool pull(std::size_t id, void*& ptr)
	{
		auto found = std::find_if(m.begin(), m.end(),
			[id](serialized_ptr const& r){return r.id == id;});

		if(found != m.end())
		{
			ptr = found->ptr;
			return true;
		}

		return false;
	}
};

inline
serialization_context& get_serialization_ctx()
{
	thread_local static serialization_context ctx;
	return ctx;
}

inline
void serialization_init()
{
	get_serialization_ctx().start();
}

} // detail

namespace txt {

using detail::serialization_init;

//inline
//std::ostream& serialize(std::ostream& os, std::size_t sz)
//{
//	return os << ' ' << sz;
//}
//
//inline
//std::ostream& serialize(std::ostream& os, long long i)
//{
//	return os << ' ' << i;
//}
//
//inline
//std::ostream& serialize(std::ostream& os, unsigned long long i)
//{
//	return os << ' ' << i;
//}
//
//inline
//std::ostream& serialize(std::ostream& os, float f)
//{
//	return os << ' ' << f;
//}
//
//inline
//std::ostream& serialize(std::ostream& os, double d)
//{
//	return os << ' ' << d;
//}

//=============================================
// SERIALIZATION
//=============================================

// If we don't recognize it sent it to <<

template<typename T>
std::ostream& serialize(std::ostream& os, T const& v)
{
	return os << ' ' << v;
}

inline
std::ostream& serialize(std::ostream& os, std::string const& s)
{
	return (serialize(os, s.size()) << ' ').write(s.data(), s.size());
}

template<typename T1, typename T2>
std::ostream& serialize(std::ostream& os, std::pair<T1, T2> const& p)
{
	serialize(os, p.first);
	serialize(os, p.second);
	return os;
}

template<typename Container>
std::ostream& serialize_container(std::ostream& os, Container& c)
{
	std::size_t n = c.size();
	serialize(os, n);
	for(auto const& v: c)
		serialize(os, v);
//		os << v;
	return os;
}

template<typename T>
std::ostream& serialize(std::ostream& os, std::vector<T> const& c)
{
	return serialize_container(os, c);
}

template<typename T>
std::ostream& serialize(std::ostream& os, std::deque<T> const& c)
{
	return serialize_container(os, c);
}

template<typename T>
std::ostream& serialize(std::ostream& os, std::list<T> const& c)
{
	return serialize_container(os, c);
}

template<typename T1, typename T2>
std::ostream& serialize(std::ostream& os, std::map<T1, T2> const& c)
{
	return serialize_container(os, c);
}

template<typename T1, typename T2>
std::ostream& serialize(std::ostream& os, std::set<T1, T2> const& c)
{
	return serialize_container(os, c);
}

template<typename Ptr>
std::ostream& serialize_ptr(std::ostream& s, const Ptr& ptr)
{
	std::size_t pos;
	if(!ptr)
		serialize(s, 'N');
	else if(detail::get_serialization_ctx().push(ptr, pos))
		serialize(serialize(s, 'R'), pos);
	else
		serialize(serialize(s, 'P'), *ptr);

	return s;
}

//=============================================
// DE-SERIALIZATION
//=============================================

template<typename T>
std::istream& deserialize(std::istream& is, T const& cv)
{
	auto& v = const_cast<T&>(cv);

	return is >> v;
}

inline
std::istream& deserialize(std::istream& is, std::string const& cs)
{
	auto& s = const_cast<std::string&>(cs);

	std::size_t n;
	if(!deserialize(is, n))
		return is;

	s.resize(n);
	return is.ignore().read(&s[0], n);
}

template<typename T1, typename T2>
std::istream& deserialize(std::istream& is, std::pair<T1, T2>& p)
{
	deserialize(is, p.first);
	deserialize(is, p.second);
	return is;
}

template<typename Container>
std::istream& deserialize_container(std::istream& is, Container& c)
{
	std::size_t n;
	if(!deserialize(is, n))
		return is;

	c.clear();
	std::insert_iterator<Container> ii(c, c.end());

	typename std::remove_const<typename Container::value_type>::type v;

	while(n--)
		if(deserialize(is, v))
			*ii++ = v;

	return is;
}

template<typename T>
std::istream& deserialize(std::istream& is, std::vector<T>& c)
{
	return deserialize_container(is, c);
}

template<typename T>
std::istream& deserialize(std::istream& is, std::deque<T>& c)
{
	return deserialize_container(is, c);
}

template<typename T>
std::istream& deserialize(std::istream& is, std::list<T>& c)
{
	return deserialize_container(is, c);
}

template<typename T1, typename T2>
std::istream& deserialize(std::istream& is, std::map<T1, T2>& c)
{
	return deserialize_container(is, c);
}

template<typename T1, typename T2>
std::istream& deserialize(std::istream& is, std::set<T1, T2>& c)
{
	return deserialize_container(is, c);
}

template<typename Ptr>//, typename std::enable_if<std::is_pointer<Ptr>::value>::type>
std::istream& deserialize_ptr(std::istream& s, Ptr& ptr)
{
	char c;
	deserialize(s, c);

	if(c == 'N')
		ptr = nullptr;
	else if(c == 'P')
	{
		ptr = new typename std::remove_pointer<Ptr>::type;
		detail::get_serialization_ctx().push(ptr);
		deserialize(s, *ptr);
	}
	else if(c == 'R')
	{
		std::size_t pos;
		deserialize(s, pos);

		void* vp;
		if(!detail::get_serialization_ctx().pull(pos, vp))
			throw std::runtime_error("deserialization error, pointer not found");

		ptr = (Ptr) vp;
	}
	else
	{
		throw std::runtime_error("deserialization error, expected reference");
	}

	return s;
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
} // basic_serialization
} // hol

#endif // version 0.1.0 (100)

#endif // HOL_BASIC_SERIALIZATION_H

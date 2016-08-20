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
#include <cctype>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <algorithm>

#include "test.h"
#include <hol/random_utils.h>
#include <hol/basic_serialization.h>

class Serial
{
public:
	int i;
	long l;
	std::string s;
	double d;

public:
	Serial()
	: i(0), l(0), s(""), d(0) {}
	Serial(int i, long l, std::string const& s, double d)
	: i(i), l(l), s(s), d(d) {}

	bool operator==(Serial const& v) const
	{
//		return i == v.i && l == v.l && s == v.s && d == v.d;
		return i == v.i && l == v.l && s == v.s;
	}

	friend std::ostream& operator<<(std::ostream& os, Serial const& s)
	{
//		using namespace hol::basic_serialization::bin;
		using namespace hol::basic_serialization::txt;

		serialize(os, s.i);
		serialize(os, s.l);
		serialize(os, s.s);
		serialize(os, s.d);
		return os;
	}
	friend std::istream& operator>>(std::istream& is, Serial& s)
	{
//		using namespace hol::basic_serialization::bin;
		using namespace hol::basic_serialization::txt;

		deserialize(is, s.i);
		deserialize(is, s.l);
		deserialize(is, s.s);
		deserialize(is, s.d);
		return is;
	}
};

Serial random_serial()
{
	thread_local static std::mt19937 mt{std::random_device{}()};
	thread_local static std::uniform_int_distribution<int> uid{0, 30};
	thread_local static std::uniform_int_distribution<long> uld{0, 90};
	thread_local static std::uniform_real_distribution<double> udd{0.0, 1.0};
	return {uid(mt), uld(mt), "stuff", udd(mt)};
}

template<typename Numeric>
std::vector<Numeric> get_random_data(const size_t n, const Numeric from, const Numeric to)
{
	thread_local static hol::PRNG<Numeric> prng;
	std::vector<Numeric> v(n);
	std::generate(v.begin(), v.end(), [&]{return prng.get(from, to);});
	return v;
}

enum
{
	a_char,
	a_short,
	a_int,
	a_long,
	a_longlong,
	a_float,
	a_double,
	a_longdouble,
	a_size,
};

int main()
{
	try
	{
		hol::PRNG_32U rng;

		std::stringstream ss;

		switch(rng.get(a_size))
		{
//			case a_char: serialize(ss, )
		}



		return 0;


		std::vector<Serial> v;
		for(int i = 0; i < 10; ++i)
			v.push_back(random_serial());

		{
			using namespace hol::basic_serialization::txt;
			std::cout << v << '\n';
		}

		{
	//		using namespace hol::basic_serialization::bin;
			using namespace hol::basic_serialization::txt;
			ss << v;
		}

		std::list<Serial> l;

		{
	//		using namespace hol::basic_serialization::bin;
			using namespace hol::basic_serialization::txt;
			ss >> l;
		}

		bug("----------------------------");

		{
			using namespace hol::basic_serialization::txt;
			std::cout << l << '\n';
		}

		std::cout << std::boolalpha << ' ' << std::equal(v.begin(), v.end(), l.begin()) << '\n';
	}
	catch(const std::exception& e)
	{
		ERR(e.what());
	}
	catch(...)
	{
		ERR("Unknown exception thrown");
	}
}








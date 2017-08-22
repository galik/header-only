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

#include <algorithm>
#include <string>
#include <vector>

#include "../../hol/zstring_utils.h"
#include "hol/bug.h"
#include "hol/random_numbers.h"
#include "hol/string_utils.h"
#include "hol/timers.h"

namespace hol{
	using namespace header_only_library::random_numbers;
	using namespace header_only_library::timers;
	using namespace header_only_library::zstring_utils;
}

struct result
{
	unsigned sum = 0;
	double milis = 0.0;
};

template<typename Func>
result time_trial(std::size_t N, Func func)
{
	hol::StdTimer timer;
	result r{};

	timer.start();
	for(std::size_t n = 0; n < N; ++n)
		r.sum += func(r.sum);
	timer.stop();

	r.milis = timer.msecs();

	return r;
}

std::string random_string(std::size_t min, std::size_t max)
{
	assert(min > 0);
	assert(max >= min);

	std::string const alphanum = "abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"0123456789";

	std::size_t n = hol::random_number(min, max);

	std::string s;
	s.reserve(n);

	while(n--)
		s += hol::random_element(alphanum);

	return s;
}

std::size_t const N = 10000000;

int main()
{
	std::vector<std::pair<std::string, std::string>> tests;

	for(std::size_t i = 0; i < 100; ++i)
		tests.emplace_back(random_string(i + 1, i + 3), random_string(i + 1, i + 3));

	for(auto& test: tests)
		if(hol::random_choice(0.25))
			test.second = test.first;

	{
		auto r1 = time_trial(N, [&](std::size_t sum){
			return std::accumulate(std::begin(tests), std::end(tests), sum,
				[&](std::size_t n, auto const& test){
					return n + hol::equal_to(test.first.c_str(), test.second.c_str());
			});
		});

		std::cout << "time1: (" << r1.sum << ") " << (r1.milis / 1000.0) << " secs" << '\n';

		auto r2 = time_trial(N, [&](std::size_t sum){
			return std::accumulate(std::begin(tests), std::end(tests), sum,
				[&](std::size_t n, auto const& test){
					return n + !std::strcmp(test.first.c_str(), test.second.c_str());
			});
		});

		std::cout << "time2: (" << r2.sum << ") " << (r2.milis / 1000.0) << " secs" << '\n';

		std::cout << "     : " << std::boolalpha << (r1.sum == r2.sum) << '\n';
	}
}









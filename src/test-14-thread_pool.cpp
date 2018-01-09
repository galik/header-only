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

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <algorithm>
#include <vector>

//#include "hol/bug.h"
#include "hol/random_numbers.h"
#include "hol/thread_utils.h"

namespace hol {
	using namespace header_only_library::random_numbers;
	using namespace header_only_library::thread_utils;
}

TEST_CASE("Thread Pool Tests", "thread_pool")
{
	SECTION("straight run")
	{
		hol::thread_pool pool;//{2};
		pool.start(2);

		std::vector<int> v0;
		std::generate_n(std::back_inserter(v0), 100000, []{ return hol::random_number(100); });
		std::sort(std::begin(v0), std::end(v0));

		std::mutex mtx;
		std::vector<int> v1;

		std::size_t chunk_size = 1000;

		for(std::size_t pos = 0; pos < v0.size() + chunk_size; pos += chunk_size)
		{
			auto end = std::min(v0.size(), pos + chunk_size);

			pool.add([&mtx, &v0, &v1, pos, end]
			{
				for(auto p = pos; p < end; ++p)
				{
					std::lock_guard<std::mutex> lock(mtx);
					v1.push_back(v0[p]);
				}
			});
		}

		pool.stop();

		std::sort(std::begin(v1), std::end(v1));

		REQUIRE(v1 == v0);
	}

	SECTION("stop start")
	{
		hol::thread_pool pool;

		for(auto n = 1; n < 10; ++n)
		{
			pool.start(n);

			std::vector<int> v0;
			std::generate_n(std::back_inserter(v0), 1000, []{ return hol::random_number(100); });
			std::sort(std::begin(v0), std::end(v0));

			std::mutex mtx;
			std::vector<int> v1;

			std::size_t chunk_size = 100;

			for(std::size_t pos = 0; pos < v0.size() + chunk_size; pos += chunk_size)
			{
				auto end = std::min(v0.size(), pos + chunk_size);

				pool.add([&mtx, &v0, &v1, pos, end]
				{
					for(auto p = pos; p < end; ++p)
					{
						std::lock_guard<std::mutex> lock(mtx);
						v1.push_back(v0[p]);
					}
				});
			}

			pool.stop();

			std::sort(std::begin(v1), std::end(v1));

			REQUIRE(v1 == v0);
		}
	}
}



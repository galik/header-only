#ifndef HOL_TEST_H
#define HOL_TEST_H
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

#include <map>
#include <chrono>
#include <string>
#include <vector>
#include <vector>
#include <iomanip>
#include <iostream>

#include <experimental/filesystem>
#include <experimental/string_view>

#include "../hol/bug.h"
#define OUT_B(msg) do{std::cout << std::boolalpha << msg;}while(0)
#define OUT(msg) do{std::cout << std::boolalpha << msg << std::endl;}while(0)
#define OUT_E(msg) OUT(msg)
#define ERR(msg) do{std::cerr << msg << std::endl;}while(0)

namespace hol {

namespace ex = std::experimental;
namespace fs = ex::filesystem;

using str = std::string;
using str_map = std::map<str, str>;
using str_vec = std::vector<str>;

using svw = ex::string_view;
using svw_map = std::map<svw, svw>;
using svw_vec = std::vector<svw>;

using namespace std::chrono;

class result
{
	using clock = std::chrono::steady_clock;

	bool success = false;
	clock::time_point b = clock::now();
	clock::time_point e = b;

public:
	str name;
	unsigned id;
	str reason;

	result(str const& name, unsigned id): name(name), id(id) {}

	void start() { b = clock::now(); }
	void stop() { e = clock::now(); }

	void successful() { stop(); success = true; }
	void unsuccessful(str const& reason) { stop(); success = false; this->reason = reason; }

	float time() const { return duration_cast<milliseconds>(e - b).count() / 1000.0; }

	explicit operator bool() const { return success; }
};

using result_vec = std::vector<result>;

} // hol

#endif // HOL_TEST_H


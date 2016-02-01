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
#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include "bug.h"

#include <experimental/filesystem>
#include <experimental/string_view>

#define con(msg) do{std::cout << std::boolalpha << msg << '\n';}while(0)
#define err(msg) do{std::cerr << msg << std::endl;}while(0)

namespace xxx = std::experimental;
namespace fs = xxx::filesystem;

namespace hol {

using str = std::string;
using str_map = std::map<str, str>;
using str_vec = std::vector<str>;

using svw = xxx::string_view;
using svw_map = std::map<svw, svw>;
using svw_vec = std::vector<svw>;

}

#endif // HOL_TEST_H


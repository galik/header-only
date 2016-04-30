#ifndef HOL_SMALL_TYPES_H
#define HOL_SMALL_TYPES_H
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
 * The purpose of this library is to provide small aliases
 * for very commonly used types in order to render code more
 * readable.
 */

#include <map>
#include <set>
#include <list>
#include <regex>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

namespace hol { namespace small_types {

namespace basic {

using str = std::string;
using siz = std::size_t;

} // basic

namespace string_containers {

using namespace basic;

using str_set = std::set<str>;
using str_vec = std::vector<str>;
using str_map = std::map<str, str>;

} // string_containers

namespace regex {

using reg_itr = std::sregex_iterator;
using creg_itr = std::cregex_iterator;
static const reg_itr reg_itr_end{};
static const creg_itr creg_itr_end{};

} // regex

namespace ios {

using sis = std::istream;        // (s)standard (i)nput           (s)tream
using sos = std::ostream;        // (s)standard         (o)utput  (s)tream
using sios = std::iostream;      // (s)standard (i)nput (o)utput  (s)tream

using sss = std::stringstream;   // (s)standard          (s)tring (s)tream
using siss = std::istringstream; // (s)standard (i)nput  (s)tring (s)tream
using soss = std::ostringstream; // (s)standard (o)utput (s)tring (s)tream

using sfs = std::fstream;        // (s)standard          (f)ile   (s)tream
using sifs = std::ifstream;      // (s)standard (i)nput  (f)ile   (s)tream
using sofs = std::ofstream;      // (s)standard (o)utput (f)ile   (s)tream

// sometimes it is cumbersome to have long filenames for building
// stream parsers
// short names allows thing like
//
// while(sgl(sgl(is, key, '='), val)
//     m[key] = val;
//
// rather than
//
// while(std::getline(std::getline(is, key, '='), val)
//     m[key] = val;
//
namespace functions {
static const auto& sgl = static_cast<std::istream&(*)(std::istream&,std::string&,char)>(std::getline);
}

} // files

}} // hol::small_types

#endif // HOL_SMALL_TYPES_H

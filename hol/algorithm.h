#ifndef HOL_ALGORITHM_H
#define HOL_ALGORITHM_H
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

#include <algorithm>
#include <type_traits>

namespace hol_header_only_library {
namespace hol_algorithms {
namespace hol {

template<typename Container, typename Iter, typename Value>
void erase_remove(Container& c, Iter b, Iter e, Value v)
{
	static_assert(std::is_same<Value, typename Container::value_type>::value,
		"value must be same type as container's values");
	c.erase(std::remove(b, e, v), e);
}

template<typename Container, typename Value>
void erase_remove(Container& c, Value v)
{
	static_assert(std::is_same<Value, typename Container::value_type>::value,
		"value must be same type as container's values");
	erase_remove(c, std::begin(c), std::end(c), v);
}

template<typename Container, typename Iter, typename Pred>
void erase_remove_if(Container& c, Iter b, Iter e, Pred p)
{
	c.erase(std::remove_if(b, e, p), e);
}

template<typename Container, typename Pred>
void erase_remove_if(Container& c, Pred p)
{
	erase_remove_if(c, std::begin(c), std::end(c), p);
}

} // hol
} // hol_algorithms
} // hol_header_only_library

#endif // HOL_ALGORITHM_H

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

namespace header_only_library {
namespace algorithms {

/**
 * Safer erase_remove uses the two iterator erase overload so the
 * caller can't forget and get the single iterator overload by mistake
 * leading to UB.
 *
 * @param c The container to erase from.
 * @param b The first element from which to search for removals.
 * @param e The element beyond the last from which to search for removals.
 * @param v The value to search to be removed.
 */
template<typename Container>
void erase_remove(Container& c,
	typename Container::iterator b,
	typename Container::iterator e,
	typename Container::value_type v)
{
	c.erase(std::remove(b, e, v), e);
}

template<typename Container>
void erase_remove(Container& c,
	typename Container::value_type v)
{
	erase_remove(c, std::begin(c), std::end(c), v);
}

template<typename Container, typename Pred>
void erase_remove_if(Container& c,
	typename Container::iterator b,
	typename Container::iterator e,
	Pred p)
{
	c.erase(std::remove_if(b, e, p), e);
}

template<typename Container, typename Pred>
void erase_remove_if(Container& c, Pred p)
{
	erase_remove_if(c, std::begin(c), std::end(c), p);
}

} // algorithms
} // header_only_library

#endif // HOL_ALGORITHM_H

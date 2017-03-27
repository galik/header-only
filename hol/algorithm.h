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

///**
// * Safer erase_remove uses the two iterator erase overload so the
// * caller can't forget and get the single iterator overload by mistake
// * leading to UB.
// *
// * @param c The container to erase from.
// * @param b The first element from which to search for removals.
// * @param e The element beyond the last from which to search for removals.
// * @param v The value to search to be removed.
// */
//template<typename Container>
//void erase_remove(Container& c,
//	typename Container::iterator b,
//	typename Container::iterator e,
//	typename Container::value_type v)
//{
//	c.erase(std::remove(b, e, v), e);
//}
//
//template<typename Container>
//void erase_remove(Container& c,
//	typename Container::value_type v)
//{
//	erase_remove(c, std::begin(c), std::end(c), v);
//}
//
//template<typename Container, typename Pred>
//void erase_remove_if(Container& c,
//	typename Container::iterator b,
//	typename Container::iterator e,
//	Pred p)
//{
//	c.erase(std::remove_if(b, e, p), e);
//}
//
//template<typename Container, typename Pred>
//void erase_remove_if(Container& c, Pred p)
//{
//	erase_remove_if(c, std::begin(c), std::end(c), p);
//}

template<typename Container, typename Value>
auto find_in(Container&& c, Value const& v)
{
	return std::find(std::begin(std::forward<Container>(c)), std::end(std::forward<Container>(c)), v);
}

template<typename Container, typename Pred>
auto find_in_if(Container&& c, Pred p)
{
	return std::find_if(std::begin(std::forward<Container>(c)), std::end(std::forward<Container>(c)), p);
}

template<typename Container, typename Generator>
void shuffle_all(Container&& c, Generator&& g)
{
	std::shuffle(std::begin(std::forward<Container>(c)), std::end(std::forward<Container>(c)), std::forward<Container>(g));
}

template<typename Container>
void shuffle_all(Container&& c)
{
	thread_local static std::mt19937 mt{std::random_device{}()};
	shuffle_all(std::forward<Container>(c), mt);
}

template<typename Container, typename Value>
void erase_remove_from(Container&& c, Value const& v)
{
	c.erase(std::remove(std::begin(std::forward<Container>(c)), std::end(std::forward<Container>(c)), v), std::end(c));
}

template<typename Container, typename Pred>
void erase_remove_from_if(Container&& c, Pred p)
{
	c.erase(std::remove_if(std::begin(std::forward<Container>(c)), std::end(std::forward<Container>(c)), p), std::end(c));
}

} // algorithms
} // header_only_library

#endif // HOL_ALGORITHM_H

#ifndef HEADER_ONLY_LIBRARY_LOCKABLE_CONTAINERS_H
#define HEADER_ONLY_LIBRARY_LOCKABLE_CONTAINERS_H
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

#include "thread_utils.h"

#include <vector>
#include <deque>
#include <queue>
#include <stack>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

namespace header_only_library {
namespace thread_utils {
//namespace mt {

template<typename Container>
class lockable_container
: public lockable
, public Container
{
public:
	lockable_container() {}

	lockable_container(lockable_container&& lv)
	: lockable_container(std::move(lv), lv.lock_for_updates()) {}

	lockable_container& operator=(lockable_container&& lv)
	{
		lv.lock_for_updates();
		Container::operator=(std::move(lv));
		return *this;
	}

private:
	lockable_container(lockable_container&& lv, updatable_lock)
	: Container(std::move(lv)) {}
};

#define HOL_LOCKABLE_SEQUENCE_CONTAINER(Container) \
template<typename T> \
using lockable_ ## Container = lockable_container<std::Container<T>>

#define HOL_LOCKABLE_ASSOCIATIVE_CONTAINER(Container) \
template<typename K, typename V> \
using lockable_ ## Container = lockable_container<std::Container<K, V>>

HOL_LOCKABLE_SEQUENCE_CONTAINER(vector);
HOL_LOCKABLE_SEQUENCE_CONTAINER(deque);
HOL_LOCKABLE_SEQUENCE_CONTAINER(queue);
HOL_LOCKABLE_SEQUENCE_CONTAINER(stack);
HOL_LOCKABLE_SEQUENCE_CONTAINER(list);
HOL_LOCKABLE_SEQUENCE_CONTAINER(set);
HOL_LOCKABLE_SEQUENCE_CONTAINER(multiset);
HOL_LOCKABLE_SEQUENCE_CONTAINER(unordered_set);

HOL_LOCKABLE_ASSOCIATIVE_CONTAINER(map);
HOL_LOCKABLE_ASSOCIATIVE_CONTAINER(multimap);
HOL_LOCKABLE_ASSOCIATIVE_CONTAINER(unordered_map);

#define HOL_READ_ONLY_LOCKED_SEQUENCE_CONTAINER(Container) \
template<typename T> \
using read_only_locked_ ## Container = read_only_locked_version_of<lockable_ ## Container<T>>

#define HOL_READ_ONLY_LOCKED_ASSOCIATIVE_CONTAINER(Container) \
template<typename K, typename V> \
using read_only_locked_ ## Container = read_only_locked_version_of<lockable_ ## Container<K, V>>

HOL_READ_ONLY_LOCKED_SEQUENCE_CONTAINER(vector);
HOL_READ_ONLY_LOCKED_SEQUENCE_CONTAINER(deque);
HOL_READ_ONLY_LOCKED_SEQUENCE_CONTAINER(queue);
HOL_READ_ONLY_LOCKED_SEQUENCE_CONTAINER(stack);
HOL_READ_ONLY_LOCKED_SEQUENCE_CONTAINER(list);
HOL_READ_ONLY_LOCKED_SEQUENCE_CONTAINER(set);
HOL_READ_ONLY_LOCKED_SEQUENCE_CONTAINER(multiset);
HOL_READ_ONLY_LOCKED_SEQUENCE_CONTAINER(unordered_set);

HOL_READ_ONLY_LOCKED_ASSOCIATIVE_CONTAINER(map);
HOL_READ_ONLY_LOCKED_ASSOCIATIVE_CONTAINER(multimap);
HOL_READ_ONLY_LOCKED_ASSOCIATIVE_CONTAINER(unordered_map);

#define HOL_UPDATABLE_LOCKED_SEQUENCE_CONTAINER(Container) \
template<typename T> \
using updatable_locked_ ## Container = updatable_locked_version_of<lockable_ ## Container<T>>

#define HOL_UPDATABLE_LOCKED_ASSOCIATIVE_CONTAINER(Container) \
template<typename K, typename V> \
using updatable_locked_ ## Container = updatable_locked_version_of<lockable_ ## Container<K, V>>

HOL_UPDATABLE_LOCKED_SEQUENCE_CONTAINER(vector);
HOL_UPDATABLE_LOCKED_SEQUENCE_CONTAINER(deque);
HOL_UPDATABLE_LOCKED_SEQUENCE_CONTAINER(queue);
HOL_UPDATABLE_LOCKED_SEQUENCE_CONTAINER(stack);
HOL_UPDATABLE_LOCKED_SEQUENCE_CONTAINER(list);
HOL_UPDATABLE_LOCKED_SEQUENCE_CONTAINER(set);
HOL_UPDATABLE_LOCKED_SEQUENCE_CONTAINER(multiset);
HOL_UPDATABLE_LOCKED_SEQUENCE_CONTAINER(unordered_set);

HOL_UPDATABLE_LOCKED_ASSOCIATIVE_CONTAINER(map);
HOL_UPDATABLE_LOCKED_ASSOCIATIVE_CONTAINER(multimap);
HOL_UPDATABLE_LOCKED_ASSOCIATIVE_CONTAINER(unordered_map);

//} // mt
} // thread_utils
} // header_only_library

#endif // HEADER_ONLY_LIBRARY_LOCKABLE_CONTAINERS_H

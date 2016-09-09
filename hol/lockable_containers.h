#ifndef HOL_LOCKABLE_CONTAINERS_H
#define HOL_LOCKABLE_CONTAINERS_H
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

namespace hol {
namespace thread_utils {
namespace mt {

template<typename Container>
class lockable_container
: public Container
, public lockable
{
public:
	using mutex_type = std::shared_timed_mutex;
	using read_lock = std::shared_lock<mutex_type>;
	using write_lock = std::unique_lock<mutex_type>;

	// default ctor
	lockable_container() {}

	// move-ctor
	lockable_container(lockable_container&& lv)
	: lockable_container(std::move(lv), lv.lock_for_writing()) {}

	// move-assign
	lockable_container& operator=(lockable_container&& lv)
	{
		lv.lock_for_writing();
		Container::operator=(std::move(lv));
		return *this;
	}

	read_lock lock_for_reading() { return read_lock(m); }
	write_lock lock_for_writing() { return write_lock(m); }

private:
	// hidden, locked move-ctor
	lockable_container(lockable_container&& lv, write_lock)
	: Container(std::move(lv)) {}

	mutex_type m;
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

} // mt
} // thread_utils
} // hol

#endif // HOL_LOCKABLE_CONTAINERS_H

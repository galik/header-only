#ifndef HOL_THREAD_UTILS_H
#define HOL_THREAD_UTILS_H
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

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <future>

#undef WARN_UNUSED_RESULT
#ifdef __GNUC__
//	[[nodiscard]] // C++17
#define WARN_UNUSED_RESULT [[gnu::warn_unused_result]]
#else
#define WARN_UNUSED_RESULT
#endif

#ifdef HOL_HAVE_GSL_SPAN
#include <gsl/span>
#endif // HOL_HAVE_GSL_SPAN

namespace header_only_library {
namespace thread_utils {
//namespace mt {

//=============================================================
//== Convenience threads/asyncs
//=============================================================
//

class detached_thread
{
	std::thread t;

public:
	template<typename Callable, typename... Args>
	explicit detached_thread(Callable&& f, Args&&... args)
	: t(std::forward<Callable>(f), std::forward<Args>(args)...) {t.detach();}
};

class raii_thread
{
	std::thread t;

public:
	raii_thread(raii_thread&& rt): t(std::move(rt.t)) {}

	template<typename Callable, typename... Args>
	raii_thread(Callable&& f, Args&&... args)
	: t(std::forward<Callable>(f), std::forward<Args>(args)...) {}

	~raii_thread() { if(t.joinable()) t.join(); }
};

class raii_task
{
	std::future<void> f;

public:
	raii_task(raii_task&& rt): f(std::move(rt.f)) {}

	template<typename Callable, typename... Args>
	raii_task(Callable&& f, Args&&... args)
	: f(std::async(std::launch::async, std::forward<Callable>(f), std::forward<Args>(args)...)) {}

	~raii_task() { if(f.valid()) f.get(); }
};

template<typename Return>
class raii_future
{
	std::future<Return> f;

public:
	raii_future(raii_future&& rt): f(std::move(rt.f)) {}

	template<typename Callable, typename... Args>
	raii_future(Callable&& f, Args&&... args)
	: f(std::async(std::launch::async, std::forward<Callable>(f), std::forward<Args>(args)...)) {}

	~raii_future() { if(f.valid()) f.get(); }

	bool valid() const { return f.valid(); }
	Return get() { return f.get(); }
};

class async_group
{
	std::vector<std::future<void>> futs;

public:
	~async_group() { for(auto&& fut: futs) if(fut.valid()) fut.get(); }

	template<typename... Args>
	void run(Args&&... args)
		{ futs.emplace_back(std::async(std::forward<Args>(args)...)); }

	template<typename... Args>
	void run_async(Args&&... args)
		{ run(std::launch::async, std::forward<Args>(args)...); }

	template<typename... Args>
	void run_deferred(Args&&... args)
		{ run(std::launch::deferred, std::forward<Args>(args)...); }
};

#ifdef HOL_HAVE_GSL_SPAN

/**
 * \brief Split the work of processing a contiguous data container's elements
 * among a supplied number of threads.
 * \tparam T container's data type
 * @param data A span view of a contiguous container
 * @param work A callable type to work on the supplied data
 * @param pieces The number of pieces to divide the data up into between threads
 */
//template<typename T, typename Work>
//void split_work(gsl::span<T> data, Work work, typename gsl::span<T>::index_type const pieces)
//{
//	using ssiz = typename gsl::span<T>::index_type;
//
//	if(!data.size())
//		return;
//
//	if(!pieces)
//		throw std::out_of_range("zero pieces to work with");
//
//	if(pieces > data.size())
//		throw std::out_of_range("more pieces than data elements");
//
//	auto rem = data.size() % pieces;
//	auto const part = data.size() / pieces;
//
//	mt::async_group asyncs;
//
//	for(ssiz p, i = 0; i < data.size(); i += p)
//	{
//		p = part;
//		if(rem)
//		{
//			++p;
//			--rem;
//		}
//
//		if((i + p) > data.size())
//			p -= ((i + p) - data.size());
//
//		assert(i + p <= data.size());
//		asyncs.run_async(work, gsl::span<int>(&data[i], p));
//	}
//}

#endif // HOL_HAVE_GSL_SPAN

//=============================================================
//== Lockable objects
//=============================================================
//
// Objects that can be locked for reading or writing
//

class lockable_base
{
public:
    using mutex_type = std::shared_timed_mutex;
    using read_only_lock  = std::shared_lock<mutex_type>;
    using updatable_lock = std::unique_lock<mutex_type>;

	virtual ~lockable_base() {};

	lockable_base() {}

	lockable_base(lockable_base const&) = delete;
	lockable_base(lockable_base&&) {}

	lockable_base& operator=(lockable_base const&) = delete;
	lockable_base& operator=(lockable_base&&) { return *this; }

	mutex_type& mutex() const { return mtx; }

private:
	mutable mutex_type mtx;
};

class lockable_for_reading
: public virtual lockable_base
{
public:
	lockable_for_reading() {}

	lockable_for_reading(lockable_for_reading&&) {};
	lockable_for_reading(lockable_for_reading const&) = delete;

	lockable_for_reading& operator=(lockable_for_reading&&) { return *this; }
	lockable_for_reading& operator=(lockable_for_reading const&) = delete;

	WARN_UNUSED_RESULT
	auto lock_for_reading() const { return read_only_lock(mutex()); }

	WARN_UNUSED_RESULT
	auto deferred_lock_for_reading() const { return read_only_lock(mutex(), std::defer_lock); }

	WARN_UNUSED_RESULT
	auto adopted_lock_for_reading() const { return read_only_lock(mutex(), std::adopt_lock); }
};

class lockable_for_updates
: public virtual lockable_base
{
public:
	lockable_for_updates() {}

	lockable_for_updates(lockable_for_updates&&) {};
	lockable_for_updates(lockable_for_updates const&) = delete;

	lockable_for_updates& operator=(lockable_for_updates&&) { return *this; }
	lockable_for_updates& operator=(lockable_for_updates const&) = delete;

	WARN_UNUSED_RESULT
	auto lock_for_updates() const { return updatable_lock(mutex()); }

	WARN_UNUSED_RESULT
	auto adopted_lock_for_updates() const { return updatable_lock(mutex(), std::adopt_lock); }

	WARN_UNUSED_RESULT
	auto deferred_lock_for_updates() const { return updatable_lock(mutex(), std::defer_lock); }
};

class lockable
: public virtual lockable_for_reading
, public virtual lockable_for_updates
{
public:
	lockable() {}

	lockable(lockable&&) = default;
	lockable(lockable const&) = delete;

	lockable& operator=(lockable&&) { return *this; }
	lockable& operator=(lockable const&) = delete;
};

//============================================================
// updatable and read_only locked versions of lockable objects
//============================================================
//
//

template<typename Lockable>
class read_only_locked_version_of
{
public:
	using const_pointer   = typename std::remove_reference<Lockable>::type const*;
	using const_reference = typename std::remove_reference<Lockable>::type const&;

	read_only_locked_version_of(Lockable const& lockable)
	: lockable(lockable), lock(lockable.lock_for_reading()) {}

	const_reference operator*()  const { return  lockable; }
	const_pointer   operator->() const { return &lockable; }

	const_reference ro()  const { return lockable; }
	const_reference get() const { return lockable; }

private:
	Lockable const& lockable;
	lockable::read_only_lock lock;
};

template<typename Lockable>
class updatable_locked_version_of
{
public:
	using pointer         = typename std::remove_reference<Lockable>::type*;
	using reference       = typename std::remove_reference<Lockable>::type&;
	using const_pointer   = typename std::remove_reference<Lockable>::type const*;
	using const_reference = typename std::remove_reference<Lockable>::type const&;

	updatable_locked_version_of(Lockable& lockable)
	: lockable(lockable), lock(lockable.lock_for_updates()) {}

	reference operator*()  { return  lockable; }
	pointer   operator->() { return &lockable; }

	reference rw()  { return lockable; }
	reference get() { return lockable; }

	const_reference operator*()  const { return  lockable; }
	const_pointer   operator->() const { return &lockable; }

	const_reference ro()  const { return lockable; }
	const_reference get() const { return lockable; }

private:
	Lockable& lockable;
	lockable::updatable_lock lock;
};

template<typename Lockable>
auto get_read_only_locked_version_of(Lockable const& lockable)
{
	return read_only_locked_version_of<Lockable>(lockable);
}

template<typename Lockable>
auto get_updatable_locked_version_of(Lockable& lockable)
{
	return updatable_locked_version_of<Lockable>(lockable);
}

//
//
//=============================================================
//== Locked iterating
//=============================================================
//
// Add the ability to iterate over a container managed
// by your class while maintaining a lock
//
// This is a proxy-class that holds the lock until it
// goes out of scope.
//
// Intended use is in range-based for-loops
//
//	class MyClass
//	: public mt::lockable_for_reading
//	, public mt::lockable_for_updates
//	{
//		std::vector<int> ints;
//
//	public:
//
//		auto locked_updatable_range()
//		-> mt::locked_iterable<updatable_lock, std::vector<int>::iterator>
//		{
//			return {lock_for_updates(), ints.begin(), ints.end()};
//		}
//
//		auto locked_read_only_range() const
//		-> mt::locked_iterable<read_only_lock, std::vector<int>::const_iterator>
//		{
//			return {lock_for_reading(), ints.cbegin(), ints.cend()};
//		}
//	};
//
//	// ...
//
//	MyClass mc;
//
//	for(auto& i: mc.locked_updatable_range())
//	{
//		// synchronized access to mc elements here
//	}
//
//-------------------------------------------------------------

template<typename LockType, typename IterType>
class locked_iterable
{
	LockType lock;

	IterType b;
	IterType e;

public:
	// we accept the iterators by reference to ensude we don't copy them
	// untim after the lock is secured
	locked_iterable(LockType&& lock, IterType const& b, IterType const& e)
	: lock(std::move(lock)), b(b), e(e) {}

	IterType begin() { return b; }
	IterType end() { return e; }
};

//} // mt
} // thread_utils
} // header_only_library

#endif // HOL_THREAD_UTILS_H

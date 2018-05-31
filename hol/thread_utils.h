#ifndef HEADER_ONLY_LIBRARY_THREAD_UTILS_H
#define HEADER_ONLY_LIBRARY_THREAD_UTILS_H
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

#include <future>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>
#include <cstddef> // std::size_t

//#include "bug.h"

#undef HOL_WARN_UNUSED_RESULT
#if __has_cpp_attribute(nodiscard)
#define HOL_WARN_UNUSED_RESULT [[nodiscard]]
#else
#ifdef __GNUC__
#define HOL_WARN_UNUSED_RESULT [[gnu::warn_unused_result]]
#else
#define HOL_WARN_UNUSED_RESULT
#endif
#endif // __has_cpp_attribute(nodiscard)

namespace header_only_library {
namespace thread_utils {

// types

class joining_thread
{
public:
	joining_thread() noexcept = default;

	joining_thread(joining_thread const&) = delete;
	joining_thread(joining_thread&& other) noexcept
	: t(std::move(other.t)) {}

	joining_thread(std::thread const&) = delete;
	joining_thread(std::thread&& other) noexcept
	: t(std::move(other)) {}

	joining_thread& operator=(joining_thread const&) = delete;
	joining_thread& operator=(joining_thread&& other) noexcept
		{ t = std::move(other.t); return *this; }

	joining_thread& operator=(std::thread const&) = delete;
	joining_thread& operator=(std::thread&& other) noexcept
		{ t = std::move(other); return *this; }

	template <typename Callable, typename... Args>
	explicit joining_thread(Callable&& f, Args&&... args)
		: t(std::forward<Callable>(f), std::forward<Args>(args)...) {}

	~joining_thread() { if (t.joinable()) t.join(); }

	std::thread::id get_id() const noexcept { return t.get_id(); }

	std::thread::native_handle_type native_handle()
		{ return t.native_handle(); }

	void swap(joining_thread& other) noexcept
		{ using std::swap; swap(t, other.t); }

	bool free() { return !t.joinable(); }

private:
	std::thread t;

	friend void swap(joining_thread& t1, joining_thread& t2) noexcept
	{
	    using std::swap;
	    swap(t1.t, t2.t);
	}
};

class cyclic_barrier
{
	struct context
	{
		context(std::size_t n): n(n) {}
		std::size_t n = 0;
		bool release = false;
		std::condition_variable cv;
		std::mutex mtx;
	};

	using context_sptr = std::shared_ptr<context>;

public:
	cyclic_barrier(std::size_t n): total_n(n), ctx{std::make_shared<context>(total_n)}
	{
		if(!n)
			throw std::invalid_argument("cyclic_barrier needs > 0 on initialization");
	}

	void wait()
	{
		std::unique_lock<std::mutex> lock(ctx->mtx);

		--ctx->n;

		if(!ctx->n)
		{
			// last one in opens the gate
			auto old_ctx = ctx;
			ctx = std::make_shared<context>(total_n);
			old_ctx->release = true;
			old_ctx->cv.notify_all();
		}
		else
		{
			// otherwise wait for the signal
			auto old_ctx = ctx;
			ctx->cv.wait(lock, [old_ctx]{ return old_ctx->release; });
		}
	}

private:
	std::size_t const total_n;
	context_sptr ctx;
};

class parallel_jobs
: private std::vector<joining_thread>
{
public:
	using std::vector<joining_thread>::reserve;
	using std::vector<joining_thread>::size;

	template<typename Callable, typename... Args>
	void add(Callable&& callable, Args&&... args)
	{
		emplace_back(std::forward<Callable>(callable), std::forward<Args>(args)...);
	}

	void wait() { clear(); }
};

template<typename... Funcs>
void parallel_invoke(Funcs... funcs)
{
	joining_thread _[]{joining_thread{funcs}...};
}

// For dividing work up among threads

template<typename Numeric>
std::vector<Numeric> divide_numeric_range(Numeric from, Numeric to, std::size_t n)
{
	std::vector<Numeric> pieces;
	pieces.reserve(n + 1);

	pieces.push_back(from);

	auto f = double(from);
	auto t = double(to);
	auto d = (t - f) / double(n);

	for(Numeric i = 0; i < n - 1; ++i)
		pieces.push_back(Numeric(f += d));

	pieces.push_back(to);

	return pieces;
}

template<typename Iter>
std::vector<Iter> divide_iterator_range(Iter from, Iter to, std::size_t n)
{
	auto d = std::size_t(std::distance(from, to));
	auto parts = divide_numeric_range(std::size_t(0), d, n);

	std::vector<Iter> pieces;
	pieces.reserve(parts.size());

	std::transform(std::begin(parts), std::prev(std::end(parts)),
		std::back_inserter(pieces),
			[from](std::size_t n){ return from + n; });

	pieces.push_back(to);

	return pieces;
}

//=============================================================
//== Lockable objects
//=============================================================
//
// Locked Objects that can be opened for reading or writing
//

/***
 * Trying to *copy* or *move* a lockable while a read_lockable or write_lockable
 * owns its `mutex` will cause *deadlock*.
 */

template <typename CRTP>
class locked_object_base
{
public:
    using mutex_type = std::shared_timed_mutex;

    locked_object_base() = default;
    virtual ~locked_object_base() = default;

    template <typename... Args>
    locked_object_base(Args&&... args) : crtp(std::forward<Args>(args)...)
    {
    }

//    template <typename T>
//    locked_object_base(std::initializer_list<T> il) : crtp(il)
//    {
//    }

    locked_object_base(locked_object_base const& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        copy_assign(other);
    }

    locked_object_base(locked_object_base&& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        move_assign(other);
    }

    locked_object_base& operator=(locked_object_base const& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        copy_assign(other);
        return *this;
    }

    locked_object_base& operator=(locked_object_base&& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        move_assign(other);
        return *this;
    }

protected:
    void copy_assign(locked_object_base const& other) { this->crtp = other.crtp; }

    void move_assign(locked_object_base& other) { this->crtp = std::move(other.crtp); }

    mutable mutex_type mtx;
    CRTP crtp;
};

template <typename CRTP>
class readable_locked_object : public virtual locked_object_base<CRTP>
{
    using mutex_type = typename locked_object_base<CRTP>::mutex_type;

public:
    class reading_accessor
    {
    public:
        reading_accessor() = default;

        reading_accessor(CRTP const& crtp, mutex_type& mtx) : crtp(&crtp), lock(mtx) {}

        reading_accessor(CRTP const& crtp, mutex_type& mtx, std::defer_lock_t)
            : crtp(&crtp), lock(mtx, std::defer_lock)
        {
        }

        // it should be impossible for a reading_lock to exist without an associated
        // locked mutex
        reading_accessor(reading_accessor const& other) : crtp(other.crtp), lock(other.lock.mtx) {}

        reading_accessor(reading_accessor&& other) : crtp(other.crtp), lock(std::move(other.lock)) {}

        reading_accessor& operator=(reading_accessor const& other)
        {
            lock = std::shared_lock<mutex_type>(other.lock.mtx);
            crtp = other.crtp;
            return *this;
        }

        reading_accessor& operator=(reading_accessor&& other)
        {
            lock = std::move(other.lock);
            crtp = other.crtp;
            return *this;
        }

        CRTP const& operator*() const { return *crtp; }
        CRTP const* operator->() const { return crtp; }

        std::shared_lock<mutex_type>& get_lock() { return lock; }

    private:
        CRTP const* crtp;
        std::shared_lock<mutex_type> lock;
    };

    readable_locked_object() = default;

    template <typename... Args>
    readable_locked_object(Args&&... args) : locked_object_base<CRTP>(std::forward<Args>(args)...)
    {
    }

    readable_locked_object(readable_locked_object const& other) : locked_object_base<CRTP>(other) {}
    readable_locked_object(readable_locked_object&& other) : locked_object_base<CRTP>(std::move(other)) {}

    readable_locked_object& operator=(readable_locked_object const& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        locked_object_base<CRTP>::copy_assign(other);
        copy_assign(other);
        return *this;
    }

    readable_locked_object& operator=(readable_locked_object&& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        locked_object_base<CRTP>::move_assign(other);
        move_assign(other);
        return *this;
    }

    HOL_WARN_UNUSED_RESULT
    reading_accessor open_for_reading() const
    {
        return reading_accessor(locked_object_base<CRTP>::crtp, locked_object_base<CRTP>::mtx);
    }

    HOL_WARN_UNUSED_RESULT
    reading_accessor open_for_deferred_reading() const
    {
        return reading_accessor(locked_object_base<CRTP>::crtp, locked_object_base<CRTP>::mtx, std::defer_lock);
    }

protected:
    void copy_assign(readable_locked_object const& other) {}
    void move_assign(readable_locked_object& other) {}
};

template <typename CRTP>
class writable_locked_object : public virtual locked_object_base<CRTP>
{
    using mutex_type = typename locked_object_base<CRTP>::mutex_type;

public:
    class writing_accessor
    {
    public:
        writing_accessor() = default;

        writing_accessor(CRTP& crtp, mutex_type& mtx) : crtp(&crtp), lock(mtx) {}

        writing_accessor(CRTP& crtp, mutex_type& mtx, std::defer_lock_t)
            : crtp(&crtp), lock(mtx, std::defer_lock)
        {
        }

        writing_accessor(writing_accessor const&) = delete;
        writing_accessor(writing_accessor&& other) : crtp(other.crtp), lock(std::move(other.lock)) {}

        writing_accessor& operator=(writing_accessor const&) = delete;
        writing_accessor& operator=(writing_accessor&& other)
        {
            lock = std::move(other.lock);
            crtp = other.crtp;
            return *this;
        }

        CRTP& operator*() { return *crtp; }
        CRTP* operator->() { return crtp; }

        std::unique_lock<mutex_type>& get_lock() { return lock; }

    private:
        CRTP* crtp;
        std::unique_lock<mutex_type> lock;
    };

    writable_locked_object() = default;

    template <typename... Args>
    writable_locked_object(Args&&... args) : locked_object_base<CRTP>(std::forward<Args>(args)...)
    {
    }

    writable_locked_object(writable_locked_object const& other) : locked_object_base<CRTP>(other) {}
    writable_locked_object(writable_locked_object&& other) : locked_object_base<CRTP>(std::move(other)) {}

    writable_locked_object& operator=(writable_locked_object const& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        locked_object_base<CRTP>::copy_assign(other);
        copy_assign(other);
        return *this;
    }

    writable_locked_object& operator=(writable_locked_object&& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        locked_object_base<CRTP>::move_assign(other);
        move_assign(other);
        return *this;
    }

    HOL_WARN_UNUSED_RESULT
    writing_accessor open_for_writing()
    {
        return writing_accessor(locked_object_base<CRTP>::crtp, locked_object_base<CRTP>::mtx);
    }

    HOL_WARN_UNUSED_RESULT
    writing_accessor open_for_deferred_writing()
    {
        return writing_accessor(locked_object_base<CRTP>::crtp, locked_object_base<CRTP>::mtx, std::defer_lock);
    }

protected:
    void copy_assign(writable_locked_object const& other) {}
    void move_assign(writable_locked_object& other) {}
};

template <typename CRTP>
class locked_object : public readable_locked_object<CRTP>, public writable_locked_object<CRTP>
{
    using mutex_type = typename locked_object_base<CRTP>::mutex_type;

public:
    template <typename... Args>
    locked_object(Args&&... args) : locked_object_base<CRTP>(std::forward<Args>(args)...)
    {
    }

    locked_object(locked_object const& other) : locked_object_base<CRTP>(other) {}
    locked_object(locked_object&& other) : locked_object_base<CRTP>(std::move(other)) {}

    locked_object& operator=(locked_object const& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        locked_object_base<CRTP>::copy_assign(other);
        readable_locked_object<CRTP>::copy_assign(other);
        writable_locked_object<CRTP>::copy_assign(other);
        copy_assign(other);
        return *this;
    }

    locked_object& operator=(locked_object&& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        locked_object_base<CRTP>::move_assign(other);
        readable_locked_object<CRTP>::move_assign(other);
        writable_locked_object<CRTP>::move_assign(other);
        move_assign(other);
        return *this;
    }

protected:
    void copy_assign(locked_object const& other) {}
    void move_assign(locked_object& other) {}
};

//template <typename CRTP>
//class lockable_container : public read_openable<CRTP>, public write_openable<CRTP>
//{
//public:
//    template <typename... Args>
//    lockable_container(Args&&... args) : lockable_base<CRTP>(std::forward<Args>(args)...)
//    {
//    }
//
//    template <typename T>
//    lockable_container(std::initializer_list<T> il) : lockable_base<CRTP>(il)
//    {
//    }
//};

namespace detail {

    template <typename Tuple, std::size_t... Is>
    void apply_multilock(Tuple& tp, std::index_sequence<Is...>)
    {
        std::lock(std::get<Is>(tp).get_lock()...);
    }

    template <typename CRTP>
    struct for_reading_type
    {
        locked_object<CRTP> const& l;
        for_reading_type(locked_object<CRTP> const& l) : l(l) {}
        auto lock() { return l.open_for_deferred_reading(); }
    };

    template <typename CRTP>
    struct for_writing_type
    {
        locked_object<CRTP>& l;
        for_writing_type(locked_object<CRTP>& l) : l(l) {}
        auto lock() { return l.open_for_deferred_writing(); }
    };

} // namespace detail

template <typename CRTP>
auto for_reading(locked_object<CRTP> const& l)
{
    return detail::for_reading_type<CRTP>(l);
}

template <typename CRTP>
auto for_writing(locked_object<CRTP>& l)
{
    return detail::for_writing_type<CRTP>(l);
}

template <typename... Lockables>
HOL_WARN_UNUSED_RESULT
auto open_locked_objects(Lockables&&... lockables)
{
    auto tp = std::make_tuple(std::forward<Lockables>(lockables).lock()...);
    detail::apply_multilock(tp, std::make_index_sequence<sizeof...(Lockables)>{});
    return tp;
}

} // thread_utils
} // header_only_library

// Alternative lock enabled objects

#include <experimental/optional>

namespace header_only_library {
namespace thread_utils {

template<typename T>
using optional_type = std::experimental::optional<T>;

class lock_enabled
{
	using mutex_type = std::shared_timed_mutex;

public:
	struct enable_reading {};
	struct enable_updates: enable_reading {};

	struct enable_reading_impl: enable_reading
	{
		explicit enable_reading_impl(mutex_type& mtx): lock(mtx) {}
		explicit enable_reading_impl(mutex_type& mtx, std::defer_lock_t): lock(mtx, std::defer_lock) {}
		explicit enable_reading_impl(mutex_type& mtx, std::chrono::microseconds wait): lock(mtx, wait) {}

//		enable_reading_impl(enable_reading_impl const&) = delete;
//		enable_reading_impl(enable_reading_impl&& access): lock(std::move(access.lock)) {}
//
//		enable_reading_impl& operator=(enable_reading_impl const&) = delete;
//		enable_reading_impl& operator=(enable_reading_impl&& access) { lock = std::move(access.lock); return *this; }

		explicit operator bool() const { return lock.owns_lock(); }
		std::shared_lock<mutex_type> lock;
	};

	struct enable_updates_impl: enable_updates
	{
		enable_updates_impl(mutex_type& mtx): lock(mtx) {}
		enable_updates_impl(mutex_type& mtx, std::defer_lock_t): lock(mtx, std::defer_lock) {}
		enable_updates_impl(mutex_type& mtx, std::chrono::microseconds wait): lock(mtx, wait) {}
		explicit operator bool() const { return lock.owns_lock(); }
		std::unique_lock<mutex_type> lock;
	};

	HOL_WARN_UNUSED_RESULT
	auto lock_for_reading() { return enable_reading_impl{mtx}; };

	HOL_WARN_UNUSED_RESULT
	auto lock_for_updates() { return enable_updates_impl{mtx}; };

	HOL_WARN_UNUSED_RESULT
	auto try_to_lock_for_reading(std::chrono::microseconds wait)
	{
		if(enable_reading_impl access{mtx, wait})
			return optional_type<enable_reading_impl>(std::move(access));
		return optional_type<enable_reading_impl>();
	}

	HOL_WARN_UNUSED_RESULT
	auto try_to_lock_for_updates(std::chrono::microseconds wait)
	{
		if(enable_updates_impl access{mtx, wait})
			return optional_type<enable_updates_impl>(std::move(access));
		return optional_type<enable_updates_impl>();
	}

	HOL_WARN_UNUSED_RESULT
	auto make_lockable_for_reading() { return enable_reading_impl{mtx, std::defer_lock}; };

	HOL_WARN_UNUSED_RESULT
	auto make_lockable_for_updates() { return enable_updates_impl{mtx, std::defer_lock}; };

private:
	mutex_type mtx;
};

// USAGE
//
//	class Business
//	: public lock_enabled
//	{
//	public:
//
//		int get_value(enable_reading) const { return 0; }
//		void set_value(enable_updates, int v) { (void) v; }
//
//
//	};
//
//
//	auto business = std::make_unique<Business>();
//
//	{
//		auto access = business->lock_for_updates();
//
//		// update x here
//		business->get_value(access);
//		business->set_value(access, 1);
//	}
//
//	{
//		auto access = business->lock_for_reading();
//
//		// examine x here
//		business->get_value(access);
//		//business->set_value(access, 1); // won't compile
//	}
//
//	{
//		auto business2 = std::make_unique<Business>();
//
//		auto access = business->make_lockable_for_reading();
//		auto access2 = business2->make_lockable_for_reading();
//
//		std::lock(access.lock, access2.lock);
//	}

//class thread_pool
//{
//public:
//	thread_pool(unsigned size = std::thread::hardware_concurrency())
//	{
//		while(size--)
//			threads.emplace_back(&thread_pool::process, this);
//	}
//
//	~thread_pool() { stop(); }
//
//	template<typename Func, typename... Params>
//	void add(Func func, Params&&... params)
//	{
//		{
//			std::unique_lock<std::mutex> lock(mtx);
//
//			if(closing_down)
//				throw std::runtime_error("adding job to dead pool");
//
//			jobs.push(std::bind(func, std::forward<Params>(params)...));
//		}
//
//		cv.notify_all();
//	}
//
//	std::size_t size() const { return threads.size(); }
//
//	void stop()
//	{
//		std::unique_lock<std::mutex> lock(mtx);
//		if(!closing_down)
//			actual_stop();
//	}
//
//private:
//	void process()
//	{
//		while(!done)
//		{
//			std::function<void()> func;
//
//			{
//				std::unique_lock<std::mutex> lock(mtx);
//
//				cv.wait(lock, [this]{ return done || !jobs.empty(); });
//
//				if(done)
//					break;
//
//				func = std::move(jobs.front());
//				jobs.pop();
//			}
//
//			cv.notify_all();
//
//			if(func)
//				func();
//		}
//	}
//
//	void actual_stop()
//	{
//		closing_down = true;
//		mtx.unlock();
//
//		for(;;)
//		{
//			std::this_thread::sleep_for(std::chrono::milliseconds(1));
//			std::unique_lock<std::mutex> lock(mtx);
//			if(jobs.empty())
//				break;
//		}
//
//		done = true;
//
//		cv.notify_all();
//
//		for(auto& thread: threads)
//			thread.join();
//	}
//
//	std::mutex mtx;
//	std::condition_variable cv;
//	std::atomic_bool done{false};
//	bool closing_down{false};
//	std::vector<std::thread> threads;
//	std::queue<std::function<void()>> jobs;
//};

class thread_pool
{
public:
	thread_pool() = default;
	thread_pool(thread_pool const&) = delete;
	thread_pool& operator=(thread_pool const&) = delete;

	~thread_pool() { stop(); }

	template<typename Func, typename... Params>
	void add(Func func, Params&&... params)
	{
		if(closing_down||done)
			throw std::runtime_error("adding job to dead pool");

		{
			std::unique_lock<std::mutex> lock(mtx);
//			jobs.push([=]{ func(std::forward<Params>(params)...); });
			jobs.push([func, &params...]{ func(std::forward<Params>(params)...); });
		}

		cv.notify_all();
	}

	std::size_t size() const
	{
		std::unique_lock<std::mutex> lock(mtx);
		return threads.size();
	}

	void start(unsigned size = std::thread::hardware_concurrency())
	{
		{
			std::unique_lock<std::mutex> lock(mtx);
			cv.wait(lock, [this]{ return !closing_down; });
		}

		if(!done)
			throw std::runtime_error("trying to start a running pool, must close first");

		actual_start(size);
	}

	//! Prevent new jobs
	//! Wait for jobs to complete
	//! remove threads
	//! TODO: add timeout?
	void stop()
	{
		bool expected = false;
		if(!closing_down.compare_exchange_strong(expected, true))
			return;

		{
			std::unique_lock<std::mutex> lock(mtx);
			cv.wait(lock, [this]{ return jobs.empty(); });
		}

		done = true;
		cv.notify_all();

		for(auto& thread: threads)
			thread.join();

		{
			std::unique_lock<std::mutex> lock(mtx);
			threads.clear();
		}

		closing_down = false;
		cv.notify_all();
	}

	void wait() // wait for current cueue to empty
	{
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [this]{ return jobs.empty(); });
	}

private:
	void actual_start(unsigned size)
	{
		{
			std::unique_lock<std::mutex> lock(mtx);
			while(size--)
				threads.emplace_back(&thread_pool::process, this);
		}

		done = false;
		cv.notify_all();
	}

	void process()
	{
		std::function<void()> func;

		while(!done)
		{
			{
				std::unique_lock<std::mutex> lock(mtx);
				cv.wait(lock, [this]{ return done || !jobs.empty(); });

				if(done)
					break;

				func = std::move(jobs.front());
				jobs.pop();
			}

			cv.notify_all();

			if(func)
				func();
		}
	}

	mutable std::mutex mtx;
	std::condition_variable cv;

	//! block queue and wait for it to empty
	std::atomic_bool closing_down{false};

	//! signal threads to end
	std::atomic_bool done{true};

	std::vector<std::thread> threads;
	std::queue<std::function<void()>> jobs;
};

} // thread_utils
} // header_only_library

#endif // HEADER_ONLY_LIBRARY_THREAD_UTILS_H

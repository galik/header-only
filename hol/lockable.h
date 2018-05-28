#ifndef HEADER_ONLY_LIBRARY_LOCKABLE_H
#define HEADER_ONLY_LIBRARY_LOCKABLE_H
//
// Copyright (c) 2018 Galik <galik.bool@gmail.com>
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

#include <mutex>
#include <shared_mutex>
#include <utility>

namespace header_only_library {
namespace lockable {

/***
 * Trying to *copy* or *move* a lockable while a read_lockable or write_lockable
 * owns its `mutex` will cause *deadlock*.
 */

template <typename CRTP>
class lockable_base
{
public:
    using mutex_type = std::shared_timed_mutex;

    lockable_base() = default;
    virtual ~lockable_base() = default;

    template <typename... Args>
    lockable_base(Args&&... args) : crtp(std::forward<Args>(args)...)
    {
    }

    template <typename T>
    lockable_base(std::initializer_list<T> il) : crtp(il)
    {
    }

    lockable_base(lockable_base const& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        copy_assign(other);
    }

    lockable_base(lockable_base&& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        move_assign(other);
    }

    lockable_base& operator=(lockable_base const& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        copy_assign(other);
        return *this;
    }

    lockable_base& operator=(lockable_base&& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        move_assign(other);
        return *this;
    }

protected:
    void copy_assign(lockable_base const& other) { this->crtp = other.crtp; }

    void move_assign(lockable_base& other) { this->crtp = std::move(other.crtp); }

    mutable mutex_type mtx;
    CRTP crtp;
};

template <typename CRTP>
class read_lockable : public virtual lockable_base<CRTP>
{
    using mutex_type = typename lockable_base<CRTP>::mutex_type;

public:
    class reading_lock
    {
    public:
        reading_lock() = default;

        reading_lock(CRTP const& crtp, mutex_type& mtx) : crtp(&crtp), lock(mtx) {}

        reading_lock(CRTP const& crtp, mutex_type& mtx, std::defer_lock_t)
            : crtp(&crtp), lock(mtx, std::defer_lock)
        {
        }

        // it should be impossible for a reading_lock to exist without an associated
        // locked mutex
        reading_lock(reading_lock const& other) : crtp(other.crtp), lock(other.lock.mtx) {}

        reading_lock(reading_lock&& other) : crtp(other.crtp), lock(std::move(other.lock)) {}

        reading_lock& operator=(reading_lock const& other)
        {
            lock = std::shared_lock<mutex_type>(other.lock.mtx);
            crtp = other.crtp;
            return *this;
        }

        reading_lock& operator=(reading_lock&& other)
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

    read_lockable() = default;

    template <typename... Args>
    read_lockable(Args&&... args) : lockable_base<CRTP>(std::forward<Args>(args)...)
    {
    }

    read_lockable(read_lockable const& other) : lockable_base<CRTP>(other) {}
    read_lockable(read_lockable&& other) : lockable_base<CRTP>(std::move(other)) {}

    read_lockable& operator=(read_lockable const& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        lockable_base<CRTP>::copy_assign(other);
        copy_assign(other);
        return *this;
    }

    read_lockable& operator=(read_lockable&& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        lockable_base<CRTP>::move_assign(other);
        move_assign(other);
        return *this;
    }

    reading_lock lock_for_reading() const
    {
        return reading_lock(lockable_base<CRTP>::crtp, lockable_base<CRTP>::mtx);
    }

    reading_lock lock_for_deferred_reading() const
    {
        return reading_lock(lockable_base<CRTP>::crtp, lockable_base<CRTP>::mtx, std::defer_lock);
    }

protected:
    void copy_assign(read_lockable const& other) {}
    void move_assign(read_lockable& other) {}
};

template <typename CRTP>
class write_lockable : public virtual lockable_base<CRTP>
{
    using mutex_type = typename lockable_base<CRTP>::mutex_type;

public:
    class writing_lock
    {
    public:
        writing_lock() = default;

        writing_lock(CRTP& crtp, mutex_type& mtx) : crtp(&crtp), lock(mtx) {}

        writing_lock(CRTP& crtp, mutex_type& mtx, std::defer_lock_t)
            : crtp(&crtp), lock(mtx, std::defer_lock)
        {
        }

        writing_lock(writing_lock const&) = delete;
        writing_lock(writing_lock&& other) : crtp(other.crtp), lock(std::move(other.lock)) {}

        writing_lock& operator=(writing_lock const&) = delete;
        writing_lock& operator=(writing_lock&& other)
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

    write_lockable() = default;

    template <typename... Args>
    write_lockable(Args&&... args) : lockable_base<CRTP>(std::forward<Args>(args)...)
    {
    }

    write_lockable(write_lockable const& other) : lockable_base<CRTP>(other) {}
    write_lockable(write_lockable&& other) : lockable_base<CRTP>(std::move(other)) {}

    write_lockable& operator=(write_lockable const& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        lockable_base<CRTP>::copy_assign(other);
        copy_assign(other);
        return *this;
    }

    write_lockable& operator=(write_lockable&& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        lockable_base<CRTP>::move_assign(other);
        move_assign(other);
        return *this;
    }

    writing_lock lock_for_writing()
    {
        return writing_lock(lockable_base<CRTP>::crtp, lockable_base<CRTP>::mtx);
    }

    writing_lock lock_for_deferred_writing()
    {
        return writing_lock(lockable_base<CRTP>::crtp, lockable_base<CRTP>::mtx, std::defer_lock);
    }

protected:
    void copy_assign(write_lockable const& other) {}
    void move_assign(write_lockable& other) {}
};

template <typename CRTP>
class lockable : public read_lockable<CRTP>, public write_lockable<CRTP>
{
    using mutex_type = typename lockable_base<CRTP>::mutex_type;

public:
    template <typename... Args>
    lockable(Args&&... args) : lockable_base<CRTP>(std::forward<Args>(args)...)
    {
    }

    lockable(lockable const& other) : lockable_base<CRTP>(other) {}
    lockable(lockable&& other) : lockable_base<CRTP>(std::move(other)) {}

    lockable& operator=(lockable const& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        lockable_base<CRTP>::copy_assign(other);
        read_lockable<CRTP>::copy_assign(other);
        write_lockable<CRTP>::copy_assign(other);
        copy_assign(other);
        return *this;
    }

    lockable& operator=(lockable&& other)
    {
        std::unique_lock<mutex_type> lock(other.mtx);
        lockable_base<CRTP>::move_assign(other);
        read_lockable<CRTP>::move_assign(other);
        write_lockable<CRTP>::move_assign(other);
        move_assign(other);
        return *this;
    }

protected:
    void copy_assign(lockable const& other) {}
    void move_assign(lockable& other) {}
};

template <typename CRTP>
class lockable_container : public read_lockable<CRTP>, public write_lockable<CRTP>
{
public:
    template <typename... Args>
    lockable_container(Args&&... args) : lockable_base<CRTP>(std::forward<Args>(args)...)
    {
    }

    template <typename T>
    lockable_container(std::initializer_list<T> il) : lockable_base<CRTP>(il)
    {
    }
};

namespace detail {

    template <typename Tuple, std::size_t... Is>
    void apply_multilock(Tuple& tp, std::index_sequence<Is...>)
    {
        std::lock(std::get<Is>(tp).get_lock()...);
    }

    template <typename CRTP>
    struct for_reading_type
    {
        lockable<CRTP> const& l;
        for_reading_type(lockable<CRTP> const& l) : l(l) {}
        auto lock() { return l.lock_for_deferred_reading(); }
    };

    template <typename CRTP>
    struct for_writing_type
    {
        lockable<CRTP>& l;
        for_writing_type(lockable<CRTP>& l) : l(l) {}
        auto lock() { return l.lock_for_deferred_writing(); }
    };

} // namespace detail

template <typename CRTP>
auto for_reading(lockable<CRTP> const& l)
{
    return detail::for_reading_type<CRTP>(l);
}

template <typename CRTP>
auto for_writing(lockable<CRTP>& l)
{
    return detail::for_writing_type<CRTP>(l);
}

template <typename... Lockables>
auto lock(Lockables&&... lockables)
{
    auto tp = std::make_tuple(std::forward<Lockables>(lockables).lock()...);
    detail::apply_multilock(tp, std::make_index_sequence<sizeof...(Lockables)>{});
    return tp;
}

} // namespace lockable
} // namespace header_only_library

#endif // GSL_LOCKABLE_H

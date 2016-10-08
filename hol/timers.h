#ifndef HOL_TIMER_H
#define HOL_TIMER_H
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

#include <chrono>
#include <ostream>
#include <iomanip>

namespace hol {

template<typename TimerImpl>
class Timer
{
public:
	using timer_impl = TimerImpl;
	using value_type = decltype(timer_impl().nsecs());

private:
	timer_impl timer;
	unsigned precision;

public:
	Timer(unsigned precision = 6): precision(precision) {}

	void clear() { timer.clear(); }
	void start() { timer.start(); }
	void stop()  { timer.stop(); }

	friend std::ostream& operator<<(std::ostream& o, const Timer<timer_impl>& timer)
	{
		return o << std::fixed << std::setprecision(timer.precision) << timer.secs();
	}

	auto   nsecs() const { return timer.nsecs(); }
	double usecs() const { return double(timer.nsecs()) / 1000.0; }
	double msecs() const { return double(timer.nsecs()) / 1000000.0; }
	double  secs() const { return double(timer.nsecs()) / 1000000000.0; }
};

class StdTimerImpl
{
	std::chrono::steady_clock::time_point tsb;
	std::chrono::steady_clock::time_point tse;

public:
	void clear() { start(); tse = tsb; }
	void start() { tsb = std::chrono::steady_clock::now(); }
	void stop()  { tse = std::chrono::steady_clock::now(); }

	auto nsecs() const
	{
		using namespace std::chrono;
		return duration_cast<nanoseconds>(tse - tsb).count();
	}
};

using StdTimer = Timer<StdTimerImpl>;

#ifdef __unix__

#include <time.h>

class PosixTimerImpl
{
	timespec tsb;
	timespec tse;

public:
	void clear() { start(); tse = tsb; }
	void start() { clock_gettime(CLOCK_MONOTONIC, &tsb); }
	void stop() { clock_gettime(CLOCK_MONOTONIC, &tse); }

	auto nsecs() const
	{
		auto b = (tsb.tv_sec * 1000000000) + tsb.tv_nsec;
		auto e = (tse.tv_sec * 1000000000) + tse.tv_nsec;
		return e - b;
	}
};

using PosixTimer = Timer<PosixTimerImpl>;

template<decltype(CLOCK_MONOTONIC_RAW) Clock = CLOCK_MONOTONIC_RAW>
class LinuxTimerImpl
{
	timespec tsb;
	timespec tse;

public:
	void clear() { start(); tse = tsb; }
	void start() { clock_gettime(Clock, &tsb); }
	void stop() { clock_gettime(Clock, &tse); }

	auto nsecs() const
	{
		auto b = (tsb.tv_sec * 1000000000) + tsb.tv_nsec;
		auto e = (tse.tv_sec * 1000000000) + tse.tv_nsec;
		return e - b;
	}
};

using LinuxTimer = Timer<LinuxTimerImpl<>>;
using LinuxMonotonicTimer = Timer<LinuxTimerImpl<CLOCK_MONOTONIC_RAW>>;
using LinuxThreadTimer = Timer<LinuxTimerImpl<CLOCK_THREAD_CPUTIME_ID>>;
using LinuxProcessTimer = Timer<LinuxTimerImpl<CLOCK_PROCESS_CPUTIME_ID>>;

#endif

class wait_timer
{
	using clock = std::chrono::steady_clock;

	clock::duration time_to_wait;
	clock::time_point timeout = clock::now();

public:
	wait_timer(std::chrono::milliseconds ms)
	: time_to_wait(ms), timeout(clock::now() + ms) {}

	void wait()
	{
		while(clock::now() < timeout)
			std::this_thread::sleep_until(timeout);
		timeout += time_to_wait; // reset timer
	}

	/**
	 * Adjust the wait time without resetting the
	 * time base.
	 * @param ms New wait time.
	 */
	void wait(std::chrono::milliseconds ms)
	{
		if(time_to_wait != ms)
		{
			auto difference = ms - time_to_wait;
			timeout += difference;
			time_to_wait = ms;
		}
		wait();
	}

	/**
	 * Reset the time-base by setting the timeout
	 * counter to now + ms (without affecting the frame position)
	 * @param ms
	 */
	void reset(std::chrono::milliseconds ms)
	{
		auto now = clock::now();
		auto difference = now - timeout;
		auto frames = difference / ms;
		timeout += (frames * ms);
		time_to_wait = ms;
	}
};

} // hol


#endif // HOL_TIMER_H

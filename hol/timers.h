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

namespace hol {

template<typename TimerImpl>
class Timer
{
	using timer_impl = TimerImpl;

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

	auto nsecs() const
	{
		return timer.nsecs();
	}

	double usecs() const
	{
		return timer.nsecs() / 1000.0;
	}

	double msecs() const
	{
		return timer.nsecs() / 1000000.0;
	}

	double secs() const
	{
		return timer.nsecs() / 1000000000.0;
	}
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

#endif

} // hol


#endif // HOL_TIMER_H

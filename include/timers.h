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

class Timer
{
	using clk = std::chrono::steady_clock;
	using microseconds = std::chrono::microseconds;
	using nanoseconds = std::chrono::nanoseconds;

	clk::time_point tsb;
	clk::time_point tse;

public:

	void clear() { tsb = tse = clk::now(); }
	void start() { tsb = clk::now(); }
	void stop() { tse = clk::now(); }

	friend std::ostream& operator<<(std::ostream& o, const Timer& timer)
	{
		return o << timer.secs();
	}

	auto nsecs() const
	{
		return std::chrono::duration_cast<nanoseconds>(tse - tsb).count();
	}

	double usecs() const
	{
		return nsecs() / 1000.0;
	}

	double msecs() const
	{
		return nsecs() / 1000000.0;
	}

	double secs() const
	{
		return nsecs() / 1000000000.0;
	}

	// return time difference in seconds
//	double secs() const
//	{
//		if(tse <= tsb)
//			return 0.0;
//		auto d = std::chrono::duration_cast<microseconds>(tse - tsb);
//		return d.count() / 1000000.0;
//	}
};

#ifdef __unix__

#include <time.h>

class PosixTimer
{
	struct clock_type {};
	struct precision_type {};
	template<typename T, typename ID>
	struct helper
	{
		ID id;
		T p;
		helper(T p = {}): p(p) {}
		operator T() const { return p; }
	};
public:

	using clock = helper<decltype(CLOCK_MONOTONIC), clock_type>;
	using precision = helper<unsigned, precision_type>;

private:
	decltype(CLOCK_MONOTONIC) clk;
	timespec tsb;
	timespec tse;
	precision p = 6;

public:
	PosixTimer()
	: PosixTimer(clock(CLOCK_MONOTONIC), precision(6)) {}

	explicit PosixTimer(clock clk)
	: PosixTimer(clk, precision(6)) {}

	explicit PosixTimer(precision p)
	: PosixTimer(clock(CLOCK_MONOTONIC), p) {}

	explicit PosixTimer(clock clk, precision p)
	: clk(clk), p(p) {}

	void clear() { start(); tse = tsb; }
	void start() { clock_gettime(clk, &tsb); }
	void stop() { clock_gettime(clk, &tse); }

	friend std::ostream& operator<<(std::ostream& o, const PosixTimer& timer)
	{
		return o << std::fixed << std::setprecision(timer.p) << timer.secs();
	}

	auto nsecs() const
	{
		auto b = (tsb.tv_sec * 1000000000) + tsb.tv_nsec;
		auto e = (tse.tv_sec * 1000000000) + tse.tv_nsec;
		return e - b;
	}

	double usecs() const
	{
		return nsecs() / 1000.0;
	}

	double msecs() const
	{
		return nsecs() / 1000000.0;
	}

	double secs() const
	{
		return nsecs() / 1000000000.0;
	}
};

#endif

} // hol


#endif // HOL_TIMER_H

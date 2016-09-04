#ifndef HOL_SIMPLE_LOGGER_H
#define HOL_SIMPLE_LOGGER_H
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

#include <map>
#include <ctime>
#include <mutex>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <shared_mutex>

//#include "bug.h"

/**
 * Usage:
 *
 * using hol::simple_logger::LOG;      // this to use
 *
 * LOG:E << "Logs an error message #" << 5; // don't add '\n'!
 * LOG:W << "Logs a warning " << warning_message << " for the logger.";
 *
 */

/**
 * Configure
 *
 * 	LOG::E << "test the logger";
 *
 *	log_out::level_name(LOG::E, "ERROR: ");
 *
 *	LOG::E << "test the logger";
 *
 *	std::ofstream ofs("test.txt");
 *
 *	log_out::stream(ofs); // now logs to file
 *
 *	LOG::E << "test the logger";
 *
 */

namespace hol {
namespace simple_logger {

/**
 * Thread-safe cross platform (xp) localtime function
 * @param timer
 * @return
 */
inline std::tm localtime_xp(std::time_t timer)
{
	std::tm bt {};
#if defined(__unix__)
	localtime_r(&timer, &bt);
#elif defined(_MSC_VER)
	localtime_s(&bt, &timer);
#else
	static std::mutex mtx;
	std::lock_guard<std::mutex> lock(mtx);
	bt = *std::localtime(&timer);
#endif
	return bt;
}

enum class LOG: unsigned {D, I, W, E, X, COUNT};

constexpr auto COUNT = static_cast<unsigned>(LOG::COUNT);
constexpr auto DEFAULT_FORMAT = "%F %T| ";

class log_out
{
	friend class Logger;

	struct config_type
	{
		using mutex_type = std::shared_timed_mutex;
		using read_lock = std::shared_lock<mutex_type>;
		using write_lock = std::unique_lock<mutex_type>;

		mutable mutex_type config_mtx;
		mutable std::mutex output_mtx;

		auto lock_for_reading() const { return read_lock(config_mtx); }
		auto lock_for_writing() const { return write_lock(config_mtx); }
		auto lock_for_deferred_reading() const { return read_lock(config_mtx, std::defer_lock); }
		auto lock_for_deferred_writing() const { return write_lock(config_mtx, std::defer_lock); }

		auto lock_for_output() const { return std::unique_lock<std::mutex>(output_mtx); }

		short precision = -1;
		bool boolalpha = true;
		std::ostream* out = &std::cout;
		std::string level_name;
		std::string format = DEFAULT_FORMAT;
		bool enabled = true;
		bool synchronized_output = false;

		config_type() {}

		config_type(const std::string& level_name): level_name(level_name) {}

		config_type(config_type&& cfg)
		: precision(cfg.precision)
		, boolalpha(cfg.boolalpha)
		, out(cfg.out)
		, level_name(std::move(cfg.level_name))
		, format(std::move(cfg.format))
		, enabled(cfg.enabled)
		, synchronized_output(cfg.synchronized_output)
		{
			cfg.out = nullptr;
		}

		config_type(config_type const& cfg)
		: precision(cfg.precision)
		, boolalpha(cfg.boolalpha)
		, out(cfg.out)
		, level_name(cfg.level_name)
		, format(cfg.format)
		, enabled(cfg.enabled)
		, synchronized_output(cfg.synchronized_output)
		{
		}

		config_type& operator=(config_type const& cfg)
		{
			precision = cfg.precision;
			boolalpha = cfg.boolalpha;
			out = cfg.out;
			level_name = cfg.level_name;
			format = cfg.format;
			enabled = cfg.enabled;
			synchronized_output = cfg.synchronized_output;

			return *this;
		}

	};

	static config_type& config(LOG L)
	{
		thread_local static std::map<LOG, config_type> user_cfg;
		thread_local static config_type cfg[COUNT] = {{"D: "}, {"I: "}, {"W: "}, {"E: "}, {"X: "}};
		if(static_cast<unsigned>(L) < COUNT)
			return cfg[static_cast<unsigned>(L)];
		return user_cfg[L];
	}

	static void output(std::ostream* os = nullptr)
	{
		for(auto L = 0U; L < COUNT; ++L)
			output(static_cast<LOG>(L), os);
	}

	static std::ostream* output(LOG L, std::ostream* os = nullptr)
	{
		auto& cfg = config(L);

		if(os)
		{
			cfg.out = os;
			if(cfg.precision != -1)
				os->precision(cfg.precision);
		}

		return L >= level() && cfg.enabled ? cfg.out : nullptr;
	}

	template<typename Level>
	static void set_enable(Level L, bool state)
	{
		config(L).enabled = state;
	}

	static auto lock_for_reading(LOG L)
	{
		return config(L).lock_for_reading();
	}

	static auto lock_for_writing(LOG L)
	{
		return config(L).lock_for_writing();
	}

	static auto lock_all_for_reading()
	{
		auto t = std::make_tuple(
			config(LOG::D).lock_for_deferred_reading()
			, config(LOG::I).lock_for_deferred_reading()
			, config(LOG::W).lock_for_deferred_reading()
			, config(LOG::E).lock_for_deferred_reading()
			, config(LOG::X).lock_for_deferred_reading()
		);
		std::lock(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t), std::get<4>(t));
		return std::move(t);
	}

	static auto lock_all_for_writing()
	{
		auto t = std::make_tuple(
			config(LOG::D).lock_for_deferred_writing()
			, config(LOG::I).lock_for_deferred_writing()
			, config(LOG::W).lock_for_deferred_writing()
			, config(LOG::E).lock_for_deferred_writing()
			, config(LOG::X).lock_for_deferred_writing()
		);
		std::lock(std::get<0>(t), std::get<1>(t), std::get<2>(t), std::get<3>(t), std::get<4>(t));
		return std::move(t);
	}

public:
	static void stream(std::ostream& os)
	{
		for(auto L = 0U; L < COUNT; ++L)
			stream(static_cast<LOG>(L), os);
	}

	static void stream(LOG L, std::ostream& os)
	{
		auto lock = lock_for_writing(L);
		output(L, &os);
	}

	static void precision(short p)
	{
		for(auto L = 0U; L < COUNT; ++L)
			precision(static_cast<LOG>(L), p);
	}

	static void precision(LOG L, short p)
	{
		auto lock = lock_for_writing(L);
		config(L).precision = p;
		config(L).out->precision(p);
	}

	static auto level_name(LOG L)
	{
		auto lock = lock_for_reading(L);
		return config(L).level_name;
	}

	static void level_name(LOG L, std::string name)
	{
		auto lock = lock_for_writing(L);
		config(L).level_name = name;
	}

	static LOG level(LOG L = LOG::COUNT)
	{
		static LOG min_level = LOG::I;

		static std::mutex mtx;
		std::lock_guard<std::mutex> lock(mtx);

		auto level = min_level;

		if(L != LOG::COUNT)
			min_level = L;

		return level;
	}

	static void format_time(const std::string& fmt = DEFAULT_FORMAT)
	{
		for(auto L = 0U; L < COUNT; ++L)
			format_time_for(static_cast<LOG>(L), fmt);
	}

	static void format_time_for(LOG L, const std::string& fmt)
	{
		auto lock = lock_for_writing(L);
		config(L).format = fmt;
	}

	static void display_time_stamp()
	{
		format_time(DEFAULT_FORMAT);
	}

	static void display_time_stamp_for(LOG L)
	{
		format_time_for(L, DEFAULT_FORMAT);
	}

	static void turn_off_time_stamp()
	{
		format_time("");
	}

	static void turn_off_time_stamp_for(LOG L)
	{
		format_time_for(L, "");
	}

	static std::string time_format(LOG L)
	{
		auto lock = lock_for_reading(L);
		return config(L).format;
	}

	template<typename Level>
	static void enable(Level L)
	{
		auto lock = lock_for_writing(L);
		set_enable(L, true);
	}

	template<typename Level, typename... Levels>
	static void enable(Level L, Levels... Ls)
	{
		enable(L);
		enable(Ls...);
	}

	template<typename Level>
	static void disable(Level L)
	{
		auto lock = lock_for_writing(L);
		set_enable(L, false);
	}

	template<typename Level, typename... Levels>
	static void disable(Level L, Levels... Ls)
	{
		disable(L);
		disable(Ls...);
	}

	static void sunchronize_output()
	{
		for(auto L = 0U; L < COUNT; ++L)
			sunchronize_output_for(static_cast<LOG>(L));
	}

	static void sunchronize_output_for(LOG L)
	{
		auto lock = lock_for_writing(L);
		config(L).synchronized_output = true;
	}

	static void unsunchronize_output()
	{
		for(auto L = 0U; L < COUNT; ++L)
			unsunchronize_output_for(static_cast<LOG>(L));
	}

	static void unsunchronize_output_for(LOG L)
	{
		auto lock = lock_for_writing(L);
		config(L).synchronized_output = false;
	}

	static LOG create_log(std::string const& name = "")
	{
		thread_local static unsigned id = COUNT;
		auto L = static_cast<LOG>(id++);
		stream(L, std::cout);
		level_name(L, name);
		enable(L);
		return L;
	}
};

class Logger
{
	LOG L;
	std::ostringstream ss;
	log_out::config_type cfg;
	decltype(log_out::lock_for_reading(LOG::COUNT)) lock;

	// default = "YYYY-MM-DD HH:MM:SS"
	std::string stamp()
	{
		auto bt = localtime_xp(std::time(0));
		char buf[64];
		return {buf, std::strftime(buf, sizeof(buf), cfg.format.c_str(), &bt)};
	}

public:
	Logger(Logger&& logger)
	: L(logger.L), ss(std::move(logger.ss)), cfg(std::move(logger.cfg))
	{
	}

	template<typename T>
	Logger(LOG level, const T& v)
	: L(level)
	{
		{
			auto lock = log_out::lock_for_reading(L);
			cfg = log_out::config(L);
		}

//		using std::operator<<;
//		operator<<(dynamic_cast<std::ostream&>(ss), v);
		ss << v;
	}

	~Logger()
	{
		if(!cfg.out || !cfg.enabled)
			return;

		decltype(log_out::config(L).lock_for_output()) lock;

		if(cfg.synchronized_output)
			lock = log_out::config(L).lock_for_output();

		if(cfg.boolalpha)
			(*cfg.out) << std::boolalpha;

		(*cfg.out) << stamp() << cfg.level_name << ss.str() << '\n';
	}

	template<typename T>
	Logger& operator<<(const T& v)
	{
		ss << v;
		return *this;
	}
};

template<typename T>
Logger operator<<(const LOG& level, const T& v)
{
	return Logger(level, v);
}

//template<typename T>
//::hol::simple_logger::Logger operator<<(const ::hol::simple_logger::LOG& level, const T& v)
//{
//	return ::hol::simple_logger::Logger(level, v);
//}

} // simple_logger
} // hol

//namespace {
//template<typename T>
//hol::simple_logger::Logger operator<<(const hol::simple_logger::LOG& level, const T& v)
//{
//	return hol::simple_logger::Logger(level, v);
//}
//} // <anon>

#endif // HOL_SIMPLE_LOGGER_H

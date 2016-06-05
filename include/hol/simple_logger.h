#ifndef HOL_SIMPLE_LOGGER_H
#define HOL_SIMPLE_LOGGER_H
/*
 *  Created on: May 22, 2016
 *      Author: galik
 *     Version: 1.0.0
 */

#include <ctime>
#include <mutex>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>

/**
 * Usage:
 *
 * using namespace hol::simple_logger;
 *
 * LOG:E << "Logs an error message #" << 5; // don't add '\n'!
 * LOG:W << "Logs a warning " << warning_message << " for the logger.";
 *
 */

namespace hol { namespace simple_logger {

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
		short precision = -1;
		bool boolalpha = true;
		std::ostream* out = &std::cout;
		std::string level_name;
		std::string format = DEFAULT_FORMAT;
		bool enabled = true;
	};

	static config_type& config(LOG L)
	{
		thread_local config_type cfg[COUNT] =
		{
			  {-1, 1, &std::cout, "D: ", DEFAULT_FORMAT, true}
			, {-1, 1, &std::cout, "I: ", DEFAULT_FORMAT, true}
			, {-1, 1, &std::cout, "W: ", DEFAULT_FORMAT, true}
			, {-1, 1, &std::cout, "E: ", DEFAULT_FORMAT, true}
			, {-1, 1, &std::cout, "X: ", DEFAULT_FORMAT, true}
		};
		return cfg[static_cast<unsigned>(L)];
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

public:
	static void stream(std::ostream& os)
	{
		for(auto L = 0U; L < COUNT; ++L)
			output(static_cast<LOG>(L), &os);
	}

	static void stream(LOG L, std::ostream& os)
	{
		output(L, &os);
	}

	static void precision(short precision)
	{
		for(auto L = 0U; L < COUNT; ++L)
			log_out::precision(static_cast<LOG>(L), precision);
	}

	static void precision(LOG L, short precision)
	{
		config(L).precision = precision;
		config(L).out->precision(precision);
	}

	static auto level_name(LOG L)
	{
		return config(L).level_name;
	}

	static void level_name(LOG L, std::string name)
	{
		config(L).level_name = name;
	}

	static LOG level(LOG L = LOG::COUNT)
	{
		static LOG min_level = LOG::I;

		auto level = min_level;

		if(L != LOG::COUNT)
			min_level = L;

		return level;
	}

	static void format(const std::string& fmt = DEFAULT_FORMAT)
	{
		for(auto L = 0U; L < COUNT; ++L)
			log_out::format(static_cast<LOG>(L), fmt);
	}

	static void format(LOG L, const std::string& fmt)
	{
		config(L).format = fmt;
	}

	static std::string format(LOG L)
	{
		return config(L).format;
	}

	template<typename Level>
	static void enable(Level L)
	{
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
		set_enable(L, false);
	}

	template<typename Level, typename... Levels>
	static void disable(Level L, Levels... Ls)
	{
		disable(L);
		disable(Ls...);
	}
};

class Logger
{
	LOG L;
	std::stringstream ss;
	std::ostream* out = nullptr;
	const std::string fmt;

	// default = "YYYY-MM-DD HH:MM:SS"
	std::string stamp()
	{
		auto bt = localtime_xp(std::time(0));
		char buf[64];
		return {buf, std::strftime(buf, sizeof(buf), fmt.c_str(), &bt)};
	}

	std::string level_name() const { return log_out::level_name(L); }

public:
	Logger(Logger&& logger)
	: L(logger.L), ss(std::move(logger.ss)), out(logger.out), fmt(logger.fmt)
	{
		logger.out = nullptr;
	}

	template<typename T>
	Logger(LOG level, const T& v)
	: L(level), out(log_out::output(level)), fmt(log_out::format(L))
	{
		ss << v;
	}

	~Logger()
	{
		if(!out)
			return;

		if(log_out::config(L).boolalpha)
			(*out) << std::boolalpha;

		(*out) << stamp() << level_name() << ss.str() << '\n';
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

}} // hol::simple_logger

#endif // HOL_SIMPLE_LOGGER_H

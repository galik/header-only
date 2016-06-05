#ifndef HOL_SIMPLE_LOGGER_H
#define HOL_SIMPLE_LOGGER_H
/*
 *  Created on: May 22, 2016
 *      Author: galik
 */

#include <iostream>
#include <iomanip>

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

enum class LOG: unsigned {I, W, E, X, COUNT};

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
		std::string level;
		std::string format = DEFAULT_FORMAT;
	};

	static config_type& config(LOG L)
	{
		thread_local config_type cfg[COUNT] =
		{
			{-1,1,&std::cout,"I: "}
		,	{-1,1,&std::cout,"W: "}
		,	{-1,1,&std::cout,"E: "}
		, 	{-1,1,&std::cout,"X: "}
		};
		return cfg[static_cast<unsigned>(L)];
	}

//	static std::ostream* output(std::ostream* os = nullptr)
	static void output(std::ostream* os = nullptr)
	{
		output(LOG::I, os);
		output(LOG::W, os);
		output(LOG::E, os);
		output(LOG::X, os);
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

		return cfg.out;
	}

public:
//	static void stream(std::ostream& os)
//	{
//		output(&os);
//	}

	static void stream(std::ostream& os)
	{
		output(LOG::I, &os);
		output(LOG::W, &os);
		output(LOG::E, &os);
		output(LOG::X, &os);
	}

	static void stream(LOG L, std::ostream& os)
	{
		output(L, &os);
	}

	static void precision(short precision)
	{
		log_out::precision(LOG::I, precision);
		log_out::precision(LOG::W, precision);
		log_out::precision(LOG::E, precision);
		log_out::precision(LOG::X, precision);
	}

	static void precision(LOG L, short precision)
	{
		config(L).precision = precision;
		config(L).out->precision(precision);
	}

	static auto level(LOG L)
	{
		return config(L).level;
	}

	static void level(LOG L, std::string level)
	{
		config(L).level = level;
	}

	static void format(const std::string& fmt = DEFAULT_FORMAT)
	{
		log_out::format(LOG::I, fmt);
		log_out::format(LOG::W, fmt);
		log_out::format(LOG::E, fmt);
		log_out::format(LOG::X, fmt);
	}

	static void format(LOG L, const std::string& fmt)
	{
		config(L).format = fmt;
	}

	static std::string format(LOG L)
	{
		return config(L).format;
	}
};

class Logger
{
	LOG ll;
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

	std::string level() const { return log_out::level(ll); }

public:
	Logger(Logger&& logger)
	: ll(logger.ll), ss(std::move(logger.ss)), out(logger.out), fmt(logger.fmt)
	{
		logger.out = nullptr;
	}

	template<typename T>
	Logger(LOG level, const T& v): ll(level), out(log_out::output(level)), fmt(log_out::format(ll)) { ss << v; }
	~Logger()
	{
		if(out)
		{
			if(log_out::config(ll).boolalpha)
				(*out) << std::boolalpha;
			(*out) << stamp() << level() << ss.str() << '\n';
		}
	}

	template<typename T>
	Logger& print(const T& v) { ss << v; return *this; }
};

template<typename T>
Logger operator<<(Logger&& logger, const T& v)
{
	return std::move(logger.print(v));
}

template<typename T>
Logger operator<<(const LOG& level, const T& v)
{
	return Logger(level, v);
}

}} // hol::simple_logger

#endif // HOL_SIMPLE_LOGGER_H

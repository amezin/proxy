/*
 * log.hpp
 *
 *  Created on: 18.12.2011
 *      Author: sanya-m
 */

#ifndef LOG_HPP_
#define LOG_HPP_

#include <cassert>
#include <cstdio>
#include <ostream>
#include <set>
#include <string>

#include "fixed_stringbuf.hpp"

namespace myproxy {

class logger {

public:

	static const std::streamsize max_message_size = 1024;
	typedef fixed_stringbuf<max_message_size, char> message_buffer;

	enum severity_level {
		severity_trace,
		severity_debug,
		severity_info,
		severity_warning,
		severity_error,
		severity_level_count
	};

private:

	std::set<FILE *> sinks;
	message_buffer buffer;

	void broadcast();

	severity_level min_level;

public:

	explicit logger(severity_level l = severity_info);

	void severity(severity_level l) {
		min_level = l;
	}

	severity_level severity() {
		return min_level;
	}

	void attach(FILE *sink);
	void detach(FILE *sink);

	class stream: public std::ostream {
	public:
		stream(message_buffer &sb);
		virtual ~stream();
	};

	class message {
		stream ls;
		bool enable;
		logger *target;
	public:
		message(bool enabled, logger &parent) :
				ls(parent.buffer), enable(enabled), target(&parent) {

		}

		message(const message &r) :
				ls(r.target->buffer), enable(r.enable), target(r.target) {

		}

		template<typename T>
		message &operator <<(const T &r) {
			if (enable)
				ls << r;
			return *this;
		}

		message &operator <<(std::ostream &(*manip)(std::ostream &)) {
			if (enable)
				ls << manip;
			return *this;
		}

		message &operator <<(std::ios &(*manip)(std::ios &)) {
			if (enable)
				ls << manip;
			return *this;
		}

		message &operator <<(std::ios_base &(*manip)(std::ios_base &)) {
			if (enable)
				ls << manip;
			return *this;
		}

		~message() {
			if (enable)
				target->broadcast();
		}

	};

	message level(severity_level l) {
		return message(l >= min_level, *this);
	}

	message trace() {
		return level(severity_trace);
	}

	message debug() {
		return level(severity_debug);
	}

	message info() {
		return level(severity_info);
	}

	message warning() {
		return level(severity_warning);
	}

	message error() {
		return level(severity_error);
	}

	class scope_trace {
		logger *target;
		const char *scope;
	public:
		scope_trace(logger &l, const char *func) :
				target(&l), scope(func) {
			target->trace() << "Entering " << scope;
		}

		~scope_trace() {
			target->trace() << "Leaving " << scope;
		}
	};
};

extern logger log;

}

#define MY_LOG_LEVEL(level) (::myproxy::log.level() << __FILE__ << ":" << __LINE__ << "[" << __PRETTY_FUNCTION__ << "]: " #level ": ")
#define MY_LOG_TRACE MY_LOG_LEVEL(trace)
#define MY_LOG_DEBUG MY_LOG_LEVEL(debug)
#define MY_LOG_INFO MY_LOG_LEVEL(info)
#define MY_LOG_WARN MY_LOG_LEVEL(warning)
#define MY_LOG_ERROR MY_LOG_LEVEL(error)

#define MY_TRACE_KV(x) #x << (x)
#define MY_TRACE_VAL(x) (MY_LOG_TRACE << MY_TRACE_KV(x))
#define MY_TRACE_SCOPE ::myproxy::logger::scope_trace tracer(::myproxy::log, __PRETTY_FUNCTION__)

#endif /* LOG_HPP_ */

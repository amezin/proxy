/*
 * log.cpp
 *
 *  Created on: 18.12.2011
 *      Author: sanya-m
 */

#include <cassert>
#include <stdexcept>
#include <cstring>

#include "log.hpp"

namespace myproxy {

logger log;

namespace {
	const char * const severity_name[] = {
			"TRACE",
			"DEBUG",
			"INFO",
			"WARN",
			"ERROR"
	};
}

const char *logger::severity_to_string(severity_level l) {
	if (l < 0 || l >= severity_level_count) {
		return 0;
	}
	return severity_name[l];
}

logger::severity_level logger::severity_from_string(const char *s) {
	for (int i = 0; i < severity_level_count; i++) {
		if (!strcmp(severity_name[i], s)) {
			return (severity_level)i;
		}
	}
	throw std::runtime_error(std::string("Invalid log severity: ") + s);
}

void logger::broadcast(severity_level level, const char *msg) {
	assert(msg);

	if (!*msg)
		return;

	for (std::set<FILE *>::iterator i = sinks.begin(); i != sinks.end(); i++) {
		std::fprintf(*i, "%s\t%s\n", severity_to_string(level), msg);
	}
}

logger::logger(severity_level l) :
		min_level(l) {
	attach(stderr);
}

void logger::attach(FILE *sink) {
	sinks.insert(sink);
}

void logger::detach(FILE *sink) {
	sinks.erase(sink);
}

logger::stream::stream(message_buffer &sb) :
		std::ostream(&sb) {

}

logger::stream::~stream() {

}

}

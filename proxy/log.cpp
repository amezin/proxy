/*
 * log.cpp
 *
 *  Created on: 18.12.2011
 *      Author: sanya-m
 */

#include "log.hpp"

namespace myproxy {

logger log;

void logger::broadcast() {
	if (buffer.empty())
		return;

	for (std::set<FILE *>::iterator i = sinks.begin(); i != sinks.end(); i++) {
		std::fprintf(*i, "%s\n", buffer.get());
	}

	buffer.reset();
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

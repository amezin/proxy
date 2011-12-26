/*
 * main.cpp
 *
 *  Created on: 18.12.2011
 *      Author: sanya-m
 */

#include <cstdlib>
#include <climits>
#include <csignal>

#include <exception>
#include <iostream>

#include "log.hpp"
#include "socket.hpp"
#include "errno_exception.hpp"
#include "event_queue.hpp"
#include "client_connect_handler.hpp"

namespace myproxy {

namespace {

int main(unsigned short port) {
	try {
		std::signal(SIGPIPE, SIG_IGN);

		intrusive_ptr<socket> server(new socket());
		server->listen(port);

		event_queue q;
		q.push(new client_connect_handler(server, q), POLLIN);
		while (q.poll()) {
			for (;;) {
				try {
					if (!q.pop()) break;
				} catch (std::exception &e) {
					log.error() << "Error occurred: " << e.what();
				}
			}
		}

	} catch (std::exception &e) {
		log.error() << "Critical error: " << e.what();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

}

}

static unsigned long parse_int_arg(const char *arg, unsigned long max_v) {
	char *eptr;
	unsigned long v = strtoul(arg, &eptr, 10);
	if (v > max_v) {
		myproxy::log.error() << arg
				<< ": number is too large. It must not be greater than "
				<< max_v;
		std::exit(EXIT_FAILURE);
	}
	if (*eptr) {
		myproxy::log.error() << arg << ": isn't a number";
		std::exit(EXIT_FAILURE);
	}
	return v;
}

int main(int argc, char *argv[]) {
	enum {
		ARG_APPNAME, ARG_PORT, ARG_LOG_VERBOSITY, ARG_COUNT
	};

	if (argc <= ARG_PORT || argc > ARG_COUNT) {
		myproxy::log.error() << "Usage: " << argv[ARG_APPNAME]
				<< " port [log_verbosity]";
		return EXIT_FAILURE;
	}

	if (argc - 1 >= ARG_LOG_VERBOSITY) {
		myproxy::log.severity(
				static_cast<myproxy::logger::severity_level>(parse_int_arg(
						argv[ARG_LOG_VERBOSITY],
						myproxy::logger::severity_level_count)));
	}

	return myproxy::main(
			static_cast<unsigned short>(parse_int_arg(argv[ARG_PORT], USHRT_MAX)));
}

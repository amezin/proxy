/*
 * client_headers_handler.hpp
 *
 *  Created on: 19.12.2011
 *      Author: sanya-m
 */

#ifndef CLIENT_HEADERS_HANDLER_HPP_
#define CLIENT_HEADERS_HANDLER_HPP_

#include <cerrno>

#include "log.hpp"
#include "errno_exception.hpp"
#include "event_queue.hpp"

namespace myproxy {

class client_headers_handler : public connection_handler {
public:

	client_headers_handler(const intrusive_ptr<socket> &s, event_queue &q) : connection_handler(s, q) {

	}

	virtual int handle(int pollflags) {
		log.debug() << "Client headers handler invoked";

		char buf[256] = {0};
		ssize_t n = sock().read(buf, sizeof(buf) - 1);

		if (n == 0) {
			return 0;
		}

		log.trace() << "Got data: " << buf;
		return POLLIN;
	}

	virtual ~client_headers_handler() {

	}
};

}

#endif /* CLIENT_HEADERS_HANDLER_HPP_ */

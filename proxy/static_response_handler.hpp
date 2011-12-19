/*
 * static_response_handler.hpp
 *
 *  Created on: 20.12.2011
 *      Author: sanya-m
 */

#ifndef STATIC_RESPONSE_HANDLER_HPP_
#define STATIC_RESPONSE_HANDLER_HPP_

#include "log.hpp"
#include "event_queue.hpp"

namespace myproxy {

class static_response_handler: public connection_handler {

	const std::string &response;
	std::string::size_type off;

public:

	static_response_handler(const std::string &res,
			const intrusive_ptr<socket> &s, event_queue &q) :
			connection_handler(s, q), response(res), off(0) {

	}

	virtual int handle(int pollflags) {
		log.debug() << "Static response handler invoked";

		off += sock().write(response.c_str() + off, response.size() - off);
		if (response.size() == off) {
			log.trace() << "Response is sent completely";
			return 0;
		}
		return POLLOUT;
	}

	virtual ~static_response_handler() {

	}
};
}

#endif /* STATIC_RESPONSE_HANDLER_HPP_ */

/*
 * client_connect_handler.hpp
 *
 *  Created on: 19.12.2011
 *      Author: sanya-m
 */

#ifndef CLIENT_CONNECT_HANDLER_HPP_
#define CLIENT_CONNECT_HANDLER_HPP_

#include "log.hpp"
#include "errno_exception.hpp"
#include "event_queue.hpp"
#include "static_response_handler.hpp"
#include "error_pages.hpp"

namespace myproxy {

class client_connect_handler : public connection_handler {
public:

	client_connect_handler(const intrusive_ptr<socket> &s, event_queue &q) : connection_handler(s, q) {

	}

	virtual int handle(int pollflags) {
		log.debug() << "Client connection handler invoked";
		intrusive_ptr<socket> s = sock().accept();
		queue().push(new static_response_handler(error_pages::not_implemented, s, queue()), POLLOUT);
		return POLLIN;
	}

	virtual ~client_connect_handler() {

	}
};

}

#endif /* CLIENT_CONNECT_HANDLER_HPP_ */

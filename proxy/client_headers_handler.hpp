/*
 * client_headers_handler.hpp
 *
 *  Created on: 20.12.2011
 *      Author: sanya-m
 */

#ifndef CLIENT_HEADERS_HANDLER_HPP_
#define CLIENT_HEADERS_HANDLER_HPP_

#include "client_line_handler.hpp"

namespace myproxy {

class client_headers_handler: public client_line_handler {

	std::string host, port;
	std::string method, url, version;

	intrusive_ptr<buffer> request;

	void handle_first_line(const std::string &ln);

protected:

	virtual bool handle_line(const std::string &ln);

public:

	client_headers_handler(const intrusive_ptr<socket> s, event_queue &q) :
			client_line_handler(s, q), request(new buffer()) {

	}

	virtual ~client_headers_handler() {

	}

};

}

#endif /* CLIENT_HEADERS_HANDLER_HPP_ */

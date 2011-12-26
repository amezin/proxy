/*
 * client_line_handler.hpp
 *
 *  Created on: 20.12.2011
 *      Author: sanya-m
 */

#ifndef CLIENT_LINE_HANDLER_HPP_
#define CLIENT_LINE_HANDLER_HPP_

#include "line_handler.hpp"

namespace myproxy {

class client_line_handler: public line_handler {

protected:

	virtual bool handle_line(const std::string &ln) = 0;

public:

	class request_parse_error: public std::runtime_error {
	public:
		request_parse_error(const std::string &arg) :
				std::runtime_error(arg) {

		}
	};

	class not_implemented_error: public std::runtime_error {
	public:
		not_implemented_error(const std::string &arg) :
				std::runtime_error(arg) {

		}
	};

	class http_version_not_supported_error: public std::runtime_error {
	public:
		http_version_not_supported_error(const std::string &arg) :
				std::runtime_error(arg) {

		}
	};

	client_line_handler(const intrusive_ptr<socket> s, event_queue &q) :
			line_handler(s, q) {

	}

	virtual ~client_line_handler() {

	}

	virtual int handle(int pollf);

};

}

#endif /* CLIENT_LINE_HANDLER_HPP_ */

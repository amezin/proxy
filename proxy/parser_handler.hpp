/*
 * parser_handler.hpp
 *
 *  Created on: 20.12.2011
 *      Author: sanya-m
 */

#ifndef PARSER_HANDLER_HPP_
#define PARSER_HANDLER_HPP_

#include "buffer_handler.hpp"

namespace myproxy {

class parser_handler: public buffer_handler {
	std::size_t cur_off;

protected:

	virtual bool handle_char(char c) = 0;

public:

	parser_handler(const intrusive_ptr<socket> s, event_queue &q) :
			buffer_handler(s, q), cur_off(0) {

	}

	std::size_t offset() const {
		return cur_off;
	}

	virtual int handle(int pollf) {
		int r = buffer_handler::handle(pollf);
		while (cur_off < this->buf()->size()) {
			if (!handle_char(this->buf()->ptr()[cur_off++]))
				return 0;
		}
		return r;
	}

	virtual ~parser_handler() {

	}

};

}

#endif /* PARSER_HANDLER_HPP_ */

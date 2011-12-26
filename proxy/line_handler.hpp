/*
 * line_handler.hpp
 *
 *  Created on: 20.12.2011
 *      Author: sanya-m
 */

#ifndef LINE_HANDLER_HPP_
#define LINE_HANDLER_HPP_

#include <stdexcept>

#include "parser_handler.hpp"

namespace myproxy {

class line_handler: public parser_handler {
public:

private:

	std::string line;

protected:

	virtual bool handle_char(char c) {
		if (c == '\n') {
			if (line.length() && line[line.length() - 1] == '\r') line.erase(line.length() - 1, 1);
			bool r = handle_line(line);
			if (r) line.clear();
			return r;
		}

		line.push_back(c);
		return true;
	}

	virtual bool handle_line(const std::string &ln) = 0;

public:

	line_handler(const intrusive_ptr<socket> s, event_queue &q) :
			parser_handler(s, q) {

	}

	virtual ~line_handler() {

	}

};

}

#endif /* LINE_HANDLER_HPP_ */

/*
 * buffer_handler.hpp
 *
 *  Created on: 20.12.2011
 *      Author: sanya-m
 */

#ifndef BUFFER_HANDLER_HPP_
#define BUFFER_HANDLER_HPP_

#include "log.hpp"
#include "event_queue.hpp"
#include "buffer.hpp"

namespace myproxy {

class buffer_handler: public connection_handler {
	intrusive_ptr<buffer> _buf;

public:
	buffer_handler(const intrusive_ptr<socket> &s, event_queue &q) :
			connection_handler(s, q), _buf(new buffer()) {

	}

	virtual int handle(int pollf) {
		log.debug() << "Invoked buffer_handler";
		if (_buf->size() == _buf->capacity()) {
			_buf->grow();
		}
		ssize_t resize = sock()->read(_buf->ptr(), _buf->capacity() - _buf->size());
		_buf->size(_buf->size() + resize);
		return (resize > 0) ? POLLIN : 0;
	}

	const intrusive_ptr<buffer> &buf() const {
		return _buf;
	}

	virtual ~buffer_handler() {

	}
};

}

#endif /* BUFFER_HANDLER_HPP_ */

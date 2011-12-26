/*
 * pipe.hpp
 *
 *  Created on: 26.12.2011
 *      Author: sanya-m
 */

#ifndef PIPE_HPP_
#define PIPE_HPP_

#include <assert.h>

#include "event_queue.hpp"

namespace myproxy {

class pipe: public connection_handler {
	intrusive_ptr<pipe> other_side;
	intrusive_ptr<buffer> buf;
	size_t off;

public:

	pipe(const intrusive_ptr<socket> &sock, event_queue &q,
			const intrusive_ptr<buffer> &buffer = intrusive_ptr<buffer>(),
			const intrusive_ptr<pipe> &other = intrusive_ptr<pipe>()) :
			connection_handler(sock, q), other_side(other), buf(buffer), off(0) {

	}

	virtual ~pipe() {

	}

	int handle_in() {
		assert(buf);
		assert(other_side);

		ssize_t resize = sock()->read(buf->ptr(), buf->capacity());
		buf->size(resize);
		if (resize == 0) {
			return 0;
		}

		queue().push(other_side, POLLOUT);

		other_side->other_side = intrusive_ptr<pipe>::from_existing(this);
		other_side = 0;

		return 0;
	}

	int handle_out() {
		assert(other_side);
		assert(other_side->buf);
		assert(!buf);

		if (off < other_side->buf->size()) {
			off += sock()->write(other_side->buf->ptr() + off, other_side->buf->size() - off);

			return POLLOUT;
		} else {
			other_side->buf->size(0);
			off = 0;

			queue().push(other_side, POLLIN);

			other_side->other_side = intrusive_ptr<pipe>::from_existing(this);
			other_side = 0;

			return 0;
		}
	}

	virtual int handle(int pollf) {
		if (pollf & POLLIN) return handle_in();
		if (pollf & POLLOUT) return handle_out();
		return 0;
	}
};

}

#endif /* PIPE_HPP_ */

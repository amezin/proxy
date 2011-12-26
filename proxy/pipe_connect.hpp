/*
 * pipe_connect.hpp
 *
 *  Created on: 26.12.2011
 *      Author: sanya-m
 */

#ifndef PIPE_CONNECT_HPP_
#define PIPE_CONNECT_HPP_

#include <errno.h>

#include "event_queue.hpp"
#include "log.hpp"
#include "static_response_handler.hpp"
#include "errno_exception.hpp"
#include "error_pages.hpp"
#include "pipe.hpp"

namespace myproxy {

class pipe_connect: public connection_handler {
	intrusive_ptr<buffer> request_buf;
	intrusive_ptr<socket> client_sock;

	std::size_t off;

public:
	pipe_connect(const intrusive_ptr<socket> s, event_queue &q,
			const intrusive_ptr<buffer> &request,
			const intrusive_ptr<socket> &client) :
			connection_handler(s, q), request_buf(request), client_sock(client), off(
					0) {

	}

	virtual ~pipe_connect() {

	}

	virtual int handle(int pollf) {
		log.trace() << "Sending request to server";

		const std::string *errordesc = 0;
		try {
			off += sock()->write(request_buf->ptr() + off,
					request_buf->size() - off);
			if (request_buf->size() == off) {
				log.trace() << "Request is sent completely";

				intrusive_ptr<pipe> writer(new pipe(client_sock, queue()));
				intrusive_ptr<pipe> reader(new pipe(sock(), queue(), new buffer(), writer));
				queue().push(reader, POLLIN);

				return 0;
			}
			return POLLOUT;
		} catch (errno_exception &e) {
			switch (e.errnum()) {
			case ENOMEM:
			case ENOBUFS:
				errordesc = &error_pages::service_unavailable;
				break;
			case ECONNREFUSED:
			case ENOTCONN:
			case ENOTSOCK:
				errordesc = &error_pages::bad_gateway;
				break;
			default:
				errordesc = &error_pages::internal_error;
				break;
			}
		} catch (std::bad_alloc &e) {
			errordesc = &error_pages::service_unavailable;
		} catch (std::length_error &e) {
			errordesc = &error_pages::bad_request;
		} catch (...) {
			errordesc = &error_pages::internal_error;
		}
		if (errordesc) {
			queue().push(
					new static_response_handler(*errordesc, client_sock,
							queue()), POLLOUT);
		}
		return 0;
	}
};

}

#endif /* PIPE_CONNECT_HPP_ */

/*
 * client_line_handler.cpp
 *
 *  Created on: 20.12.2011
 *      Author: sanya-m
 */

#include <cerrno>

#include "client_line_handler.hpp"
#include "static_response_handler.hpp"
#include "error_pages.hpp"
#include "errno_exception.hpp"

namespace myproxy {

int client_line_handler::handle(int pollf) {
	const std::string *errordesc = 0;
	try {
		return line_handler::handle(pollf);
	} catch (errno_exception &e) {
		switch (e.errnum()) {
		case ENOMEM:
		case ENOBUFS:
			errordesc = &error_pages::service_unavailable;
			break;
		case ECONNREFUSED:
		case ENOTCONN:
		case ENOTSOCK:
			break;
		default:
			errordesc = &error_pages::internal_error;
			break;
		}
	} catch (std::bad_alloc &e) {
		errordesc = &error_pages::service_unavailable;
	} catch (std::length_error &e) {
		errordesc = &error_pages::bad_request;
	} catch (request_parse_error &e) {
		errordesc = &error_pages::bad_request;
	} catch (not_implemented_error &e) {
		errordesc = &error_pages::not_implemented;
	} catch (http_version_not_supported_error &e) {
		errordesc = &error_pages::version_not_supported;
	} catch (dns_error &) {
		errordesc = &error_pages::not_found;
	} catch (...) {
		errordesc = &error_pages::internal_error;
	}
	if (errordesc) {
		queue().push(new static_response_handler(*errordesc, sock(), queue()), POLLOUT);
	}
	return 0;
}


}



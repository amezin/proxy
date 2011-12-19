/*
 * errno_exception.cpp
 *
 *  Created on: 19.12.2011
 *      Author: sanya-m
 */

#include <cerrno>
#include <cstring>

#include "errno_exception.hpp"

namespace myproxy {

std::string errno_exception::message(int no) {
	int perrno = errno;
	char message[max_message_length];
	if (strerror_r(no, message, max_message_length)) {
		errno = perrno;
		return std::string("Can't get error message");
	}
	return std::string(message);
}

int errno_exception::check_return_code(bool check_errno, int r, const char *src, const char *file, int line, const char *func) {
	if (r >= 0) return r;
	if (check_errno) r = errno;

	myproxy::log.warning() << file << ":" << line << "[" << func << "] throws errno_exception with code " << r << ", message: " <<  message(r) << " source: " << src;
	throw errno_exception(r);
}


}  // namespace myproxy

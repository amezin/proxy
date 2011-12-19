/*
 * errno_exception.hpp
 *
 *  Created on: 18.12.2011
 *      Author: sanya-m
 */

#ifndef ERRNO_EXCEPTION_HPP_
#define ERRNO_EXCEPTION_HPP_

#include <exception>
#include <stdexcept>

#include "log.hpp"

namespace myproxy {

class errno_exception: public std::runtime_error {
	int code;

public:

	static const size_t max_message_length = 1024;

	errno_exception(int no) :
			std::runtime_error(message(no)), code(no) {
		log.debug() << "Constructed errno_exception: " << this->what();
	}

	int errnum() const {
		return code;
	}

	static std::string message(int no);
	static int check_return_code(bool check_errno, int r, const char *src,
			const char *file, int line, const char *func);

};

#define MY_CHECK_RETURN(x) ::myproxy::errno_exception::check_return_code(false, (x), #x, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define MY_CHECK_ERRNO(x) ::myproxy::errno_exception::check_return_code(true, (x), #x, __FILE__, __LINE__, __PRETTY_FUNCTION__)

}

#endif /* ERRNO_EXCEPTION_HPP_ */

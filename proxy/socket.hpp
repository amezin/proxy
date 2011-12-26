/*
 * socket.hpp
 *
 *  Created on: 19.12.2011
 *      Author: sanya-m
 */

#ifndef SOCKET_HPP_
#define SOCKET_HPP_

#include <cstdio>
#include <stdexcept>

#include "ref_count.hpp"

namespace myproxy {

class dns_error : public std::runtime_error {
public:
	dns_error(const std::string &arg) : std::runtime_error(arg) {

	}
};

class socket : public ref_counted {
	int fd;

	socket(int);

	void make_nonblocking();
	void check_so_error();

public:

	socket();

	int so_error();

	void connect(const char *host, const char *port);
	void listen(unsigned short port);
	intrusive_ptr<socket> accept();

	ssize_t read(void *buf, ssize_t count);
	ssize_t write(const void *buf, ssize_t count);

	int descriptor() const {
		return fd;
	}

	virtual ~socket();

};

}

#endif /* SOCKET_HPP_ */

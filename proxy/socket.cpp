/*
 * socket.cpp
 *
 *  Created on: 19.12.2011
 *      Author: sanya-m
 */

#include <cassert>
#include <cerrno>

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "log.hpp"
#include "errno_exception.hpp"
#include "nomemory.hpp"

#include "socket.hpp"

namespace myproxy {

std::ostream &operator <<(std::ostream &os, const sockaddr_in &sa) {
	char buf[INET_ADDRSTRLEN];
	::inet_ntop(sa.sin_family, &sa, buf, INET_ADDRSTRLEN);
	return os << buf;
}

socket::socket() {
	log.trace() << "Allocating new socket";

	fd = ::socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		log.warning() << "Can't create socket: "
				<< errno_exception::message(errno);
		throw errno_exception(errno);
	}

	try {
		make_nonblocking();
		return;
	} catch (...) {
		log.warning() << "Can't make socket " << fd << " non-blocking";
		close(fd);
		throw;
	}
}

void socket::connect(const char *host, const char *port) {
	log.info() << "Connecting to " << host << ":" << port;

	log.trace() << "Resolving address for " << host << ":" << port;
	addrinfo *addr;
	addrinfo hint = { 0 };
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	for (;;) {
		int r = ::getaddrinfo(host, port, &hint, &addr);
		if (r == 0) {
			break;
		}
		if (r != EAI_MEMORY) {
			log.warning() << "Error when resolving address " << host << ": "
					<< ::gai_strerror(r);
			throw std::runtime_error(std::string(::gai_strerror(r)));
		}
		log.debug() << "EAI_MEMORY when resolving address";
		new_handler();
	}
	try {
		log.trace() << "Address for " << host << ":" << port << " resolved: "
				<< *reinterpret_cast<sockaddr_in*>(addr->ai_addr);

		int r = ::connect(fd, addr->ai_addr, addr->ai_addrlen);
		if (r != 0 && errno != EINPROGRESS) {
			log.warning() << "Can't connect socket " << fd << " to " << host
					<< ": " << errno_exception::message(errno);
			throw errno_exception(errno);
		}

		if (r == EINPROGRESS) {
			log.debug() << "Connection on " << fd << " to " << host
					<< " in progress";
		} else {
			log.trace() << "Connected socket " << fd << " to " << host
					<< " successfully";
		}
		freeaddrinfo(addr);

	} catch (...) {
		freeaddrinfo(addr);
		throw;
	}
}

int socket::so_error() {
	int errval = 0;
	socklen_t errval_size = sizeof(errval);
	int r = ::getsockopt(fd, SOL_SOCKET, SO_ERROR, &errval, &errval_size);
	if (r) {
		log.warning() << "getsockopt on " << fd << " failed: "
				<< errno_exception::message(errno);
		throw errno_exception(errno);
	}assert(errval_size == sizeof(errval));
	return errval;
}

void socket::check_so_error() {
	int errval = so_error();
	if (errval) {
		log.warning() << "SO_ERROR on socket " << fd << ": "
				<< errno_exception::message(errval);
		throw errno_exception(errval);
	}
}

void socket::listen(unsigned short port) {
	log.info() << "Binding socket " << fd << " to port " << port;

	sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);
	int r = ::bind(fd, (sockaddr*) &addr, sizeof(addr));
	if (r < 0) {
		log.warning() << "Can't bind socket " << fd << " to port " << port
				<< ": " << errno_exception::message(errno);
		throw errno_exception(errno);
	}

	r = ::listen(fd, SOMAXCONN);
	if (r < 0) {
		log.warning() << "Can't listen on socket " << fd << ": "
				<< errno_exception::message(errno);
		throw errno_exception(errno);
	}

	log.info() << "Socket " << fd << " now listening on port " << port;
}

intrusive_ptr<socket> socket::accept() {
	log.info() << "Accepting new connection on socket " << fd;

	sockaddr_in clientaddr;
	socklen_t addrlen = sizeof(clientaddr);

	int nfd = ::accept(fd, (sockaddr*) &clientaddr, &addrlen);
	if (nfd < 0) {
		log.warning() << "Accept on socket " << fd << " failed: "
				<< errno_exception::message(errno);
		throw errno_exception(errno);
	}

	log.info() << "Connected client " << clientaddr;

	return intrusive_ptr<socket>(new socket(nfd));
}

void socket::make_nonblocking() {
	log.trace() << "Trying to make socket " << fd << " non-blocking";

	if (::fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK) < 0) {
		log.warning() << "fcntl failed: " << errno_exception::message(errno);
		throw errno_exception(errno);
	}
}

socket::socket(int fdes) :
		fd(fdes) {
	assert(!(fd < 0));

	make_nonblocking();
}

ssize_t socket::read(void *buf, ssize_t count) {
	log.trace() << "Reading up to " << count << " bytes from socket " << fd;
	check_so_error();

	ssize_t val = ::read(fd, buf, count);
	if (val >= 0) {
		log.trace() << "Read " << val << " of " << count
				<< " bytes from socket " << fd;
		return val;
	}

	log.warning() << "Read from socket " << fd << "failed: "
			<< errno_exception::message(errno);
	throw errno_exception(errno);
}

ssize_t socket::write(void *buf, ssize_t count) {
	log.trace() << "Writing " << count << " bytes to socket " << fd;
	check_so_error();

	ssize_t val = ::write(fd, buf, count);
	if (val >= 0) {
		log.trace() << "Wrote " << val << " of " << count << " bytes to socket "
				<< fd;
		return val;
	}

	log.warning() << "Write to socket " << fd << "failed: "
			<< errno_exception::message(errno);
	throw errno_exception(errno);
}

socket::~socket() {
	log.trace() << "Closing socket " << fd;
	if (::close(fd) == 0) {
		log.trace() << "Closed socket " << fd << " successfully";
		return;
	}
	log.warning() << "Error when closing socket " << fd << ": "
			<< errno_exception::message(errno);
}
}

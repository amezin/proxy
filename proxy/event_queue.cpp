/*
 * event_queue.cpp
 *
 *  Created on: 19.12.2011
 *      Author: sanya-m
 */

#include <cerrno>

#include "event_queue.hpp"
#include "errno_exception.hpp"
#include "log.hpp"

namespace myproxy {

connection_handler::connection_handler(const intrusive_ptr<socket> &s,
		event_queue &queue) :
		sck(s), q(queue) {

}

connection_handler::~connection_handler() {

}

event_queue::event_queue() :
		front(0) {

}

event_queue::~event_queue() {

}

int event_queue::poll() {
	handlers.swap(new_handlers);
	fds.swap(new_fds);

	new_handlers.clear();
	new_fds.clear();

	front = 0;

	log.debug() << "Doing poll on " << fds.size() << " descriptors";

	int r = ::poll(&fds[0], fds.size(), -1);
	if (r < 0) {
		log.warning() << "Poll failed: " << errno_exception::message(errno);
		throw errno_exception(errno);
	}
	return r;
}

bool event_queue::pop() {
	log.trace() << "Requesting an event from queue. Queue size: " << fds.size()
			<< ", front=" << front;

	while (front != fds.size() && fds[front].revents == 0) {
		log.trace() << "Socket " << fds[front].fd << " has no events, copying";

		new_handlers.push_back(handlers[front]);
		try {
			new_fds.push_back(fds[front]);
		} catch (...) {
			new_handlers.pop_back();
			throw;
		}
		++front;
	}

	if (front == fds.size()) {
		log.trace() << "Queue emptied";
		return false;
	}

	log.trace() << "Socket " << fds[front].fd << " ready";

	int e = fds[front].revents;
	++front;
	intrusive_ptr<connection_handler> p = handlers[front - 1];
	handlers[front - 1] = 0;

	log.trace() << "Processing event";
	log.trace() << "POLLIN: " << ((e & POLLIN) != 0);
	log.trace() << "POLLOUT: " << ((e & POLLOUT) != 0);
	log.trace() << "POLLHUP: " << ((e & POLLHUP) != 0);
	log.trace() << "POLLERR: " << ((e & POLLERR) != 0);
	log.trace() << "OTHER: " << ((e & ~(POLLIN | POLLOUT | POLLHUP | POLLERR)) != 0);

	int repush = p->handle(e);
	if (repush) {
		log.trace() << "Handler requested re-push";
		push(p, repush);
	}

	log.trace() << "Event processed successfully";
	return true;
}

void event_queue::push(const intrusive_ptr<connection_handler> &p, int events) {
	log.trace() << "Pushing socket " << p->sock().descriptor() << " to queue";

	assert(p);

	::pollfd pfd;
	pfd.revents = 0;
	pfd.events = events;
	pfd.fd = p->sock().descriptor();

	new_handlers.push_back(p);
	try {
		new_fds.push_back(pfd);
	} catch (...) {
		new_handlers.pop_back();
		throw;
	}
}

}


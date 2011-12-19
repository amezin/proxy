/*
 * event_queue.hpp
 *
 *  Created on: 19.12.2011
 *      Author: sanya-m
 */

#ifndef EVENT_QUEUE_HPP_
#define EVENT_QUEUE_HPP_

#include <vector>

#include <poll.h>

#include "socket.hpp"
#include "ref_count.hpp"

namespace myproxy {

class event_queue;

class connection_handler : public ref_counted {

	intrusive_ptr<socket> sck;
	event_queue &q;

	friend class event_queue;

protected:

	connection_handler(const intrusive_ptr<socket> &, event_queue &);

public:

	event_queue &queue() const {
		return q;
	}

	socket &sock() const {
		return *(this->sck);
	}

	virtual int handle(int pollflags) = 0;

	virtual ~connection_handler();

};

class event_queue : public ref_counted {
	typedef std::vector<intrusive_ptr<connection_handler> > handler_set;

	handler_set handlers;
	handler_set new_handlers;

	std::vector<pollfd> fds;
	std::vector<pollfd> new_fds;

	std::size_t front;

public:

	event_queue();
	virtual ~event_queue();

	bool pop();
	void push(const intrusive_ptr<connection_handler> &p, int events);

	int poll();
};

}


#endif /* EVENT_QUEUE_HPP_ */

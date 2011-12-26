/*
 * buffer.hpp
 *
 *  Created on: 20.12.2011
 *      Author: sanya-m
 */

#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <cassert>
#include <vector>

#include "log.hpp"
#include "ref_count.hpp"

namespace myproxy {

class buffer: public ref_counted {
	std::vector<char> buf;
	std::size_t used;

public:
	static const std::size_t default_buffer_size = 1024;

	buffer(std::size_t size = default_buffer_size) : buf(default_buffer_size), used(0) {

	}

	buffer(const std::string &s) : buf(s.begin(), s.end()), used(s.length()) {
		grow();
	}

	std::vector<char> &get() {
		return buf;
	}

	char *ptr() {
		return &buf[0];
	}

	void grow() {
		if (buf.capacity() == buf.size()) {
			buf.push_back(0);
			log.trace() << "Buffer grown from " << buf.size() - 1 << " to " << buf.capacity();
		}
		buf.resize(buf.capacity());
	}

	std::size_t size() const {
		return used;
	}

	void size(std::size_t u) {
		assert(u <= buf.size());
		used = u;
	}

	std::size_t capacity() const {
		return buf.size();
	}

	void append_char(char c) {
		if (used >= buf.size()) {
			grow();
		}
		buf[used++] = c;
	}

	template<typename Iter>
	void append(Iter begin, Iter end) {
		while (begin != end) {
			append_char(*begin);
			begin++;
		}
	}

	virtual ~buffer() {

	}
};

}

#endif /* BUFFER_HPP_ */

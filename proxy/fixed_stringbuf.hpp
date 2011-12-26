/*
 * fixed_stringbuf.hpp
 *
 *  Created on: 18.12.2011
 *      Author: sanya-m
 */

#ifndef FIXED_STRINGBUF_HPP_
#define FIXED_STRINGBUF_HPP_

#include <algorithm>
#include <streambuf>

namespace myproxy {

template<std::streamsize Size, typename Char, typename Traits = std::char_traits<Char> >
class fixed_stringbuf: public std::basic_streambuf<Char, Traits> {
public:
	static const std::streamsize size = Size;

	typedef Char char_type;
	typedef Traits traits_type;

	typedef typename Traits::int_type int_type;
	typedef typename Traits::pos_type pos_type;
	typedef typename Traits::off_type off_type;

private:
	Char buf[Size];
protected:

	virtual int_type underflow(int_type c) {
		if (this->gptr() >= this->pptr()) return Traits::eof();

		setg(buf, this->gptr(), this->pptr());
		return *(this->gptr());
	}

	virtual std::streamsize showmanyc() {
		return this->gptr() - this->pptr();
	}

public:

	void reset() {
		std::fill(buf, buf + Size, 0);
		setp(buf, buf + Size - 1);
		setg(buf, buf, buf);
	}

	bool empty() const {
		return (this->pptr() == buf);
	}

	fixed_stringbuf() {
		reset();
	}

	explicit fixed_stringbuf(const Char *str) {
		reset();
		Char *p = buf;
		while (*str && p + 1 < buf + Size) {
			*p++ = *str++;
		}
		*p = 0;
	}

	const Char *get() const {
		return buf;
	}

	virtual ~fixed_stringbuf() {

	}

};

}

#endif /* FIXED_STRINGBUF_HPP_ */

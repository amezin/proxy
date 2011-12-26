/*
 * ref_count.hpp
 *
 *  Created on: 19.12.2011
 *      Author: sanya-m
 */

#ifndef REF_COUNT_HPP_
#define REF_COUNT_HPP_

#include <cassert>
#include <cstdlib>

#include <algorithm>

#include "log.hpp"

namespace myproxy {

class ref_counted {
	size_t ref_cnt;

	ref_counted(const ref_counted &);
	ref_counted &operator =(const ref_counted &);

	friend class intrusive_ptr_base;

protected:
	ref_counted() :
			ref_cnt(1) {

	}

	virtual ~ref_counted() {

	}

public:
	size_t ref_count() const {
		return ref_cnt;
	}
};

class intrusive_ptr_base {
protected:
	intrusive_ptr_base() {

	}

	intrusive_ptr_base(const intrusive_ptr_base &r) {

	}

	static void add_ref(ref_counted *p) {
		if (!p)
			return;

		assert(p->ref_cnt);
		p->ref_cnt++;
#ifdef REF_COUNT_PTR_DEBUG
		log.trace() << "Added ref to " << p << ". New ref_count=" << p->ref_cnt;
#endif
	}
	static void release(ref_counted *p) {
		if (!p)
			return;

		assert(p->ref_cnt);
		p->ref_cnt--;
#ifdef REF_COUNT_PTR_DEBUG
		log.trace() << "Removed ref to " << p << ". New ref_count=" << p->ref_cnt;
#endif
		if (p->ref_cnt == 0) {
			delete p;
		}
	}
};

template<typename T>
class intrusive_ptr: public intrusive_ptr_base {
	T *ptr;
public:

	intrusive_ptr(T* v = 0) :
			ptr(v) {
#ifdef REF_COUNT_PTR_DEBUG
		log.trace() << "Constructed intrusive_ptr to " << ptr;
#endif
		if (ptr)
			assert(ptr->ref_count() == 1);
	}

	template<typename T2>
	intrusive_ptr(T2* v = 0) :
			ptr(v) {
#ifdef REF_COUNT_PTR_DEBUG
		log.trace() << "Constructed intrusive_ptr to " << ptr;
#endif
		if (ptr)
			assert(ptr->ref_count() == 1);
	}

	template<typename T2>
	static intrusive_ptr<T> from_existing(T2 *v = 0) {
		assert(v);
		assert(v->ref_count() > 0);

#ifdef REF_COUNT_PTR_DEBUG
		log.trace() << "Constructed intrusive_ptr to existing ref_counted " << v;
#endif

		intrusive_ptr<T> r;
		r.ptr = v;
		intrusive_ptr_base::add_ref(r.ptr);
		return r;
	}

	T *raw() const {
		return ptr;
	}

	T *operator ->() const {
		assert(ptr);
		return ptr;
	}

	T &operator *() const {
		assert(ptr);
		return *ptr;
	}

	operator bool() const {
		return ptr != 0;
	}

	intrusive_ptr(const intrusive_ptr &r) :
			ptr(r.ptr) {
		intrusive_ptr_base::add_ref(ptr);
#ifdef REF_COUNT_PTR_DEBUG
		log.trace() << "Copied intrusive_ptr to " << ptr;
#endif
	}

	intrusive_ptr &operator =(const intrusive_ptr &r) {
#ifdef REF_COUNT_PTR_DEBUG
		log.trace() << "Assigning intrusive_ptr to " << ptr << " = " << r.ptr;
#endif
		intrusive_ptr_base::add_ref(r.ptr);
		intrusive_ptr_base::release(ptr);
		ptr = r.ptr;
		return *this;
	}

	template<typename T2>
	intrusive_ptr(const intrusive_ptr<T2> &r) :
			ptr(r.raw()) {
		intrusive_ptr_base::add_ref(ptr);
#ifdef REF_COUNT_PTR_DEBUG
		log.trace() << "Copied intrusive_ptr to " << ptr;
#endif
	}

	template<typename T2>
	intrusive_ptr &operator =(const intrusive_ptr<T2> &r) {
#ifdef REF_COUNT_PTR_DEBUG
		log.trace() << "Assigning intrusive_ptr to " << ptr << " = " << r.ptr;
#endif
		intrusive_ptr_base::add_ref(r.raw());
		intrusive_ptr_base::release(ptr);
		ptr = r.raw();
		return *this;
	}

	void swap(intrusive_ptr &r) {
		std::swap(ptr, r.ptr);
	}

	~intrusive_ptr() {
#ifdef REF_COUNT_PTR_DEBUG
		log.trace() << "Deleting intrusive_ptr to " << ptr;
#endif
		intrusive_ptr_base::release(ptr);
	}
};

}

#endif /* REF_COUNT_HPP_ */

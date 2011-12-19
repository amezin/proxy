/*
 * nomemory.cpp
 *
 *  Created on: 19.12.2011
 *      Author: sanya-m
 */

#include <memory>

#include "log.hpp"
#include "nomemory.hpp"

namespace myproxy {

void new_handler() {
	log.info() << "Ran out of memory, trying to free some";
	throw std::bad_alloc();
}

}

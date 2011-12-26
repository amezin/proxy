/*
 * error_pages.hpp
 *
 *  Created on: 20.12.2011
 *      Author: sanya-m
 */

#ifndef ERROR_PAGES_HPP_
#define ERROR_PAGES_HPP_

#include <string>

namespace myproxy {

namespace error_pages {

extern const ::std::string bad_request, not_found, internal_error,
		not_implemented, bad_gateway, service_unavailable, version_not_supported;

}

}

#endif /* ERROR_PAGES_HPP_ */

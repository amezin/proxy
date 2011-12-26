/*
 * client_headers_handler.cpp
 *
 *  Created on: 20.12.2011
 *      Author: sanya-m
 */

#include <cstring>
#include <sstream>

#include "log.hpp"
#include "client_headers_handler.hpp"

namespace myproxy {

namespace {

const std::string http_version("HTTP/1.0");
const std::string get_method("GET");
const std::string head_method("HEAD");
const std::string url_prefix("http://");
const std::string default_port("80");

}

void client_headers_handler::handle_first_line(const std::string &ln) {
	std::istringstream is(ln);
	char test;

	log.debug() << "HTTP request: " << ln;

	if (!(is >> method >> url >> version) || (is >> test)) {
		throw request_parse_error("First line contains invalid argument count");
	}

	if (version != http_version) {
		throw http_version_not_supported_error("Http version is not supported");
	}

	if (method != get_method && method != head_method) {
		throw not_implemented_error("Request method is not implemented");
	}

	if (std::strncmp(url.c_str(), url_prefix.c_str(), url_prefix.length())) {
		throw request_parse_error("Bad url or protocol");
	}

	url.erase(0, url_prefix.length());

	size_t hostname_len = 0;
	while (hostname_len < url.length() && url[hostname_len] != '/'
			&& url[hostname_len] != ':') {
		hostname_len++;
	}

	host = url.substr(0, hostname_len);
	url.erase(0, hostname_len);

	if (url.length() > 1 && url[0] == ':') {
		url.erase(0, 1);

		size_t portname_len = 0;
		while (portname_len < url.length() && url[portname_len] != '/') {
			portname_len++;
		}

		port = url.substr(0, portname_len);
		url.erase(0, portname_len);
	} else {
		port = default_port;
	}
}

bool client_headers_handler::handle_line(const std::string &ln) {
	if (url.empty()) {
		handle_first_line(ln);
		return true;
	}

	if (!ln.empty()) {
		log.trace() << "HTTP Header: " << ln;
		return true;
	}

	return false;
}

}

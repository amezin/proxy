/*
 * error_pages.cpp
 *
 *  Created on: 20.12.2011
 *      Author: sanya-m
 */

#include "error_pages.hpp"

namespace myproxy {

namespace error_pages {

const ::std::string bad_request("HTTP/1.0 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<html><head><title>Bad Request</title></head><body><h1>Bad Request</h1></body></html>");
const ::std::string not_found("HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><head><title>Not Found</title></head><body><h1>Not Found</h1></body></html>");
const ::std::string internal_error("HTTP/1.0 500 Internal Server Error\r\nContent-Type: text/html\r\n\r\n<html><head><title>Internal Server Error</title></head><body><h1>Internal Server Error</h1></body></html>");
const ::std::string not_implemented("HTTP/1.0 501 Not Implemented\r\nContent-Type: text/html\r\n\r\n<html><head><title>Not Implemented</title></head><body><h1>Not Implemented</h1></body></html>");
const ::std::string bad_gateway("HTTP/1.0 502 Bad Gateway\r\nContent-Type: text/html\r\n\r\n<html><head><title>Bad Gateway</title></head><body><h1>Bad Gateway</h1></body></html>");
const ::std::string service_unavailable("HTTP/1.0 503 Service Unavailable\r\nContent-Type: text/html\r\n\r\n<html><head><title>Service Unavailable</title></head><body><h1>Service Unavailable</h1></body></html>");
const ::std::string version_not_supported("HTTP/1.0 505 HTTP Version Not Supported\r\nContent-Type: text/html\r\n\r\n<html><head><title>HTTP Version Not Supported</title></head><body><h1>HTTP Version Not Supported</h1></body></html>");

}

}



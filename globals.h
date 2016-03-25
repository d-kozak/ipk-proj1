//
// Created by david on 10.3.16.
//

#ifndef IPK_PROJ1_HELPERS_H
#define IPK_PROJ1_HELPERS_H

#include <iostream>
#include <stdlib.h>

enum ret_val {
	EOK,
	WRONG_ARGUMENTS,
	URL_PARSE_ERROR,
	SOCKET_ERROR,
	GET_HOST_BY_NAME_ERROR,
	CONNECT_ERROR,
	SEND_ERROR,
	RECV_ERROR,
	CLOSE_ERROR,
	REDIRECTION_EXCEEDED_ERROR,
	UNIMPLEMENTED_HTTP_RET_VAL,
	INTERNAL_ERROR
};

#endif //IPK_PROJ1_HELPERS_H



/*	std::string url = "www.nic.cz";

	std::string msg = "GET / HTTP/1.1\r\n"
			"Host: www.nic.cz\r\n"
			"Connection: close\r\n\r\n";
	msg.append(url);
	msg.append("\n");
	msg.append("Connection: close\n");
	msg.append("Accept: n");
msg.append("Content-Length: 0\n");
msg.append("Content-Type: application/x-www-form-urlencoded\n");

std::string msg = "GET / HTTP/1.1\n"
		"Host: www.seznam.cz\n"
		"Accept: \n"
		"Content-Length: 0\n"
		"Content-Type: application/x-www-form-urlencoded\n";
std::cout << msg;
uint16_t port_number = 80;*/

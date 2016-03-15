//
// Created by david on 10.3.16.
//

#ifndef IPK_PROJ1_HELPERS_H
#define IPK_PROJ1_HELPERS_H

#include <iostream>
#include <stdlib.h>

/**
 * @brief function prints error msg and exits program with given return value
 */
inline void error(const std::string&& msg, int ret_val) {
	std::cerr << msg << "\n";
	exit(ret_val);
}

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

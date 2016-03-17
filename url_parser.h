//
// Created by david on 10.3.16.
//

#ifndef IPK_PROJ1_PARSED_URL_H
#define IPK_PROJ1_PARSED_URL_H

#include <iostream>
#include <string>
#include <vector>

#include "helpers.h"

class Parsed_url {
	std::string domain;
	std::string local_link;
	int port;

public:
	Parsed_url();
	Parsed_url(const std::string &domain, const std::string &local_link, int port);
	std::string to_string();
	void setDomain(const std::string &domain);
	void setLocal_link(const std::string &local_link);
	void setPort(int port);
	const std::string &getDomain() const;
	const std::string &getLocal_link() const;
	int getPort() const;
};

Parsed_url* parse_url(std::string& url);

#endif //IPK_PROJ1_PARSED_URL_H

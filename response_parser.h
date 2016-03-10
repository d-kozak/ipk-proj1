//
// Created by david on 10.3.16.
//

#ifndef IPK_PROJ1_RESPONSE_PARSER_H
#define IPK_PROJ1_RESPONSE_PARSER_H

#include <iostream>
#include <vector>
#include "helpers.h"

typedef enum {
	OK,
	REDIR,
	FAIL
} response_result;

struct parsed_respose{
	response_result result;
	std::string message;
};

parsed_respose parse_response(std::string& message);

#endif //IPK_PROJ1_RESPONSE_PARSER_H

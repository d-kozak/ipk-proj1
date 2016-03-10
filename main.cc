#include <iostream>
#include <regex>

#include "parsed_url.h"
#include "socket_handler.h"
#include "helpers.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		error("Wrong number of arguments, please pass only url", 1);
	}
	Parsed_url parsed_url = parse_url(argv[1]);
	communicate(parsed_url);
	return 0;

}
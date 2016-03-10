

#include "main.h"


int main(int argc, char **argv) {
	if (argc != 2) {
		error("Wrong number of arguments, please pass only url", 1);
	}
	Parsed_url parsed_url = parse_url(argv[1]);
	std::string response = communicate(parsed_url);
	std::cout << response;
	return 0;
}
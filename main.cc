

#include "main.h"


int main(int argc, char **argv) {
	if (argc != 2) {
		error("Wrong number of arguments, please pass only url", 1);
	}
	std::string url (argv[1]);
	Parsed_url* parsed_url = parse_url(url);

	std::string* response = communicate(parsed_url);

	delete parsed_url;
	delete response;
	return 0;

}
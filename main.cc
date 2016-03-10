

#include "main.h"


int main(int argc, char **argv) {
	if (argc != 2) {
		error("Wrong number of arguments, please pass only url", 1);
	}
	Parsed_url parsed_url = parse_url(argv[1]);
	std::string response = communicate(parsed_url);
	response_result result;
	int counter = 5;
	while((result = parse_response(response)) != OK && counter-- > 0){
		switch (result){
			case REDIR: break;
			case FAIL: break;
			default:
				error("Default in switch in main, shoul not happen",10);
		}
	}
	return 0;
}
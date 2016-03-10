

#include "main.h"


int main(int argc, char **argv) {
	if (argc != 2) {
		error("Wrong number of arguments, please pass only url", 1);
	}
	Parsed_url parsed_url = parse_url(argv[1]);
	std::string response = communicate(parsed_url);
	parsed_respose result;
	int counter = 5;
	while((result = parse_response(response)).result != OK && counter-- > 0){
		switch (result.result){
			case REDIR:
				response = communicate(parse_url(result.message));
				break;
			case FAIL:
				error("Fail happened",12);
				break;
			default:
				error("Default in switch in main, shoul not happen",10);
		}
	}

	std::cout << result.message;
	return 0;
}
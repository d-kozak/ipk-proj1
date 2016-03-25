#include "webclient.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Wrong number of arguments: " << argc << " , please specify only the url and nothing more";
		return WRONG_ARGUMENTS;
	}
	std::string url(argv[1]);

	std::string response;

	try {
		Parsed_url parsed_url = parse_url(url);
		int counter = 5;
		while (!(response = communicate(parsed_url)).empty()) {

			// now clear the url string and reuse it to call parse_url again
			url.clear();
			url.append(response);
			parsed_url = parse_url(url);

			if (counter-- <= 0){
				throw BaseException("Max number of redirecting was exceeded",REDIRECTION_EXCEEDED_ERROR);
			}
		}
	} catch (BaseException &e) {
		std::cerr << e.what();
		return e.getRetVal();
	}
	return 0;
}
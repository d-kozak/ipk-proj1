#include "main.h"

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Wrong number of arguments: " << argc << " , please specify only the url and nothing more";
		return WRONG_ARGUMENTS;
	}
	std::string url(argv[1]);

	char *response = NULL;
	Parsed_url *parsed_url = NULL;

	try {
		parsed_url = parse_url(url);
		int counter = 5;
		while ((response = communicate(parsed_url)) != NULL) {
			delete parsed_url; // first get rid of old instance
			parsed_url = NULL;

			// now clear the url string and reuse it to call parse_url again
			url.clear();
			url.append(response);
			parsed_url = parse_url(url);

			if (counter-- <= 0){
				throw BaseException("Max number of redirecting was exceeded",REDIRECTION_EXCEEDED_ERROR);
			}
		}
		delete parsed_url;
		free(response);
	} catch (BaseException &e) {
		std::cerr << e.what();
		if (parsed_url != NULL)
			delete parsed_url;
		if (response != NULL)
			free(response);
		return e.getRetVal();
	}
	return 0;
}
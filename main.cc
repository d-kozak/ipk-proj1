#include "main.h"

int ret_val = 0;

int main(int argc, char **argv) {
	if (argc != 2) {
		error("Wrong number of arguments, please pass only url", 1);
		return ret_val;
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

			if (counter-- <= 0)
				throw new RedirectionNumberExceededException();
		}
		delete parsed_url;
		free(response);
	} catch (InvalidUrlException &e) {
		if (parsed_url != NULL)
			delete parsed_url;
		if (response != NULL)
			free(response);
	} catch (SocketErrorException &e) {
		if (parsed_url != NULL)
			delete parsed_url;
		if (response != NULL)
			free(response);
	} catch (SocketHandlerInternalException &e) {
		if (parsed_url != NULL)
			delete parsed_url;
		if (response != NULL)
			free(response);
	} catch (RedirectionNumberExceededException &e) {
		if (parsed_url != NULL)
			delete parsed_url;
		if (response != NULL)
			free(response);
	}

	return ret_val;
}
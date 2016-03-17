//
// Created by david on 10.3.16.
//

#include <vector>
#include "socket_handler.h"
#include <fstream>

static const int BUFFER_SIZE = 2048;
static const int HEADER_SIZE = 320;

using namespace std;

int parse_ret_val(char *buffer, ssize_t bytes_count) {
	int index = 0;
	int col = 0;
	vector<char> val;
	while (buffer[index] != '\r' && index < bytes_count) {
		if (col == 1) {
			val.push_back(buffer[index]);
		}
		if (isspace(buffer[index])) {
			col++;
		}
		index++;
	}
	if (!val.empty())
		return stoi(val.data());
	else
		return 404;
}

static vector<char> *remove_header(char *buffer, bool &isChunked) {
	vector<char> *res = new vector<char>;
	int counter = 0;
	if (strstr(buffer, "Transfer-Encoding: chunked") != NULL)
		isChunked = true;

	while (*buffer != '\0') {
		if (*buffer == '\r') {
			counter++;
		} else if (*buffer == '\n') {
			if (counter == 3) {
				buffer++;
				break;
			}
			else
				counter++;
		} else
			counter = 0;
		buffer++;
	}

	while (*buffer != '\0') {
		res->push_back(*buffer);
		buffer++;
	}

	return res;
}

/**
 * Function prints the reponse without chunks into file output_file
 */
static void print_without_chunk_numbers(vector<char> *data, ofstream &output_file) {
	long i = 0, end = (int) data->size(), chunk_size;
	vector<char> chunk_num;

	vector<char> tmp_tester;
	size_t next_char;
	// loop through the whole data vector
	while (i < end) {
		// parse the chunk size(it always ends with \r\n
		while ((*data)[i] != '\r') {
			chunk_num.push_back((*data)[i]);
			i++;
		}
		i += 2; //skip the "\r\n"

		if (chunk_num.size() == 1 && chunk_num[0] == '0') // no more data to print
			break;

		// get the size of the current chunk
		chunk_size = stol(chunk_num.data(), &next_char, 16);

		// print current chunk into file
		for (std::vector<char>::iterator it = data->begin() + i; it != data->begin() + i + chunk_size; ++it) {
			output_file << *it;
			tmp_tester.push_back(*it);
		}
		//jump to next chunk
		i += chunk_size + 2;
		// clear the vector for storing chunk_size;
		chunk_num.clear();
	}
}

char *parse_next_location(char *buffer) {
	//cout << buffer << "\n";
	static const string location_header = "Location: ";
	char *start_of_url = strstr(buffer, location_header.c_str());
	start_of_url += location_header.size();
	char *end_of_url = strchr(start_of_url, '\r');
	unsigned long url_size = end_of_url - start_of_url;
	char *res = (char *) malloc(url_size + 1);
	memcpy(res, start_of_url, url_size);
	res[url_size] = '\0';
	return res;
}

/**
 * Function parses filename from local_link into result
 * If there is no local_link specified, the filename will be index.html
 */
void parse_file_name(const string &local_link, string &result) {
	result.clear(); //clear the string

	if (local_link == "/") {
		result.append("index.html");
	} else {
		unsigned long last_slash_index = local_link.find_last_of('/');
		result.append(local_link.substr(last_slash_index + 1, local_link.size() - last_slash_index + 1));
	}
}

static std::string create_http_request(const Parsed_url &parsed_url) {
	std::string message = "GET " + parsed_url.getLocal_link() + " HTTP/1.1\r\n";
	message.append("Host: " + parsed_url.getDomain() + "\r\n");
	message.append("Connection: close\r\n\r\n");
	return message;
}

/**
 * Function communicates with specified server using BSD socket
 * @return string - next url to search at, NULL means success
 */
char* communicate(const Parsed_url *parsed_url) {
	int client_socket;
	std::string msg = create_http_request(*parsed_url);

	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
		perror("ERROR: socket");
		error("", 1);
		throw SocketErrorException();
	}

	hostent *server = gethostbyname(parsed_url->getDomain().c_str());
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host  as %s.\n", parsed_url->getDomain().c_str());
		error("", 2);
		throw SocketErrorException();
	}

	struct sockaddr_in server_address;
	bzero((char *) &server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &server_address.sin_addr.s_addr, (size_t) server->h_length);

	server_address.sin_port = htons((uint16_t) parsed_url->getPort());

	if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
		perror("ERROR: connect");
		error("", 3);
		throw SocketErrorException();
	}

	ssize_t bytes_count = 0;
	//std::cout << "size: " << msg.size() << "strlen: " << strlen((msg.c_str())) << "\n";
	bytes_count = send(client_socket, msg.c_str(), msg.size(), 0);
	if (bytes_count < 0) {
		perror("ERROR: sendto");
		error("", 4);
		throw SocketErrorException();
	}

	char buffer[BUFFER_SIZE + 1];
	memset(buffer, 0, BUFFER_SIZE);

	int ret_val;
	// first process the header
	if ((bytes_count = recv(client_socket, buffer, HEADER_SIZE, 0)) > 0) {
		switch (ret_val = parse_ret_val(buffer, bytes_count)) {
			case 200:
				break;
			case 301:
				error("Redirecting 301 not implemented yet", 9);
				throw SocketHandlerInternalException();
			case 302:
				return parse_next_location(buffer);;
			case 404:
				error("Page not found", 8);
				throw PageNotFoundException();
			default:
				cerr << ret_val << "\n";
				error("Unknown return value", 7);
				throw SocketHandlerInternalException();
		}
	} else {
		error("No data received", 10);
		throw SocketHandlerInternalException();
	}

	// get the first part of data
	bool isChunked = false;
	vector<char> *data = remove_header(buffer, isChunked);

	// clear the buffer
	memset(buffer, 0, BUFFER_SIZE);

	while ((bytes_count = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
		for (int i = 0; i < bytes_count; i++)
			data->push_back(buffer[i]);
		memset(buffer, 0, BUFFER_SIZE);
	}


	if (bytes_count < 0) {
		delete parsed_url;
		perror("ERROR: recvfrom");
		error("", 5);
		throw SocketErrorException();
	}
	if (close(client_socket) != 0) {
		delete parsed_url;
		perror("ERROR: close");
		error("", 5);
		throw SocketErrorException();
	}

	string file_name;
	parse_file_name(parsed_url->getLocal_link(), file_name);


	// open the file for writing
	ofstream output_file;
	output_file.open(file_name);

	if (isChunked)
		print_without_chunk_numbers(data, output_file);
	else
		output_file << data->data();

	output_file.close();
	return NULL;
}


//
// Created by david on 10.3.16.
//

#include <vector>
#include "socket_handler.h"
#include <fstream>

static const int BUFFER_SIZE = 2048;
static const int HEADER_SIZE = 320;

using namespace std;

static inline bool is_version_10(char buffer[]) {
	return buffer[7] == '0';
}

/**
 * Function parses HTPP response return value
 * @return int HTTP response return value
 */
int parse_ret_val(char *buffer) {
	static const int CODE_SIZE = 3; // len of the code is constant
	while (*buffer != ' ') //skip the 'HTTP/1.1 part'
		buffer++;
	buffer++; // skip the whitespace

	char code[CODE_SIZE + 1];
	memcpy(code, buffer, CODE_SIZE); //copy the code to separate memory
	code[3] = '\0'; // just to be sure, end the string
	char *ptr;
	int num = (int) strtol(code, &ptr, 10);
	if (*ptr != '\0') {
		cerr << "-" << *ptr << "-";
		throw BaseException("Converting of ret val in response was not successfull", INTERNAL_ERROR);
	}
	return num;
}

static vector<char> *remove_header(char *buffer, bool &isChunked, ssize_t bytes_count) {
	vector<char> *res = new vector<char>;
	int new_lines_counter = 0;
	ssize_t index = 0;
	if (strstr(buffer, "Transfer-Encoding: chunked") != NULL)
		isChunked = true;

	bool encouteredZero = false;

	while (true) {
		if (*buffer == '\0') {
			if (encouteredZero) {
				cout << "Encoutered two '0' in sequence\n";
				break;
			}
			else encouteredZero = true;
		} else
			encouteredZero = false;
		if (*buffer == '\r') {
			new_lines_counter++;
		} else if (*buffer == '\n') {
			if (new_lines_counter == 3) {
				buffer++;
				break;
			}
			else
				new_lines_counter++;
		} else
			new_lines_counter = 0;
		buffer++;
		index++;
	}

	while (index++ < bytes_count - 1) {
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

		std::copy(data->begin() + i, data->begin() + i + chunk_size, std::ostream_iterator<char>(output_file));

		//jump to next chunk
		i += chunk_size + 2;
		// clear the vector for storing chunk_size;
		chunk_num.clear();
	}
}

char *parse_next_location(char *buffer) {
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
		string last_part = local_link.substr(last_slash_index + 1, local_link.size() - last_slash_index + 1);

		unsigned long pos;
		unsigned long i = 0;

		while ((pos = last_part.find("%20")) != string::npos) {
			result.append(last_part.substr(i, pos) + " ");
			i = pos + 3; //jump over the %20
			last_part = last_part.substr(i, last_part.size());
		}

		result.append(last_part);
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
char *communicate(const Parsed_url *parsed_url) {
	int client_socket;
	std::string msg = create_http_request(*parsed_url);

	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
		perror("ERROR: socket");
		throw BaseException("Socket was not created succesfully", SOCKET_ERROR);
	}

	hostent *server = gethostbyname(parsed_url->getDomain().c_str());
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host  as %s.\n", parsed_url->getDomain().c_str());
		throw BaseException("DNS translation was not succesfull", GET_HOST_BY_NAME_ERROR);
	}

	struct sockaddr_in server_address;
	bzero((char *) &server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &server_address.sin_addr.s_addr, (size_t) server->h_length);

	server_address.sin_port = htons((uint16_t) parsed_url->getPort());

	if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
		perror("ERROR: connect");
		throw BaseException("Connection was not started successfully\n", CONNECT_ERROR);
	}

	ssize_t bytes_count = 0;
	//std::cout << "size: " << msg.size() << "strlen: " << strlen((msg.c_str())) << "\n";
	bytes_count = send(client_socket, msg.c_str(), msg.size(), 0);
	if (bytes_count < 0) {
		perror("ERROR: sendto");
		throw BaseException("Request was not send successfully\n", SEND_ERROR);
	}

	char buffer[BUFFER_SIZE + 1];
	memset(buffer, 0, BUFFER_SIZE);

	int ret_val;
	// first process the header
	if ((bytes_count = recv(client_socket, buffer, HEADER_SIZE, 0)) > 0) {
		if (is_version_10(buffer)) {
			cerr << "This is 1.0 ! :O";
			exit(UNIMPLEMENTED_HTTP_RET_VAL);
		}

		switch (ret_val = parse_ret_val(buffer)) {
			case 200: // ok
				break;
			case 301:
				cout << buffer;
				throw BaseException("301 Not implemented yet", INTERNAL_ERROR);
			case 302:
				return parse_next_location(buffer);
			default:
				std::cerr << "RET VAL: " << ret_val << "\n";
				throw BaseException("HTTP ERROR", UNIMPLEMENTED_HTTP_RET_VAL);
		}
	} else {
		throw BaseException("No data received", RECV_ERROR);
	}

	// get the first part of data
	bool isChunked = false;
	vector<char> *data = remove_header(buffer, isChunked, bytes_count);

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
		throw BaseException("The transminsion of data was not successfull", RECV_ERROR);
	}
	if (close(client_socket) != 0) {
		delete parsed_url;
		perror("ERROR: close");
		throw BaseException("Closing of socket was not successfull", CLOSE_ERROR);
	}

	string file_name;
	parse_file_name(parsed_url->getLocal_link(), file_name);


	// open the file for writing
	ofstream output_file(file_name, std::ios_base::binary);

	if (isChunked)
		print_without_chunk_numbers(data, output_file);
	else {
		std::copy(data->begin(), data->end(), std::ostream_iterator<char>(output_file));
	}
	output_file.close();
	delete data;
	return NULL;
}


//
// Created by david on 10.3.16.
//

#include <vector>
#include "socket_handler.h"
#include <fstream>

static const int BUFFER_SIZE = 2048;
static const int HEADER_SIZE = 320;

using namespace std;

static inline bool is_version_10(vector<char> &response) {
	return response[7] == '0';
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

/**
 * Function erases header from the response
 */
static void remove_header(vector<char> &buffer, ssize_t &bytes_count) {
	static const string header_end = "\r\n\r\n"; // specific header end accorting to http protocol
	char *end = strstr(buffer.data(), header_end.data()); // try to find the end of header

	if (end == NULL) {
		throw BaseException("Deformed header, does not end with two carry + newlines", INTERNAL_ERROR);
	}

	unsigned long end_of_head = end - buffer.data() + header_end.size();

	// remove the head size from bytes count(only the real data will stay in vector,head will be erased
	bytes_count -= end_of_head;

	// erase header from response
	buffer.erase(buffer.begin(), buffer.begin() + end_of_head);
}

/**
 * Function prints the reponse without chunks into file output_file
 */
static void print_without_chunk_numbers(vector<char> &data, ofstream &output_file) {
	long i = 0, end = (int) data.size(), chunk_size;
	vector<char> chunk_num;

	size_t next_char;
	// loop through the whole data vector
	while (i < end) {
		// parse the chunk size(it always ends with \r\n
		while (data[i] != '\r') {
			chunk_num.push_back(data[i]);
			i++;
		}
		i += 2; //skip the "\r\n"

		if (chunk_num.size() == 1 && chunk_num[0] == '0') // no more data to print
			break;

		// get the size of the current chunk
		chunk_size = stol(chunk_num.data(), &next_char, 16);

		std::copy(data.begin() + i, data.begin() + i + chunk_size, std::ostream_iterator<char>(output_file));

		//jump to next chunk
		i += chunk_size + 2;
		// clear the vector for storing chunk_size;
		chunk_num.clear();
	}
}

/**
 * Function parses next location from html response header
 */
string parse_next_location(vector<char> &response) {
	static const string location_header = "Location: ";

	char *start_of_location_attribute = strstr(response.data(), location_header.data());

	if (start_of_location_attribute == NULL) {
		throw BaseException("Deformed redirection header, it does not contain info about the next location to look at",
							INTERNAL_ERROR);
	}

	char *end_of_location_line = strchr(start_of_location_attribute, '\r');
	start_of_location_attribute += location_header.size();

	unsigned long startIndex = start_of_location_attribute - response.data();
	unsigned long endIndex = end_of_location_line - response.data();

	return string(response.begin() + startIndex, response.begin() + endIndex);
}

/**
 * Function parses filename from local_link into result
 * If no local_link is specified, the filename will be index.html
 */
string parse_file_name(const string &local_link) {

	if (local_link == "/") {
		return "index.html";
	} else {
		string file_name;
		unsigned long last_slash_index = local_link.find_last_of('/');
		string last_part = local_link.substr(last_slash_index + 1, local_link.size() - last_slash_index + 1);

		unsigned long pos;
		unsigned long i = 0;

		// check for spaces( %20 is their internal representation)
		while ((pos = last_part.find("%20")) != string::npos) {
			file_name.append(last_part.substr(i, pos) + " ");
			i = pos + 3; //jump over the %20
			last_part = last_part.substr(i, last_part.size());
		}

		file_name.append(last_part);
		return file_name;
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
string communicate(const Parsed_url &parsed_url) {
	int client_socket;
	std::string msg = create_http_request(parsed_url);

	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
		perror("ERROR: socket");
		throw BaseException("Socket was not created succesfully", SOCKET_ERROR);
	}

	hostent *server = gethostbyname(parsed_url.getDomain().c_str());
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host  as %s.\n", parsed_url.getDomain().c_str());
		throw BaseException("DNS translation was not succesfull", GET_HOST_BY_NAME_ERROR);
	}

	struct sockaddr_in server_address;
	bzero((char *) &server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &server_address.sin_addr.s_addr, (size_t) server->h_length);

	server_address.sin_port = htons((uint16_t) parsed_url.getPort());

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


	vector<char> response;
	response.resize(BUFFER_SIZE);

	int ret_val;
	// first process the header
	if ((bytes_count = recv(client_socket, response.data(), HEADER_SIZE, 0)) > 0) {
		if (is_version_10(response)) {
			cerr << "This is 1.0 ! :O";
			exit(UNIMPLEMENTED_HTTP_RET_VAL);
		}

		switch (ret_val = parse_ret_val(response.data())) {
			case 200: // ok
				break;
			case 301:
				cout << response.data();
				throw BaseException("301 Not implemented yet", INTERNAL_ERROR);
			case 302:
				return parse_next_location(response);
			default:
				std::cerr << "RET VAL: " << ret_val << "\n";
				throw BaseException("HTTP ERROR", UNIMPLEMENTED_HTTP_RET_VAL);
		}
	} else {
		throw BaseException("No data received", RECV_ERROR);
	}

	// get the first part of data
	bool isChunked = strstr(response.data(), "Transfer-Encoding: chunked") != NULL;
	remove_header(response, bytes_count);


	unsigned long oldSize = 0;
	do {
		oldSize += (unsigned long) bytes_count;
		response.resize(oldSize + BUFFER_SIZE);
	} while ((bytes_count = recv(client_socket, response.data() + oldSize, BUFFER_SIZE, 0)) > 0);


	if (bytes_count < 0) {
		perror("ERROR: recvfrom");
		throw BaseException("The transminsion of data was not successfull", RECV_ERROR);
	}
	if (close(client_socket) != 0) {
		perror("ERROR: close");
		throw BaseException("Closing of socket was not successfull", CLOSE_ERROR);
	}

	string file_name = parse_file_name(parsed_url.getLocal_link());


	// open the file for writing
	ofstream output_file(file_name, std::ios_base::binary);

	if (isChunked)
		print_without_chunk_numbers(response, output_file);
	else {
		std::copy(response.begin(), response.begin() + oldSize, std::ostream_iterator<char>(output_file));
	}
	output_file.close();
	return "";
}


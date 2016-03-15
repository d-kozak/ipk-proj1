//
// Created by david on 10.3.16.
//

#include <vector>
#include "socket_handler.h"

static const int BUFFER_SIZE = 2048;
static const int HEADER_SIZE = 320;

using namespace std;

int parse_ret_val(char *buffer){
	int index = 0;
	int col = 0;
	vector<char> val;
	while(buffer[index] != '\r'){
		if(col == 1){
			val.push_back(buffer[index]);
		}
		if(isspace(buffer[index])){
			col++;
		}
		index++;
	}
	return stoi(val.data());
}

vector<char>* remove_header(char *buffer){
	vector<char>* res = new vector<char>;
	int counter = 0;
	while(*buffer != '\0'){
		if(*buffer == '\r'){
			counter++;
		} else if(*buffer == '\n'){
			if(counter == 3) {
				buffer++;
				break;
			}
			else
				counter++;
		} else
			counter = 0;
		buffer++;
	}

	while(*buffer != '\0'){
		res->push_back(*buffer);
		buffer++;
	}

	return res;
}

static std::string create_http_request(const Parsed_url &parsed_url) {
	std::string message = "GET " + parsed_url.getLocal_link() + " HTTP/1.1\r\n";
	message.append("Host: " + parsed_url.getDomain() + "\r\n");
	message.append("Connection: close\r\n\r\n");
	return message;
}

std::string* communicate(const Parsed_url* parsed_url){
	int client_socket;
	std::string msg = create_http_request(*parsed_url);

	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
		perror("ERROR: socket");
		exit(EXIT_FAILURE);
	}

	hostent *server = gethostbyname(parsed_url->getDomain().c_str());
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host  as %s.\n", parsed_url->getDomain().c_str());
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_address;
	bzero((char *) &server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &server_address.sin_addr.s_addr, (size_t) server->h_length);

	server_address.sin_port = htons((uint16_t) parsed_url->getPort());

	if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
		perror("ERROR: connect");
		exit(EXIT_FAILURE);
	}

	ssize_t bytes_count = 0;
	//std::cout << "size: " << msg.size() << "strlen: " << strlen((msg.c_str())) << "\n";
	bytes_count = send(client_socket, msg.c_str(), msg.size(), 0);
	if (bytes_count < 0) {
		perror("ERROR: sendto");
		exit(EXIT_FAILURE);
	}

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	std::string* response = new std::string("");

	int ret_val;
	// first process the header
	if((bytes_count = recv(client_socket,buffer,HEADER_SIZE,0)) > 0){
		switch(ret_val = parse_ret_val(buffer)){
			case 200:
				break;
			case 301:
			case 302:
				error("Redirecting not implemented yet",9);
				break;
			case 404:
				error("Page not found",8);
				break;
			default:
				cerr << ret_val << "\n";
				error("Unknown return value",7);
		}
	} else {
		error("No data received",10);
	}

	vector<char>* data = remove_header(buffer);

	cout << buffer << "\n";
	cout << "-------------------------\n";
	cout << data->data();
	exit(666);

	while ((bytes_count = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
		response->append(buffer);
		memset(buffer, 0, BUFFER_SIZE);
	}

	if (bytes_count < 0) {
		perror("ERROR: recvfrom");
		exit(EXIT_FAILURE);
	}
	if (close(client_socket) != 0) {
		perror("ERROR: close");
		exit(EXIT_FAILURE);
	}

	// TODO return the data vector somehow, or use it in some other way
	return response;
}


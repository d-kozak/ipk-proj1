//
// Created by david on 10.3.16.
//

#include "socket_handler.h"

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

static const int BUFFER_SIZE = 2048;

static std::string create_http_request(const Parsed_url &parsed_url) {
	std::string message = "GET " + parsed_url.getLocal_link() + " HTTP/1.1\r\n";
	message.append("Host: " + parsed_url.getDomain() + "\r\n");
	message.append("Connection: close\r\n\r\n");
	return message;
}


void communicate(const Parsed_url &parsed_url) {
	int client_socket;
	std::string msg = create_http_request(parsed_url);

	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
		perror("ERROR: socket");
		exit(EXIT_FAILURE);
	}

	hostent *server = gethostbyname(parsed_url.getDomain().c_str());
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host  as %s.\n", parsed_url.getDomain().c_str());
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_address;
	bzero((char *) &server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &server_address.sin_addr.s_addr, (size_t) server->h_length);

	server_address.sin_port = htons((uint16_t) parsed_url.getPort());

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
	std::string response = "";


	while ((bytes_count = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
		std::cout << "inside while loop";
		response.append(buffer);
		memset(buffer, 0, BUFFER_SIZE);
	}

	if (bytes_count < 0) {
		perror("ERROR: recvfrom");
		exit(EXIT_FAILURE);
	}

	std::cout << "------------Response------------\n" << response << "\n----------------------------\n";

	if (close(client_socket) != 0) {
		perror("ERROR: close");
		exit(EXIT_FAILURE);
	}
}

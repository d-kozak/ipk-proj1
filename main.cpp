// regex_search example
#include <iostream>
#include <string>
#include <regex>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

static const int BUFFER_SIZE = 2048;

int regex() {

	std::string s("this subject has a submarine as a subsequence");
	std::smatch m;
	std::regex e("\\b(sub)([^ ]*)");   // matches words beginning by "sub"

	std::cout << "Target sequence: " << s << std::endl;
	std::cout << "Regular expression: /\\b(sub)([^ ]*)/" << std::endl;
	std::cout << "The following matches and submatches were found:" << std::endl;

	while (std::regex_search(s, m, e)) {
		for (auto x:m) std::cout << x << " ";
		std::cout << std::endl;
		s = m.suffix().str();
	}

	return 0;

}

int main(int argc, char **argv) {
	std::string url = "www.seznam.cz";

	std::string msg = "GET / HTTP/1.0\nHost: ";
	msg.append(url);
	msg.append("\n");
	//msg.append("Connection: close\n");
	//msg.append("Accept: */*\n");
	//msg.append("Content-Length: 0\n");
	//msg.append("Content-Type: application/x-www-form-urlencoded\n");

	/*std::string msg = "GET / HTTP/1.1\n"
			"Host: www.seznam.cz\n"
			"Accept: *\/*\n"
			"Content-Length: 0\n"
			"Content-Type: application/x-www-form-urlencoded\n";*/
	std::cout << msg;
	uint16_t port_number = 80;

	int client_socket;
	if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
		perror("ERROR: socket");
		exit(EXIT_FAILURE);
	}

	hostent * server = gethostbyname(url.c_str());
	if(server == NULL){
		fprintf(stderr,"ERROR, no such host  as %s.\n",url.c_str());
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_address;
	bzero((char*)& server_address,sizeof(server_address));
	server_address.sin_family =  AF_INET;
	bcopy((char*)server->h_addr,(char*)&server_address.sin_addr.s_addr,server->h_length);

	server_address.sin_port = htons(port_number);

	if (connect(client_socket, (const struct sockaddr *) &server_address, sizeof(server_address)) != 0) {
		perror("ERROR: connect");
		exit(EXIT_FAILURE);
	}

	ssize_t bytes_count = 0;
	//std::cout << "size: " << msg.size() << "strlen: " << strlen((msg.c_str())) << "\n";
	bytes_count = send(client_socket, msg.c_str() , msg.size(), 0);
	if (bytes_count < 0) {
		perror("ERROR: sendto");
		exit(EXIT_FAILURE);
	}

	char buffer[BUFFER_SIZE];
	memset(buffer,0,BUFFER_SIZE);
	std::string response = "";


	while((bytes_count = recv(client_socket,buffer,BUFFER_SIZE,0)) > 0) {
		std::cout << "inside while loop";
		response.append(buffer);
		memset(buffer,0,BUFFER_SIZE);
	}

	if (bytes_count < 0) {
		perror("ERROR: recvfrom");
		exit(EXIT_FAILURE);
	}

	std::cout << "------------Response------------\n" <<response << "\n----------------------------\n";

	if (close(client_socket) != 0) {
		perror("ERROR: close");
		exit(EXIT_FAILURE);
	}

	return 0;
}
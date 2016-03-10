// regex_search example
#include <iostream>
#include <string>
#include <regex>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

static const int BUFFER_SIZE = 2048;

class Parsed_url {
	std::string domain;
	std::string local_link;
	int port;

public:
	Parsed_url(){}
	Parsed_url(const std::string &domain, const std::string &local_link, int port) : domain(domain),
																					 local_link(local_link), port(port) { }
	std::string to_string(){
		return domain + " " +  local_link  + " " + std::to_string(port);
	}


	void setDomain(const std::string &domain) {
		Parsed_url::domain = domain;
	}

	void setLocal_link(const std::string &local_link) {
		Parsed_url::local_link = local_link;
	}

	void setPort(int port) {
		Parsed_url::port = port;
	}


	const std::string &getDomain() const {
		return domain;
	}

	const std::string &getLocal_link() const {
		return local_link;
	}

	int getPort() const {
		return port;
	}
};

/**
 * @brief function prints error msg and exits program with given return value
 */
void error(std::string msg, int ret_val) {
	std::cerr << msg << "\n";
	exit(ret_val);
}


/**
 * @param url - url to parse
 * @brief function parses given url and returns domain, port num and local url from domain
 */
Parsed_url parse_url(std::string url) {
	Parsed_url result;
	using namespace std;
	//http://www.fit.vutbr.cz:80/common/img/fit_logo_cz.gif?bla=1&id=5
	// some\ text -- zrusit '\ '

	// check and remove http://
	if (url.substr(0, 7).compare("http://") != 0) {
		error("Given url does not start with http://", 1);
	}
	url.erase(0, 7);

	// check whether the string contains a specific local link or if "/" will be used
	unsigned long slash_pos = url.find("/");
	if (slash_pos == string::npos) {
		return Parsed_url(url, "/", 80);
	}

	//look for port number
	string first_part = url.substr(0, slash_pos);

	unsigned long colon_pos = first_part.find(":");
	if(colon_pos == string::npos){
		// if no port is specified, 80 will be used
		result.setPort(80);
		result.setDomain(first_part);
	}else{
		// otherwise we have to parse the specific port
		string domain = first_part.substr(0, colon_pos);
		unsigned long num_len = first_part.size() - domain.size();
		result.setPort(stoi(first_part.substr(colon_pos + 1, num_len))); // + 1 to avoid the colon
		result.setDomain(domain);
	};

	unsigned long len_local_link = url.size() - first_part.size() - 1; // again we have to avoid the colon

	if(len_local_link > 0){
		// parse the local link from url
		string local_link = url.substr(slash_pos,len_local_link);

		// remove parameters like :bla=1&id=5
		unsigned long question_mark_pos = local_link.find("?");
		if(question_mark_pos != string::npos){
			local_link = local_link.substr(0,question_mark_pos);
		}

		// TODO add control for 'some\ text.txt'
		result.setLocal_link(local_link);
	} else {
		// there is no specified local link, --> use  the "/" instead
		result.setLocal_link("/");
	}

	return result;

}

std::string create_http_request(const Parsed_url & parsed_url){
	std::string message = "GET " + parsed_url.getLocal_link() + " HTTP/1.1\r\n";
	message.append("Host: " + parsed_url.getDomain() + "\r\n");
	message.append("Connection: close\r\n\r\n");
	return message;
}

int main(int argc, char **argv) {
	if(argc != 2){
		error("Wrong number of arguments, please pass only url",1);
	}
	Parsed_url parsed_url = parse_url(argv[1]);
	std::string message = create_http_request(parsed_url);
	std::cout <<  message << "\n";
	return 0;

	std::string url = "www.nic.cz";

	std::string msg = "GET / HTTP/1.1\r\n"
			"Host: www.nic.cz\r\n"
			"Connection: close\r\n\r\n";
	//msg.append(url);
	//msg.append("\n");
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

	hostent *server = gethostbyname(url.c_str());
	if (server == NULL) {
		fprintf(stderr, "ERROR, no such host  as %s.\n", url.c_str());
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_address;
	bzero((char *) &server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	bcopy((char *) server->h_addr, (char *) &server_address.sin_addr.s_addr, server->h_length);

	server_address.sin_port = htons(port_number);

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

	return 0;
}
//
// Created by david on 10.3.16.
//

#include "url_parser.h"

using namespace std;

Parsed_url::Parsed_url(){}
Parsed_url::Parsed_url(const std::string &domain, const std::string &local_link, int port) : domain(domain),
																				 local_link(local_link), port(port) { }
std::string Parsed_url::to_string(){
	return domain + " " +  local_link  + " " + std::to_string(port);
}


void Parsed_url::setDomain(const std::string &domain) {
	Parsed_url::domain = domain;
}

void Parsed_url::setLocal_link(const std::string &local_link) {
	Parsed_url::local_link = local_link;
}

void Parsed_url::setPort(int port) {
	Parsed_url::port = port;
}

int Parsed_url::getPort() const {
	return port;
}

const std::string &Parsed_url::getDomain() const {
	return Parsed_url::domain;
}

const std::string &Parsed_url::getLocal_link() const {
	return Parsed_url::local_link;
}

/**
 * @param url - url to parse
 * @brief function parses given url and returns domain, port num and local url from domain
 */
Parsed_url parse_url(std::string& url) {
	using namespace std;
	//http://www.fit.vutbr.cz:80/common/img/fit_logo_cz.gif?bla=1&id=5
	// some\ text -- zrusit '\ '

	// check and remove http://
	if (url.substr(0, 7).compare("http://") != 0) {
		std::cerr << url;
		throw BaseException("Given url " + url  + " is not valid, please check the format\n",URL_PARSE_ERROR);
	}
	url.erase(0, 7);

	// check whether the string contains a specific local link or if "/" will be used
	unsigned long slash_pos = url.find("/");
	if (slash_pos == string::npos) {
		return Parsed_url(url, "/", 80);
	}

	Parsed_url result;
	//look for port number
	string first_part = url.substr(0, slash_pos);

	unsigned long colon_pos = first_part.find(":");
	if (colon_pos == string::npos) {
		// if no port is specified, 80 will be used
		result.setPort(80);
		result.setDomain(first_part);
	} else {
		// otherwise we have to parse the specific port
		string domain = first_part.substr(0, colon_pos);
		unsigned long num_len = first_part.size() - domain.size();
		result.setPort(stoi(first_part.substr(colon_pos + 1, num_len))); // + 1 to avoid the colon
		result.setDomain(domain);
	};

	unsigned long len_local_link = url.size() - first_part.size(); // again we have to avoid the colon

	if (len_local_link > 0) {
		// parse the local link from url
		string local_link = url.substr(slash_pos, len_local_link);

		// remove parameters like :bla=1&id=5
		unsigned long question_mark_pos = local_link.find("?");
		if (question_mark_pos != string::npos) {
			local_link = local_link.substr(0, question_mark_pos);
		}

		// control for 'some\ text.txt'
		if(local_link.find(" ") != string::npos){
			string tmp;

			for(char c : local_link){
				if(c == ' ')
					tmp.append("%20");
				else
					tmp += c;
			}
			local_link = tmp;
		}
		result.setLocal_link(local_link);
	} else {
		// there is no specified local link, --> use  the "/" instead
		result.setLocal_link("/");
	}

	return result;
}

/**
 * Function parses filename from local_link into result
 * If no local_link is specified, the filename will be index.html
 */
const string parse_file_name(const string &local_link) {

	// if the last '/' is in the  'http://' , use the default name
	// TODO maybe this is not the best way, think about it :)
	if (local_link.find_last_of('/') == 6) {
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
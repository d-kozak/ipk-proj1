//
// Created by david on 10.3.16.
//

#include <stdexcept>
#include "response_parser.h"

using namespace std;

class StringTokenizer {
	vector<string> content;
	unsigned long pos;

public:

	StringTokenizer(string &content, char delim) : pos(0) {
		unsigned long end_index;
		string line;

		while ((end_index = content.find(delim)) != string::npos) {
			line = content.substr(0, end_index);
			content = content.substr(end_index + 1, content.size() - 1 - end_index);
			this->content.push_back(line);
		}
		// add last element
		this->content.push_back(content);
	}

	string get_next_token() {
		return content[pos++];
	}

	bool has_next_token() {
		return pos < content.size();
	}

	string &operator[](const unsigned int index) {
		if (index < this->content.size())
			return this->content[index];
		else
			throw invalid_argument("index out of bounds");
	}

	void print_content() {
		for (auto const &value : this->content) {
			cout << value << "\n";
		}
	}

	string to_string(){
		string s = "";
		for (auto const &value : this->content) {
			s.append(value);
		}
		return s;
	}

};

parsed_respose parse_response(string &message) {
	StringTokenizer stringTokenizer(message, '\n');


	string line = stringTokenizer.get_next_token();
	StringTokenizer parsed_line(line, ' ');

	int ret_val = stoi(parsed_line[1]);

	switch (ret_val) {
		case 200:
			cout << "200";
			{
				parsed_respose parsed_respose1{OK,stringTokenizer.to_string()};
				return parsed_respose1;
			}
			//break;
		case 301:
			cout << "301";

		case 302:
			cout << "302";
			while (stringTokenizer.has_next_token()) {
				string line = stringTokenizer.get_next_token();
				if (line.find("Location: ") != string::npos) {
					StringTokenizer parsed_line(line, ' ');
					string url = parsed_line[1].substr(0,parsed_line[1].size() - 2);

					parsed_respose parsed_respose1{REDIR,url};
					return  parsed_respose1;
				}
			}

			error("Could not find location in parsed respoonse", 11);

		case 404:
			error("404 - operation failed",12);
		default:
			error("Default in switch in parse response, should not happed", 10);
	}
}
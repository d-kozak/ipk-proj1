//
// Created by david on 10.3.16.
//

#include "response_parser.h"

using namespace std;

class StringTokenizer {
	vector<string> content;
	unsigned long pos;

public:

	StringTokenizer(string content, char delim) : pos(0) {
		unsigned long end_index;
		string line;

		while ((end_index = content.find(delim)) != string::npos) {
			line = content.substr(0, end_index - 1);
			content = content.substr(end_index + 1, content.size() - 1 - end_index);
			this->content.push_back(line);
		}
		this->print_content();
	}

	string get_next_token() {
		return content[pos++];
	}

	bool has_next_token() {
		return pos < content.size();
	}

	void print_content(){
		for (auto const &value : this->content) {
			cout << value << "\n";
		}
	}

};

response_result parse_response(string message) {
	StringTokenizer stringTokenizer(message, '\n');
	return OK;
}
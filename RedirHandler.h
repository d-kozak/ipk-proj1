//
// Created by david on 28.3.16.
//

#ifndef IPK_PROJ1_URLREDIRECTOR_H
#define IPK_PROJ1_URLREDIRECTOR_H

#include <iostream>
#include <map>
#include <fstream>
#include <string>

using namespace std;


class RedirHandler {
	static const string FILE_NAME;
	map<string,string> memory;

public:
	RedirHandler();

	bool check_for_redir(const string& url);

	string get_redirected_url(const string& original_url);

	void save_new_redirection(const string& from, const string& to);

};


#endif //IPK_PROJ1_URLREDIRECTOR_H

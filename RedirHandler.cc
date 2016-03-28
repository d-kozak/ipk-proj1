//
// Created by david on 28.3.16.
//

#include "RedirHandler.h"

const string RedirHandler::FILE_NAME = "redirections";

RedirHandler::RedirHandler() {
	std::fstream infile(this->FILE_NAME);

	if(!infile.is_open()){
		// if the file was not opened, it might be because no redirections occured
		//cerr << "Redirhandler internal error, memory file was not opened successfully" << endl;
		//cerr << "No data will be loaded" << endl;
		return;
	}

	std::string from,to;

	while(infile >> from >> to){
		this->memory[from] = to;
	}

	infile.close();
}

bool RedirHandler::check_for_redir(const std::string &url) {
	return this->memory.find(url) != this->memory.end();
}


string RedirHandler::get_redirected_url(const string &original_url) {
	string redirected_url = original_url;
	while(this->check_for_redir(redirected_url)){
		//cout << "Redirecting from " << original_url << " to " << redirected_url << endl;	
		redirected_url = this->memory[redirected_url];
	}
	return redirected_url;
}

void RedirHandler::save_new_redirection(const string &from, const string &to) {
	ofstream out(this->FILE_NAME,ios_base::app | ios_base::out);
	if(!out.is_open()){
		cerr << "Redirhandler internal error, memory file was not opened successfully, new redirection will not be saved" << endl;
		cerr << "New redirection will not be saved" << endl;
		return;
	}
	out << from << " " << to << endl;
	out.close();
}

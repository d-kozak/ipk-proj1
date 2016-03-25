//
// Created by david on 17.3.16.
//

#ifndef IPK_PROJ1_EXCEPTIONS_H
#define IPK_PROJ1_EXCEPTIONS_H

class BaseException {
	std::string msg;
	int ret_val;

public:
	BaseException(std::string msg,int ret_val) : ret_val(ret_val) {
		this->msg = "ERROR: ";
		this->msg.append(msg + "\n");
	}

	const std::string &what() {
		return this->msg;
	}

	int getRetVal(){
		return this->ret_val;
	}
};

#endif //IPK_PROJ1_EXCEPTIONS_H

//
// Created by david on 17.3.16.
//

#ifndef IPK_PROJ1_EXCEPTIONS_H
#define IPK_PROJ1_EXCEPTIONS_H

class BaseException {
};

class InvalidUrlException : public BaseException {
};

class PageNotFoundException : public BaseException {
};

class SocketHandlerInternalException : public BaseException {
};

class SocketErrorException : public BaseException {
};

#endif //IPK_PROJ1_EXCEPTIONS_H

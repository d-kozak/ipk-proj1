//
// Created by david on 10.3.16.
//

#ifndef IPK_PROJ1_SOCKET_HANDLER_H
#define IPK_PROJ1_SOCKET_HANDLER_H

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <iterator>

#include "url_parser.h"

char* communicate(const Parsed_url* parsed_url);

class PageNotFoundException{};
class SocketHandlerInternalException {};
class SocketErrorException{};

#endif //IPK_PROJ1_SOCKET_HANDLER_H

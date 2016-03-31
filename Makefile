#Makefile pro projekt do IPK  - http client

#prekladac c
CC=clang++

#mira optimalizace
OPT=-O2

#zakladni parametry prekladu
CPPFLAGS= -std=c++11 -Wall -Wextra -pedantic -g

#nazev programu
program=webclient

#seznamy vsech .c,.h a .o souboru
OBJ=webclient.o socket_handler.o url_parser.o RedirHandler.o
HEAD=webclient.h socket_handler.h url_parser.h exceptions.h globals.h RedirHandler.h
SRC=webclient.cc socket_handler.cc url_parser.cc RedirHandler.cc

.PHONY: build
.PHONY: clean
.PHONY: distrib

build: ${program}

${program}: ${OBJ}
	${CC} ${OBJ} -o ${program} ${OPT} -g

#zavislost vsech .o na .h
${OBJ}: ${HEAD}

distrib:
	tar -cvzf xkozak15.tar.gz ${HEAD} ${SRC}  Makefile
clean:
	rm -f *.o ${program}
	rm -f xkozak15.zip

#Makefile pro projekt do IPK  - http client

#prekladac c
CC=g++

#mira optimalizace
OPT=-O2

#zakladni parametry prekladu
CPPFLAGS= -std=c++11 -Wall -Wextra -pedantic -g

#nazev programu
program=webclient

#seznamy vsech .c,.h a .o souboru
OBJ=main.o socket_handler.o url_parser.o response_parser.o
HEAD=main.h socket_handler.h url_parser.h response_parser.h
SRC=main.cc socket_handler.cc url_parser.cc response_parser.cc

.PHONY: build
.PHONY: clean
.PHONY: distrib

build: ${program}

${program}: ${OBJ}
	${CC} ${OBJ} -o ${program} ${OPT}

#zavislost vsech .o na .h
${OBJ}: ${HEAD}

distrib:
	zip xkozak15.zip ${HEAD} ${SRC} Makefile

clean:
	rm -f *.o ${program}
	rm -f xkozak15.zip
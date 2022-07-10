CFLAGS=-g -Wall
CC = g++ -std=c++11
AR=ar
RANLIB=ranlib
LIBS=-L./ -lcgic $(shell pkg-config --libs --static opencv)

all:libcgic.a\
mult-get.cgi

install: libcgic.a
	cp libcgic.a /usr/local/lib
	cp cgic.h /usr/local/include
	@echo libcgic.a is in /usr/local/lib. cgic.h is in /usr/local/include.

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

#mingw32 and cygwin users: replace .cgi with .exe

mult-get.cgi: mult-get.o cJSON.o libcgic.a
	$(CC) mult-get.o cJSON.o -o ./mult-get.cgi ${LIBS}
	
.PHONY:clean
clean:
	rm ./*.cgi *.o *.a 




CFLAGS=-g -Wall
CC = g++ -std=c++11
AR=ar
RANLIB=ranlib
LIBS= zlib-master/libz.a -L./ -lcgic $(shell pkg-config --libs --static opencv)

all:libcgic.a\
slcAnalyse.cgi

install: libcgic.a
	cp libcgic.a /usr/local/lib
	cp cgic.h /usr/local/include
	@echo libcgic.a is in /usr/local/lib. cgic.h is in /usr/local/include.

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

#mingw32 and cygwin users: replace .cgi with .exe

slcAnalyse.cgi: slcAnalyse.o cJSON.o
	$(CC) slcAnalyse.o cJSON.o -o ./slcAnalyse.cgi ${LIBS}
	
.PHONY:clean
clean:
	rm ./*.cgi *.o *.a 




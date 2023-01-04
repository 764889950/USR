CFLAGS=-g -Wall
CC=gcc
AR=ar
RANLIB=ranlib
LIBS=-L./lib -L./ -lcgic

all:libcgic.a\
reset.cgi\
pressure-get.cgi\
ultrasound-get.cgi

install: libcgic.a
	cp libcgic.a /usr/local/lib
	cp cgic.h /usr/local/include
	@echo libcgic.a is in /usr/local/lib. cgic.h is in /usr/local/include.

libcgic.a: cgic.o cgic.h
	rm -f libcgic.a
	$(AR) rc libcgic.a cgic.o
	$(RANLIB) libcgic.a

#mingw32 and cygwin users: replace .cgi with .exe

reset.cgi: reset.o libcgic.a
	gcc reset.o -o ./reset.cgi ${LIBS}

pressure-get.cgi: pressure-get.o libcgic.a
	gcc pressure-get.o -o ./pressure-get.cgi ${LIBS}

ultrasound-get.cgi: ultrasound-get.o libcgic.a
	gcc ultrasound-get.o -o ./ultrasound-get.cgi ${LIBS}

.PHONY:clean
clean:
	rm ./*.cgi *.o *.a 




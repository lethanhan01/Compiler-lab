CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 
ZIP_NAME ?= LeThanhAn_20235631.zip

all: scanner

scanner: scanner.o reader.o charcode.o token.o error.o
	${CC} scanner.o reader.o charcode.o token.o error.o -o scanner

reader.o: reader.c
	${CC} ${CFLAGS} reader.c

scanner.o: scanner.c
	${CC} ${CFLAGS} scanner.c

charcode.o: charcode.c
	${CC} ${CFLAGS} charcode.c

token.o: token.c
	${CC} ${CFLAGS} token.c

error.o: error.c
	${CC} ${CFLAGS} error.c

clean:
	rm -f *.o *~ scanner scanner.exe $(ZIP_NAME) *.zip

zip: clean
	zip $(ZIP_NAME) *.c *.h Makefile

.PHONY: all clean zip


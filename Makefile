CFLAGS=-g -Wall -Wextra $(shell pkg-config --cflags hidapi-hidraw)
LDFLAGS=$(shell pkg-config --libs hidapi-hidraw)

all: via

via: main.o
	cc main.o ${LDFLAGS} -o via

main.o: main.c commands.h keycodes.h
	cc ${CFLAGS} -c main.c -o main.o

clean:
	rm -f *.o via

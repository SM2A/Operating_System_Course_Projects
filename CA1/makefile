COMPILER=gcc
SRC=Source

all: clean client.out server.out

client.out: ${SRC}/client.c ${SRC}/structure.h
	${COMPILER} ${SRC}/client.c -o $@
server.out: ${SRC}/client.c ${SRC}/structure.h
	${COMPILER} ${SRC}/server.c -o $@

clean:
	rm -f *.out
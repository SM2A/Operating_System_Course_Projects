COMPILER = g++
VERSION = -std=c++11
CC = ${COMPILER} ${VERSION}

all: clean serial

serial : readImg.cpp
	${CC} readImg.cpp -o ImageFilters.out

clean:
	rm -f ImageFilters.out

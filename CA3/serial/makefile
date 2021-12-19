COMPILER = g++
VERSION = -std=c++11
OPTION = -w
CC = ${COMPILER} ${VERSION} ${OPTION}


all: clean serial

serial : readImg.cpp
	${CC} readImg.cpp -o ImageFilters.out

clean:
	rm -f ImageFilters.out

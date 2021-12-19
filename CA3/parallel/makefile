COMPILER = g++
VERSION = -std=c++11
OPTION = -pthread
CC = ${COMPILER} ${VERSION} ${OPTION}

all: clean parallel

parallel : readImg.cpp
	${CC} readImg.cpp -o ImageFilters.out

clean:
	rm -f ImageFilters.out

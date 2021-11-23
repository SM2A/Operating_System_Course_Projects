COMPILER = g++
OPTION = -w
CC = ${COMPILER}

SRC_DIR=Source
BUILD_DIR=Build

all: main map reduce

main: ${SRC_DIR}/main_process.cpp
	mkdir -p ${BUILD_DIR}
	${CC} ${SRC_DIR}/main_process.cpp -o $@

map: ${SRC_DIR}/map_process.cpp
	${CC} ${SRC_DIR}/map_process.cpp -o ${BUILD_DIR}/$@

reduce: ${SRC_DIR}/reduce_process.cpp
	${CC} ${SRC_DIR}/reduce_process.cpp -o ${BUILD_DIR}/$@

clean:
	rm -f main
	rm -rfv ${BUILD_DIR}
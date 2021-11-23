COMPILER = g++
COMPILE = -c
CF = -pedantic
CC = ${COMPILER} ${COMPILE} ${CF}

SOURCES := ${shell find * -type f -name "*.cpp"}
OBJDIR  := Build
OBJECTS := ${addprefix ${OBJDIR}/,${SOURCES:.cpp=.o}}

all: main.out

main.out: ${OBJECTS}
	${COMPILER} ${CF} $^ -o $@

${OBJECTS}: ${OBJDIR}/%.o: %.cpp
	mkdir -p ${@D}
	${CC} $< -o $@

-include $(OBJECTS:.o=.d)

clean:
	rm main.out
	rm -rfv Build
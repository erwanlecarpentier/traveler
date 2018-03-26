CCC=g++
INCLUDE=-I./src
CCFLAGS=-std=c++11 -Wall -Wextra ${INCLUDE} -O2 -g
LDFLAGS=-s -lm -lconfig++
EXEC=exe

all : clean compile run

clean :
	rm -f ${EXEC}

clear :
	clear

compile : clear clear demo/main.cpp
	${CCC} ${CCFLAGS} demo/main.cpp -o ${EXEC} ${LDFLAGS}

run :
	./${EXEC}


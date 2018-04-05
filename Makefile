CCC=g++
INCLUDE=-I./src -I./src/utils -I./src/environment -I./src/policy -I./src/policy/mcts -I./src/policy/tmp_mcts
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


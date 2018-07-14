CCC=g++
INCLUDE=-I./demo -I./src -I./src/utils -I./src/environment -I./src/policy -I./src/policy/mcts -I./src/policy/tmp_mcts
CCFLAGS=-std=c++11 -Wall -Wextra ${INCLUDE} -g -O2
LDFLAGS=-lm -lpthread -lconfig++ -s
EXEC=exe

CFGPATH?=config/parameters.cfg
export CFGPATH

all : clean compile run

clean :
	rm -f ${EXEC}

compile : demo/main.cpp
	${CCC} ${CCFLAGS} demo/main.cpp -o ${EXEC} ${LDFLAGS}

run :
	./${EXEC} ${CFGPATH}


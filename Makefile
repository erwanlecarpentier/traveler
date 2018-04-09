CCC=g++
INCLUDE=-I./src -I./src/utils -I./src/environment -I./src/policy -I./src/policy/mcts -I./src/policy/tmp_mcts
#INCLUDESP=-I/opt/DMIA/EIGEN/eigen/include/eigen3 -I/opt/DMIA/EIGEN/libconfig/include -L/opt/DMIA/EIGEN/libconfig/lib#serv-prol1
CCFLAGS=-std=c++11 -Wall -Wextra ${INCLUDE} -O2 -g
#CCFLAGS=-std=c++11 -Wall -Wextra ${INCLUDE} ${INCLUDESP} -O2 -g#serv-prol1
LDFLAGS=-s -lm -lconfig++
EXEC=exe
CFGPATH?=config/parameters.cfg
export CFGPATH

all : clean compile run

clean :
	rm -f ${EXEC}

clear :
	clear

compile : clear clear demo/main.cpp
	${CCC} ${CCFLAGS} demo/main.cpp -o ${EXEC} ${LDFLAGS}

run :
	./${EXEC} ${CFGPATH}


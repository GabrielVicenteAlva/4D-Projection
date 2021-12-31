CC=g++

SRC=./
BIN=./vsr_bin

OBJ=vsr.o
LIBRARY=vsr.a

PROJECT=projection4d
DEF=VSR_PRECISION_DOUBLE

main: main.cpp $(PROJECT).o
	g++ -o a.out main.cpp $(PROJECT).o -lGL -lGLU -lglut -I $(SRC) -D $(DEF)

$(PROJECT).o: $(PROJECT).cpp
	g++ -o $(PROJECT).o -c $(PROJECT).cpp -I $(SRC) -D $(DEF)

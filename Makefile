CXXFLAGS= -std=c++11 -Wall -pedantic -pedantic-errors

todos= inicializador terminador trenNS trenSN coordinador

all:$(todos)

clean:
		rm -f $(todos)
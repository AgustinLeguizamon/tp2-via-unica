CXXFLAGS= -std=c++11 -Wall -pedantic -pedantic-errors

todos= inicializador terminador trenNS trenSN

all:$(todos)

clean:
		rm -f $(todos)
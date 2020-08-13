CC = g++
CFLAGS = -Wall -Werror -std=c++17

default: all

all: main main-opt

main: main.cpp maze.h mazecell.h
	${CC} ${CFLAGS} -g main.cpp maze.h mazecell.h -o main

main-opt: main.cpp maze.h mazecell.h
	${CC} ${CFLAGS} main.cpp maze.h mazecell.h -o main-opt -O3
#***************************************************************************
#
#   File        : makefile
#   Student Id  : 757931
#   Name        : Sebastian Baker
#
#***************************************************************************


CC=gcc
CFLAGS=-Wall # -Werror #-g -v -o0
OUT=bin/flow
SRC=src/linkedlist.o src/data_handler.o src/tasks.o src/main.o

all: clean compile

compile: $(SRC)
	$(CC) $(SRC) $(CFLAGS) -o $(OUT);

clean:
	rm -f src/*.o;

run: clean compile
	bin/flow data/flow_data_50k.csv 128;

val: clean compile
	valgrind --leak-check=full --track-origins=yes bin/flow data/flow_data.csv 24;

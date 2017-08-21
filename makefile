#***************************************************************************
#
#   File        : makefile
#   Student Id  : 757931
#   Name        : Sebastian Baker
#
#***************************************************************************


CC=gcc
CFLAGS=-Wall -Werror
OUT=bin/flow
SRC=src/data_handler.o src/tasks.o src/main.o

all: clean compile clean

compile: $(SRC)
	$(CC) $(SRC) $(CFLAGS) -o $(OUT);

clean:
	rm -f src/*.o;

run: clean compile clean
	bin/flow data/flow_data.csv 128;

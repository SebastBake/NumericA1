#***************************************************************************
#
#   File        : makefile
#   Student Id  : 757931
#   Name        : Sebastian Baker
#
#***************************************************************************


CC=gcc
CFLAGS=-Wall -Werror -g
OUT=bin/flow
SRC=src/data_handler.o src/tasks.o src/main.o

all: clean compile

compile: $(SRC)
	$(CC) $(SRC) $(CFLAGS) -o $(OUT);

clean:
	rm -f src/*.o;

run: clean
	bin/flow data/flow_data.csv 128;

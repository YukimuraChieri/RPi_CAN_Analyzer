SRC = ./src
INC = ./inc
OBJ = ./build/obj
BIN = ./build/bin
TARGET = $(BIN)/CAN_Analyzer

CC = gcc
CFLAGS = -Wall -O -g
INCLUDE = -I $(INC)

TARGET: $(OBJ)/main.o
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)/main.o

$(OBJ)/main.o: $(SRC)/main.c
	$(CC) $(CFLAGS) -c -o $(OBJ)/main.o $(SRC)/main.c

.PHONY: clean

clean:
	rm $(OBJ)/* $(BIN)/*



SRC = ./src
INC = ./inc
OBJ = ./build/obj
BIN = ./build/bin
TARGET = $(BIN)/CAN_Analyzer

CC = gcc
CFLAGS = -Wall -O -g
INCLUDE = -I $(INC)

TARGET: $(OBJ)/main.o $(OBJ)/UDPService.o $(OBJ)/CANService.o $(OBJ)/crcLib.o
	$(CC) $(CFLAGS) -o $(TARGET) $^ -lpthread

$(OBJ)/main.o: $(SRC)/main.c
	$(CC) $(CFLAGS) -c -o $(OBJ)/main.o $(SRC)/main.c $(INCLUDE)

$(OBJ)/UDPService.o: $(SRC)/UDPService.c $(INC)/UDPService.h
	$(CC) $(CFLAGS) -c -o $(OBJ)/UDPService.o $(SRC)/UDPService.c $(INCLUDE)

$(OBJ)/CANService.o: $(SRC)/CANService.c $(INC)/CANService.h
	$(CC) $(CFLAGS) -c -o $(OBJ)/CANService.o $(SRC)/CANService.c $(INCLUDE)

$(OBJ)/crcLib.o: $(SRC)/crcLib.c $(INC)/crcLib.h
	$(CC) $(CFLAGS) -c -o $(OBJ)/crcLib.o $(SRC)/crcLib.c $(INCLUDE)

.PHONY: clean

clean:
	rm $(OBJ)/* $(BIN)/*



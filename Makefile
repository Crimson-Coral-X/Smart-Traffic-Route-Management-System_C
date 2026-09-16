
CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -Wpedantic -O2
TARGET=traffic_engine
OBJ=main.o graph.o algorithms.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c graph.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) $(TARGET).exe

.PHONY: all clean

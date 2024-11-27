# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -Isrc
LDFLAGS = -lGL -lGLU -lGLEW -lglfw -lm
LIBS = -lGL -lGLU -lGLEW -lglfw -lm

SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
BIN = bin/voxel_viewer

all: $(BIN)

$(BIN): $(OBJ)
	mkdir -p bin
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build/* bin/*

.PHONY: all clean

CC = gcc
CFLAGS = -Iincludes -Wall

TARGET = build/arena_allocator.exe

build:
	mkdir build
	$(CC) $(CFLAGS) src/arena.c src/main.c -o $(TARGET)

clean:
	rmdir /s /q build

run: clean build
	$(TARGET)
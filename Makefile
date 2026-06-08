CC = gcc
FLAGS = -Wall -pedantic -lpthread

all: main

main:
	$(CC) src/main.c -o main $(FLAGS)

clean:
	find . -type f -executable -not -name "*.c" -not -name "Makefile" -delete

run: all
	@echo "Running simulation:" > log.txt
	@echo >> log.txt
	./main >> log.txt
	@echo >> log.txt
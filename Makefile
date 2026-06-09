CC = gcc
FLAGS = -Wall -pedantic -lpthread
SOURCES = src/main.c src/priority.c src/lottery.c

all: main

main:
	$(CC) $(SOURCES) -o main $(FLAGS)

clean:
	find . -type f -executable -not -name "*.c" -not -name "Makefile" -delete

run: all
	@echo "Running simulation:" > log.txt
	@echo >> log.txt
	./main >> log.txt
	@echo >> log.txt
CC = gcc
TARGETDIR := ./target

day01.c:
	$(CC) -ggdb -o $(TARGETDIR)/day01 ./src/day01/main.c

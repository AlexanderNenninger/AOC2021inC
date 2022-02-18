CC = gcc
TARGETDIR := ./target
LIB := ./target/lib/

project: day25

day01.c:
	$(CC) -ggdb -o $(TARGETDIR)/day01 ./src/day01/main.c

day05.c:
	$(CC) -ggdb -c -o $(TARGETDIR)/day05.o ./src/day05/main.c

hashmap.c:
	$(CC) -ggdb -c -o $(LIB)/libhashmap.a src/lib/hashmap/hashmap.c

day05: hashmap.c day05.c
	$(CC) -ggdb $(TARGETDIR)/day05.o -L$(LIB) -lhashmap -o $(TARGETDIR)/day05

day25:
	$(CC) -ggdb -o $(TARGETDIR)/day25 ./src/day25/main.c

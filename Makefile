CC = gcc
TARGETDIR := ./target
LIB := ./target/lib/
OPT := O3

project: day25 day05 day01

day01:
	$(CC) -ggdb -${OPT} -o $(TARGETDIR)/day01 ./src/day01/main.c

day05.c:
	$(CC) -ggdb -${OPT} -c -o $(TARGETDIR)/day05.o ./src/day05/main.c

hashmap.c:
	$(CC) -ggdb -${OPT} -c -o $(LIB)/libhashmap.a src/lib/hashmap/hashmap.c

day05: hashmap.c day05.c
	$(CC) -ggdb -${OPT} $(TARGETDIR)/day05.o -L$(LIB) -lhashmap -o $(TARGETDIR)/day05

day25:
	$(CC) -ggdb -${OPT} -o $(TARGETDIR)/day25 ./src/day25/main.c

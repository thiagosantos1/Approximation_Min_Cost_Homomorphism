CC     = gcc
HDR1   = ../include/graph.h
OPT    = -g -Wall
CFLAGS = -c -I../include 
OBJS   = makegraph.o greedy.o utils.o bipartite.o

all: main

main: main.o
	$(CC) -o ../bin$@ main.o
main.o: main.c makegraph.c $(HDR1)
	$(CC) $(CFLAGS) $(OPT) main.c
makegraph.o: makegraph.c $(HDR1)
	$(CC) $(CFLAGS) $(OPT) makegraph.c

clean:
	rm -f *.o ../bin/main

run: main
	./main ../etc/graph_g.txt ../etc/graph_h.txt ../etc/list_cost.txt ../etc/list_homom.txt ../etc/list_pairs.txt

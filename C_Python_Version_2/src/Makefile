CC     = gcc
HDR1   = ../include/graph.h
OPT    = -g -Wall 
CFLAGS = -c -I../include
LIBS   = -lglpk
OBJS   = makegraph.o greedy.o utils.o bipartite.o 

all: LP_run

LP_run: main.o makegraph.o pair_consistency.o LP_Min_Cost.o
	$(CC) $(OPT) -o ../bin/$@ main.o makegraph.o pair_consistency.o LP_Min_Cost.o $(LIBS)

main.o: main.c $(HDR1)
	$(CC) $(CFLAGS) $(OPT) main.c
makegraph.o: makegraph.c $(HDR1)
	$(CC) $(CFLAGS) $(OPT) makegraph.c
pair_consistency.o: pair_consistency.c $(HDR1)
	$(CC) $(CFLAGS) $(OPT) pair_consistency.c
LP_Min_Cost.o: LP_Min_Cost.c $(HDR1)
	$(CC) $(CFLAGS) $(OPT) LP_Min_Cost.c

clean:
	rm -f *.o ../bin/

data:
	python3 pre_process_data_digraph.py ../etc/graph_g.txt ../etc/graph_h.txt

list:
	python3 graph_consistency.py ../etc/graph_g.txt ../etc/graph_h.txt ../etc/full_list.txt 2

flat_g:
	python3 flat_graph.py ../etc/graph_g.txt

run:
	../bin/./LP_run ../etc/graph_g.txt ../etc/graph_h.txt ../etc/list_cost.txt ../etc/list_homom.txt ../etc/list_pairs.txt
	

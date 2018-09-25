#!/bin/sh
#chmod u+x

ARGC=$#
BIP=2
 
EXPECTED_ARGS=5
E_BADARGS=65

if [ $# -lt $EXPECTED_ARGS ]
then
  echo "Usage: `basename $0` {argv}. <pair_consistency>, <num_vertices G>, <num_of_partitions>, <is_bipartite> &<num_of_graphs_G>"
  exit $E_BADARGS
fi

PAIR_FLAG="$1"
NUM_VERT_G="$2" 
NUM_PART="$3"
is_bipartite="$4"
NUM_GRAPH_G="$5"

# make sure to recompile if needed
make

# loop for each H, create different Gs 
for (( g=1; g<=$NUM_GRAPH_G; g++ ))
do  # create G 
  if [ $is_bipartite -lt $BIP ] 
  then
		python3 create_bip_graph.py $NUM_VERT_G $NUM_PART "graph_g"
	else
		python3 create_rand_graph.py $NUM_VERT_G $NUM_PART "graph_g"
	fi
	# pre-process data and run list reduction
	make data
	make list

	# run program for continuos
	../bin/./LP_run ../etc/graph_g.txt ../etc/graph_h.txt ../etc/list_cost.txt ../etc/list_homom.txt ../etc/list_pairs.txt $PAIR_FLAG 1

	# run program for integral
	../bin/./LP_run ../etc/graph_g.txt ../etc/graph_h.txt ../etc/list_cost.txt ../etc/list_homom.txt ../etc/list_pairs.txt $PAIR_FLAG 2

done


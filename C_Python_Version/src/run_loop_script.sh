#!/bin/sh
#chmod u+x

ARGC=$#
BIP=2

EXPECTED_ARGS=7
E_BADARGS=65

if [ $# -lt $EXPECTED_ARGS ]
then
  echo "Usage: `basename $0` {argv}. <pair_consistency>, <num_vertices G>, <num_vertices H>, <num_of_partitions>, <is_bipartite>, <num of graphs H> & <Loop repetitions G>"
  exit $E_BADARGS
fi

PAIR_FLAG="$1"
NUM_VERT_G="$2"
NUM_VERT_H="$3" 
NUM_PART="$4"
is_bipartite="$5"
NUM_GRAPHS_H="$6"
NUM_REP_LOOP="$7"

# make sure to recompile if needed
make

#loop for how many graphs H you wanna create
for (( h=1; h<=$NUM_GRAPHS_H; h++ ))
do
	# create H
	if [ $is_bipartite -lt $BIP ] 
  then
		python3 create_bip_graph_h.py $NUM_VERT_H $NUM_PART "graph_h"
	else
		python3 create_rand_graph.py $NUM_VERT_H $NUM_PART "graph_h"
	fi

  # loop for each H, create different Gs 
  for (( g=1; g<=$NUM_REP_LOOP; g++ ))
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

done





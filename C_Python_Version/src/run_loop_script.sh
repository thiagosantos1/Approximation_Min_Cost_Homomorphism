#!/bin/sh

ARGC=$#

EXPECTED_ARGS=6
E_BADARGS=65

if [ $# -lt $EXPECTED_ARGS ]
then
  echo "Usage: `basename $0` {argv}. <pair_consistency>, <type_solution>, <num_vertices G>, <num_vertices H>, 
  									 <num_of_partitions> & <Loop repetitions>"
  exit $E_BADARGS
fi

PAIR_FLAG="$1"
TYPE_SOLU_FLAG="$2"
NUM_VERT_G="$3"
NUM_VERT_H="$4" 
NUM_PART="$5"
NUM_REP_LOOP="$6"

echo $PAIR_FLAG
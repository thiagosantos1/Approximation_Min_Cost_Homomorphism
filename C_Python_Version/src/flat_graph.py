# Author: Thiago Santos

# program to flat a graph. Then, we aren't gonna care for partitions anymore
# This is to help the lp, make it faster
# the idea is that we will create a graph with size = size * 2
# if ij is an edge, i is gonna be connected to j'

import random
import sys
import os 

def flat_graph(graph):

  try:
    graph_file = open(graph, 'r')
  except IOError as exc:
    print("Failure opening file " + str(graph) )
    sys.exit(2)

  line = graph_file.readline()
  line = line.split()
  num_vertices, num_partition = int(line[0]), int(line[2])

  base = num_vertices
  # doblet it, to create bipartite
  num_vertices *= 2

  lines = graph_file.readlines()
  graph_file.close()
  new_graph = []
  for line in lines:
    xy = line.split(' ')  
    x,y = int(xy[0]), int(xy[1])
    new_graph.append([x,y+base])

  for pair in new_graph:
    print(pair)


if __name__ == '__main__':

  if len(sys.argv) < 2:
    print("Please provide <graph>\n")
    sys.exit(1)

  flat_graph(sys.argv[1])
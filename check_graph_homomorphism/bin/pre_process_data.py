# Used to pre-process the data.
# Get's a file with an edge list, and creates a new cost list, with randowm cost of maping 
import os # to get full directory
import random
import sys

def process_data(graph_g, graph_h):
  sub_path = "/../../LP_LORA/bin/version_1/" # to save the file in the LP folder automatic
  abs_path = os.getcwd()
  path = abs_path + sub_path + "list_cost.mat"
  min_rand = 5
  max_rand = 300
  # for G first
  try:
    graph_file = open(graph_g, 'r')
  except IOError as exc:
    print("Failure opening file " + str(graph_g) )
    sys.exit(2)

  num_vertices = int(graph_file.readline())
  left_g = []
  right_g = []

  lines = graph_file.readlines()
  for line in lines:
    xy = line.split(' ')  

    x = int(xy[0])
    y = int(xy[1])
    if x not in left_g:
    	left_g.append(x)

    if y not in right_g:
    	right_g.append(y)

  # for H
  try:
    graph_file = open(graph_h, 'r')
  except IOError as exc:
    print("Failure opening file " + str(graph_h) )
    sys.exit(2)

  num_vertices = int(graph_file.readline())
  left_h = []
  right_h = []
  
  lines = graph_file.readlines()
  for line in lines:
    xy = line.split(' ')  

    x = int(xy[0])
    y = int(xy[1])
    if x not in left_h:
      left_h.append(x)

    if y not in right_h:
      right_h.append(y)
  
  list_output = ""
  try:
    list_output = open(path, 'w')
  except IOError as exc:
    print("Failure opening file " + str(graph_g) + " to write")
    sys.exit(2)

  try:
    list_out = open("list.txt", 'w')
  except IOError as exc:
    print("Failure opening file " + str(graph_h) + " to write")
    sys.exit(2)

  left_g = sorted(left_g)
  left_h = sorted(left_h)
  right_g = sorted(right_g)
  right_h = sorted(right_h)

  # for G
  rand_cost = 0
  for elem_g in left_g:
    for elem_h in left_h:
      rand_cost = random.randint(min_rand, max_rand)
      list_output.write(str(elem_g) + " " + str(elem_h) + " " + str(rand_cost) + "\n")
      list_out.write(str(elem_h+1) + " ")
    list_out.write("\n")

  # for H
  rand_cost = 0
  size_g = 0
  size_h = 0
  for elem_g in right_g:
    size_g +=1
    size_h = 0
    for elem_h in right_h:
      size_h +=1
      rand_cost = random.randint(min_rand, max_rand)
      list_output.write(str(elem_g) + " " + str(elem_h) + " " + str(rand_cost))

      if size_g < len(right_g):
        list_output.write("\n")
      elif size_h < len(right_h):
        list_output.write("\n")

      list_out.write(str(elem_h+1) + " ")
    list_out.write("\n")



if len(sys.argv) < 3:
  print("Please provide <FileGraph_G>, <FileGraph_H> \n")
  sys.exit(1)

file_g = sys.argv[1]
file_h = sys.argv[2]
print("\t*** Starting creating full list homomophism ***")
process_data(file_g, file_h)
print("\t*** List homomophism created ***")
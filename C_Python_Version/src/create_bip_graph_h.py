import random
import sys
import os # to get full directory

# program to create a random bipirtite graph h, with N vertices
# in the promp, the user defines N, as well the percentage of vertices in the left of graph

def make_rand_bip_graph(num_vertices, num_partition, output_name):

	# to save in the octave folder
	sub_path = "/../etc/" # to save the file in the LP folder automatic
	abs_path = os.getcwd()
	path = abs_path + sub_path + output_name + ".txt" 

	try:
		list_output = open(path, 'w')
	except IOError as exc:
		print("Failure opening file " + str(output_name) + " to write")
		sys.exit(2)

	vertices = [x for x in range(num_vertices)]
	partitions = [[] for x in range(num_partition)]
	max_num_vertices = num_vertices - num_partition
	matrix_graph = [[0 for x in range(num_vertices)] for y in range(num_vertices)]
	
	# create partition
	for x in range(num_partition-1):
		num_vertices_partition = random.randint(1, max_num_vertices)
		
		if x ==0:
			partitions[x] = [y for y in range(num_vertices_partition)]
		else:
			last = partitions[x-1][len(partitions[x-1])-1]
			partitions[x] = [y for y in range(last+1,last+num_vertices_partition+1)]
		max_num_vertices = num_vertices - len(partitions[x]) -1

	# complete the last partion
	last_index = num_partition-2
	last = partitions[last_index][len(partitions[last_index])-1]
	partitions[last_index+1] = [y for y in range(last+1,num_vertices)]
	list_output.write(str(num_vertices) + "\n") 

	for x in range(0, len(partitions) -1):
		bi_partition_vertices = partitions[x] # vertices
		bi_partition_edges_vertices = partitions[x+1] # to connect with edges

		for vertice in bi_partition_vertices:
			max_degre = len(bi_partition_edges_vertices)
			if max_degre > 1:
				max_degre = max_degre#//2

			rand_num_edges = random.randint(1, max_degre)
			num_edges = 0
			edge_verti = 0

			while num_edges< rand_num_edges: # untill all edges of vertice has not been choosen, keep addind
				index = random.randint(0, len(bi_partition_edges_vertices)-1)
				edge_verti = bi_partition_edges_vertices[index]
				if matrix_graph[vertice][edge_verti]==0:
					matrix_graph[vertice][edge_verti] = 1
					num_edges +=1

			# for edge in edg_list:
			# 	list_output.write(str(vertice) + " " + str(edge))
			# 	list_output_octave.write(str(vertice) + " " + str(edge))
			# 	if vertice != partitions[len(partitions)-2][len(partitions[len(partitions)-2])-1]  or edge != edg_list[len(edg_list)-1]:
			# 		list_output.write("\n")
			# 		list_output_octave.write("\n")
		
		# check for min ordering consistency. If 2 edges cross, we need the X bar property.
		# Thus, we should add it
		# for each new level created, we need to check for this consistency
		for i in bi_partition_vertices:
			for j in bi_partition_edges_vertices:
				for i_ in range(i+1, len(bi_partition_vertices)):
					for j_ in range(bi_partition_edges_vertices[0], j):
						if matrix_graph[i_][j_] == 1 and matrix_graph[i][j] == 1:
							matrix_graph[i][j_] = 1

	for i in range(0,len(matrix_graph)):
		for j in range(0,len(matrix_graph)):
			if matrix_graph[i][j] == 1:
				list_output.write(str(i) + " " + str(j) + "\n")

if len(sys.argv) < 4:
  print("Please provide <num_vertices>, <num_of_partition(>=2)> & <output_name>\n")
  sys.exit(1)

num_vertices = int(sys.argv[1])
num_partition = int(sys.argv[2])
output_name = sys.argv[3]
if num_partition < 2:
	print("num_of_partition must be at least 2")
	sys.exit(2)

if num_vertices < num_partition:
	print("Please provide <num_vertices> >= <num_of_partition(>=2)\n")
	sys.exit(1)

make_rand_bip_graph(num_vertices, num_partition, output_name)
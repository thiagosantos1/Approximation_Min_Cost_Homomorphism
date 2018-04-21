import random
import sys
import os # to get full directory

# program to create a random bipirtite graph, with N vertices
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
	print("partition: ",partitions)
	for x in range(0, len(partitions) -1):
		bi_partition_vertices = []
		bi_partition_vertices.extend(partitions[x]) # vertices
		bi_partition_edges_vertices = []
		bi_partition_edges_vertices.extend(partitions[x+1]) # to connect with edges
		if x < len(partitions) -2:
			bi_partition_edges_vertices.extend(partitions[x+2]) 

		for vertice in bi_partition_vertices:
			print(x,len(bi_partition_edges_vertices))
			rand_num_edges = random.randint(1, len(bi_partition_edges_vertices))
			edg_list = []

			while len(edg_list) < rand_num_edges: # untill all edges of vertice has not been choosen, keep addind
				index = random.randint(0, len(bi_partition_edges_vertices)-1)
				edge_verti = bi_partition_edges_vertices[index]
				if edge_verti not in edg_list:
					edg_list.append(edge_verti)
			edg_list.sort()

			for edge in edg_list:
				list_output.write(str(vertice) + " " + str(edge))
				if vertice != partitions[len(partitions)-2][len(partitions[len(partitions)-2])-1]  or edge != edg_list[len(edg_list)-1]:
					list_output.write("\n")
		
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
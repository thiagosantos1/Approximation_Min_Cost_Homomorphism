# Author: Thiago Santos
# clash designed to create or get a hash, based on 2 integers
# it's used to create a key for a pair in the graph
# let N be the number of vertices, the hash = (x * N) + y
# to get the x and y based or key, do --> x = key//N   --> y = key - (x*N)

def create_hash(x,y, N):
	key = (x*N) + y

	if x>N or y>N:
		key = -1 # error
	
	return key


def get_x_y_hash(key, N):

	temp = key//N

	if temp * N == key: # means you don't have mod - then, it means y is the last node = N
		x = temp -1
	else:
		x = temp

	y = key - (x*N)

	return [x,y]

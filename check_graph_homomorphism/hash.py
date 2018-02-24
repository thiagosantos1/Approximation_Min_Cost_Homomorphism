# Author: Thiago Santos
# clash designed to create or get a hash, based on 2 integers
# it's used to create a key for a pair in the graph
# let N be the number of vertices, the hash = (x * N) + y
# to get the x and y based or key, do --> x = key//N   --> y = key - (x*N)

def create_hash(x,y, N):
	try:
		key = (x*N) + y

		if x>N or y>N:
			key = -1 # error
	except IOError as exc:
		print("Failure making hash")
		sys.exit(2)

	return key


def get_x_y_hash(key, N):
	try:
		temp = key//N

		if temp * N == key and key!=0: # means you don't have mod - then, it means y is the last node = N
			x = temp -1
		else:
			x = temp

		y = key - (x*N)

	except IOError as exc:
		print("Failure getting hash")
		sys.exit(2)

	return [x,y]

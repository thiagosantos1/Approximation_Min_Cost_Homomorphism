# Author: Thiago Santos
# Program disegned to check consistency between 2 digraphs, ir order to find if they are homomorphic or not
# it's given 3 files. Digraph G and Digraph H and a List T with the list of vertices on H that each vertex in G can match with it

import random
import sys

class Vertex:
  def __init__(self, n):
    self.name = n
    self.neighbors = {} # dictionary to save neighbors
    self.color = 'black'
  
  def add_neighbor(self, v):
    for key,value in self.neighbors.items():
      if v == key: # if neighbor already added
        return False
    self.neighbors[v] = 1 

class Graph:
  
  def __init__(self, file_name):
    # Variables for each instance of Graph
    self.vertices = {} # dictionary for all vertexes
    self.list_match_G_H = {} # save for each vertex of G, a list of possible vertexes that it may be matched on H
    self.list_pairs = {} # create a list of all pairs based on the reduced list_G_H. Used for path consitency
    self.graph_name = file_name

    self.init_graph(file_name)

  def add_vertex(self, vertex): # add a new vertex to the graph
    if isinstance(vertex, Vertex) and vertex.name not in self.vertices:
      self.vertices[vertex.name] = vertex
      return True
    else:
      return False
  
  def add_edge(self, u,v): # edge from u to v
    if u in self.vertices and v in self.vertices:
      for key,value in self.vertices.items():
        if key == u:
          value.add_neighbor(v)
          return True
    else:
      return False
  
  def add_vertex_list(self, u,v): #add a new vertex v from H that vertex u can be matched
    if u in self.vertices:
      for key,value in self.vertices.items():
        if key == u:
          self.list_match_G_H[u].append(v)
          return True
    else:
      return False
  
  def init_graph(self,file_name):

    try:
      graph_file = open(file_name, 'r')
    except IOError as exc:
      print("Failure opening file " + str(file_name) )
      sys.exit(2)

    lines = graph_file.readlines()
    for x in range(len(lines)):
      self.add_vertex(Vertex(x))

    vertex = 0
    for line in lines:
      edge = 0
      for u in line:
          if u != '\n' and u != ' ':
            if int(u) ==1:
              self.add_edge(vertex,edge)
            edge +=1

      vertex +=1

  def init_list(self,file_name):

    try:
      graph_file = open(file_name, 'r')
    except IOError as exc:
      print("Failure opening file " + str(file_name) )
      sys.exit(2)

    lines = graph_file.readlines()
    if len(lines) > len(self.vertices): # means you have more elements than vertices
      print("List " + str(file_name) + " Cannot have more elements than Graph G" )
      sys.exit(2)

    elif len(lines) < len(self.vertices): # means you have more elements than vertices
      print("There it not Homomorphism\n")
      sys.exit(2)

    for x in range(len(lines)):
      self.list_match_G_H[x] = [] 

    vertex = 0
    for line in lines:
      for v in line:
          if v != '\n' and v != ' ':
            self.add_vertex_list(vertex, v)

      vertex +=1

  def arc_consistency(self,graph_h):
    done = False
    stop = True
    while not done:
      done = True
      for x in range(len(self.vertices) ): # for all vertices x, do the domain reduction against all arcs with x
        # for all neighbors of x that has't been checked
        arcs = (y for y,value in self.vertices[x].neighbors.items() )
        for y in arcs:
          stop = self.reduction(x,y,graph_h)
          if not stop:
            done = False

  def reduction(self, x, y, graph_h):
    eliminate_a = True # set to false if a has a connection if list of b based on Graph H
    eliminate_b = True
    done = True
    a_lis = ""
    b_lis = ""
    for a_lis in self.list_match_G_H[x]: # for all elem in the list(x) of possibe match 
      for b_lis in self.list_match_G_H[y]:
        if self.isThereArc(a_lis,b_lis,graph_h): # if a and b is an arc in h
          eliminate_a = False
          break;

      if eliminate_a: # if did not find an arc, reduce a from list of x
        self.list_match_G_H[x].remove(a_lis)
        done = False
      eliminate_a = True


    for b_lis in self.list_match_G_H[y]: # for all elem in the list(x) of possibe match 
      for a_lis in self.list_match_G_H[x]:
        if self.isThereArc(a_lis,b_lis,graph_h): # if a and b is an arc in h
          eliminate_b = False
          break;

      if eliminate_b: # if did not find an arc, reduce a from list of x
        self.list_match_G_H[y].remove(b_lis)
        done = False
      eliminate_b = True

    
    return done
      
  def isThereArc(self,x,y,graph_h):
    return True if int(y) in graph_h.vertices[int(x)].neighbors else False
  
  def make_pairs(self,graph_h):

    for x in range(len(self.vertices) ): # for all vertices x
      self.make_pairs_xx(int(x)) # create all pairs aa | a e L(x)
      # for all neighbors of x that has't been checked
      arcs = [y for y,value in self.vertices[x].neighbors.items() ]
      non_arcs = [b for b,value in self.vertices.items() if b not in arcs and 
                                                            x not in self.vertices[b].neighbors and
                                                            b!=x]

      for y in arcs:
        self.make_pairs_xy(int(x), int(y), graph_h, True)

      for y in non_arcs:
        self.make_pairs_xy(int(x), int(y), graph_h, False)

    self.copy_reverse()

  # make pairs (a,a) for all values in the list of x
  def make_pairs_xx(self, x):
    key = str(x)+str(x); # create a key for the dictionary
    for value in self.list_match_G_H[x]:

      xy = str(value)+str(value); # create the pair

      if key not in self.list_pairs: #create key and a set inside
        self.list_pairs[key] = set()

      self.list_pairs[key].add(xy)

  # make all possible pairs a,b | a e L(x) & b e L(y) & ab e A(H)
  def make_pairs_xy(self, x, y, graph_h, arc_need):
    key = str(x)+str(y);
    for x_pair in self.list_match_G_H[x]:
      for y_pair in self.list_match_G_H[y]:
        if self.isThereArc(x_pair,y_pair,graph_h) or not arc_need: # just if ab e A(H) or there's no need of arc

          xy = str(x_pair)+str(y_pair); # create the pair
          if key not in self.list_pairs: #create key and a set inside
            self.list_pairs[key] = set()

          self.list_pairs[key].add(xy)

  def copy_reverse(self):
    return
    #for key, value in self.list_pairs.items():




  def pair_consistency(self, graph_h):
    done = False
    stop = True
    while not done:
      done = True
      for x in range(len(self.vertices) ): # for all vertices x, do the domain reduction against all arcs with x
        # for all neighbors of x that has't been checked
        pairs = (y for y,value in self.vertices.items() if y !=x ) 
        for y in pairs:
          # reduction from arc xy to a third variable z
          for z in range(len(self.vertices)):
            if z!= y and z!= x:
              stop = self.pair_reduction(x,y,z,graph_h)
              if not stop:
                done = False

  def pair_reduction(self,x,y,z,graph_h):
    done = True
    eliminate_a_b = True # set to false if a has a connection if list of b based on Graph H
    a_lis = ""
    b_lis = ""
    c_lis = ""
    pair  = ""
    # to check if key is in the dic - xy
    pair_a_c = ""
    pair_b_c = ""
    x_p = min(x,z)
    y_p = min(y,z)


    key_a = str(x) + str(z)
    key_b = str(y) + str(z)
    key = str(x)+str(y)               #no such domain
    if key_a not in self.list_pairs or key_b not in self.list_pairs or key not in self.list_pairs:
      return True # done so far for this one

    for a_lis in self.list_match_G_H[x]: # for all elem in the list(x) of possibe match 
      for b_lis in self.list_match_G_H[y]:
        for c_lis in self.list_match_G_H[z]:
          
          pair_a_c = str(a_lis) + str(c_lis)
          pair_b_c = str(b_lis) + str(c_lis)
          pair = str(a_lis) + str(b_lis)
          if pair_a_c in self.list_pairs[key_a] and pair_b_c in self.list_pairs[key_b]:
            eliminate_a_b = False
            break


        if eliminate_a_b and pair in self.list_pairs[key] : 
          self.list_pairs[key].remove(pair)
          done = False

        eliminate_a_b = True
    
    return done

  def is_homomorphic(self):
    if len(self.list_match_G_H) < len(self.vertices) or len(self.list_pairs) < len(self.vertices):
       return False

    else:
      for key,value in self.list_match_G_H.items():
        if len(value) <=0:
          return False

      for key,value in self.list_pairs.items():
        if len(value) <=0:
          return False

    return True

  def check_homomophism(self, graph_h):
    if self.is_homomorphic():
      print("Graph " + self.graph_name + " is homomorphic with Graph " + graph_h.graph_name + "\n")
    else:
      print("Graph " + self.graph_name + " is not homomorphic with Graph " + graph_h.graph_name + "\n")

  def print_graph(self):
    print("\nGraph " + self.graph_name)
    for key in sorted(list(self.vertices.keys())):
      print(str(key) + " ",str(self.vertices[key].neighbors))

    print()

  def print_list_G_H(self):
    print("\nList")
    for key,value in sorted(self.list_match_G_H.items()):
      print(str(key) + " ",str(value))

    print()

  def print_pairs(self):
    print("\t*** Pairs ***\n")
    for key,value in sorted(self.list_pairs.items()):
      print("Key(x,y) "+str(key) + " --> ",str(value))

    print()



if len(sys.argv) < 4:
  print("Please provide <FileGraph_G>, <FileGraph_H> & FileList_G_H\n")
  sys.exit(1)

file_g = sys.argv[1]
file_h = sys.argv[2]
file_list = sys.argv[3]

graph_g = Graph(file_g)
graph_h = Graph(file_h)


graph_g.print_graph()
graph_h.print_graph()

graph_g.init_list(file_list)
print("\t*** Before reduction ***",end="")
graph_g.print_list_G_H()

graph_g.arc_consistency(graph_h)

print("\t*** After reduction ***",end="")
graph_g.print_list_G_H()

graph_g.make_pairs(graph_h)

graph_g.print_pairs()

graph_g.pair_consistency(graph_h)

print("\t*** After Pair Consistency ***\n")
graph_g.print_pairs()

graph_g.check_homomophism(graph_h)


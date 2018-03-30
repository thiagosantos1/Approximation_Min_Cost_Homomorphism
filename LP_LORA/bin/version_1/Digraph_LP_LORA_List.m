# The idea is to read G and H and save in matrixes, and while reading, count how many in each side of the 
# partition in G and H. That is gonna be how we create our variables
# We also have to read file that contains the cost of maping each vertex in G for a vertex in H

# This LP uses the pair list(list homomisph). We first run pair consistency in the python program
# Then, use the updated list to run our LP faster.
graph_g = fopen("graph_g.mat");
if graph_g <1;
  disp("Error opening graph g\n"); 
  exit(1);
endif

graph_h = fopen("graph_h_1.mat");
if graph_h <1; 
  disp("Error opening graph h\n");
  exit(1);
endif

list_cost_g_h = fopen("list_cost.mat");
if list_cost_g_h <1;
  disp("Error opening list_cost_g_h \n");
  exit(1);
endif

# now, read the list_homom and create new constraints, 
# based on where a vertex can or not be mapped to another in H
# we assume there's at least one possibility for each vertex in G
# otherwise, our python program already concluded that there is not a homomophism
# each line represets a list of each vertex(line 1 = vertex 1, etc...)
list_homom = fopen("list_homom.mat");
if list_homom <1;
  disp("Error opening list_homom\n"); 
  exit(1);
endif

num_vertex_g = str2num(fgetl(graph_g));
num_vertex_h = str2num(fgetl(graph_h));

# to store a matrix representing the agency of each vertex
# initialize a matrix NxN with 0
adj_matrix_g(num_vertex_g, num_vertex_g) = 0;
adj_matrix_h(num_vertex_h,num_vertex_h) = 0;
# row represents index of adj_matrix_g and colums represents index adj_matrix_h
adj_matrix_cost_g_h(num_vertex_g,num_vertex_h) = 0; # cost to map each vertex in G to one in H

# row represents index of adj_matrix_g and colums represents index adj_matrix_h
# we use this matrix to save the index on C and A of each pair
adj_matrix_index_saving(num_vertex_g,num_vertex_h) = 0; 

# list_homom
# save a matrix with all possible assigns for each vertex, based on the list_homom 
adj_matrix_list_homom(num_vertex_g, num_vertex_h) = 0;

# list of elements in G and H
vertices_graph_g = [];
vertices_graph_h = [];

# indexes is represented by each element in right of G againts all elements in left of H
# Then, by each element of left of G against all elements in right of H
# that's why we use this count, to be able to track back later the index of each pair.
c= [];
A = [];
b = [];
lb = [];
ub = [];
ctype = "";
vartype = ""; 

# reading for G
while (row = fgetl(graph_g)) >0;
  row = strsplit(row," "); % split the string by space
  x = str2num(char(row(1)));
  y = str2num(char(row(2)));
  
  adj_matrix_g(x+1, [y+1]) = 1;
  if ! any(vertices_graph_g == x+1);
    vertices_graph_g(end +1) = x+1;
  endif
  
  if ! any(vertices_graph_g == y+1);
    vertices_graph_g(end +1) = y+1;
  endif
  
endwhile

vertices_graph_g = sort(vertices_graph_g);

# reading for H
while (row = fgetl(graph_h)) >0;
  row = strsplit(row," "); % split the string by space
  x = str2num(char(row(1)));
  y = str2num(char(row(2)));
  

  adj_matrix_h(x+1, [y+1]) = 1;
  if ! any(vertices_graph_h == x+1);
    vertices_graph_h(end +1) = x+1;
  endif
 
  if ! any(vertices_graph_h == y+1);
    vertices_graph_h(end +1) = y+1; 
  endif
endwhile

vertices_graph_h = sort(vertices_graph_h);

# read list homom and save in the matrix
vertex_g = 1;
while (row = fgetl(list_homom)) >0;
  row = strsplit(row," "); % split the string by space
  for x = 1:length(row);
    vertex_h = str2num(char(row(x)));
    adj_matrix_list_homom(vertex_g, [vertex_h]) = 1; #vertex_h is available to vertex_g
  endfor
  vertex_g +=1;
endwhile

# reading for cost G to H
while (row = fgetl(list_cost_g_h)) >0;
  row = strsplit(row," "); % split the string by space
  l_side = str2num(char(row(1)));
  r_side = str2num(char(row(2)));
  cost   = str2num(char(row(3)));
  
  # left side
  adj_matrix_cost_g_h(l_side+1, [r_side+1]) = cost;
endwhile

# base on the adj_matrix_cost_g_h we can build c
size_adj = size(adj_matrix_cost_g_h);

# for every vertex in G 
for vertex_g=1:size_adj(1);
  start = -1;
  # check for it's possible assigns in H
  for vertex_h = 1:size_adj(2);
    if adj_matrix_cost_g_h(vertex_g, [vertex_h]) !=0;
      if start <=0; # means it's the first time you see a number
        c(end +1) = adj_matrix_cost_g_h(vertex_g, [vertex_h]);
        adj_matrix_index_saving(vertex_g, [vertex_h]) = length(c);
        
        vartype = cstrcat(vartype,"C");
        lb(end +1) = 0;
        start = 1;
      else
        c(end +1) = adj_matrix_cost_g_h(vertex_g, [vertex_h]) - adj_matrix_cost_g_h(vertex_g, [vertex_h-1]);
        adj_matrix_index_saving(vertex_g, [vertex_h]) = length(c);
        vartype =cstrcat(vartype,"C");
        lb(end +1) = 0;
      endif
    else
      if start >0;
        break;
      endif
    endif
  endfor 
endfor

# **** Have to check this. Because now we don't have a min ordering ***
# create first constrants based on min ordering
# for every vertex in G 
size_A = size(A);
for vertex_g=1:length(vertices_graph_g);
  start = -1;
  # check for all possible assigns in H
  for vertex_h = 1:length(vertices_graph_h);
    size_A = size(A);
    if start <=0;
      A(size_A(1)+1,[size_A(2)+1])= 1;
      b(end +1) = 1;
      ctype = cstrcat(ctype,"L");
      start = 1;
    else
      A(size_A(1)+1,[size_A(2)+1]) = -1;
      A(size_A(1)+1,[size_A(2)]) = 1;   
      b(end +1) = 0;
      ctype = cstrcat(ctype,"L");
    endif
  
  endfor
endfor

# function to return the l(i) of a vertex
# get the first neighbor of a vertex
function result = get_first_neighbor(vertex, matrix) 
  result =1;
  for vertex_pair = 1:length(matrix); 
    if matrix(vertex,[vertex_pair]) ==1;
      result = vertex_pair;
      break;
    endif
  endfor
endfunction







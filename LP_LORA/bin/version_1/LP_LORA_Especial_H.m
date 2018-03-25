# do the same of LP_LORA & LP_LORA_2 but with the file, for any graphs G and H

# The idea is to read G and H and save in matrixes, and while reading, count how many in each side of the 
# partition in G and H. That is gonna be how we create our variables
# We also have to read file that contains the cost of maping each vertex in G for a vertex in H

graph_g = fopen("graph_g_2.mat");
if graph_g <1;
  disp("Error opening graph g\n"); 
  exit(1);
endif

graph_h = fopen("graph_h_2.mat");
if graph_h <1; 
  disp("Error opening graph h\n");
  exit(1);
endif

list_cost_g_h = fopen("list_cost_2.mat");
if list_cost_g_h <1;
  disp("Error opening list_cost_g_h \n");
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

# list of elements in left and right of G and H
left_lis_G = [];
right_lis_G = [];
left_lis_H = [];
right_lis_H = [];

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
  l_side = str2num(char(row(1)));
  r_side = str2num(char(row(2)));
  
  
  # left side
  adj_matrix_g(l_side+1, [r_side+1]) = 1;
  if ! any(left_lis_G == l_side+1);
    left_lis_G(end +1) = l_side+1;
  endif
  
  # right side
  adj_matrix_g(r_side+1, [l_side+1]) = 1; # change here if working with Digraph
  if ! any(right_lis_G == r_side+1);
    right_lis_G(end +1) = r_side+1; 
  endif

endwhile

# reading for H
while (row = fgetl(graph_h)) >0;
  row = strsplit(row," "); % split the string by space
  l_side = str2num(char(row(1)));
  r_side = str2num(char(row(2)));
  
  # left side
  adj_matrix_h(l_side+1, [r_side+1]) = 1;
  if ! any(left_lis_H == l_side+1);
    left_lis_H(end +1) = l_side+1;
  endif
  
  # right side
  adj_matrix_h(r_side+1, [l_side+1]) = 1; # change here if working with Digraph
  if ! any(right_lis_H == r_side+1);
    right_lis_H(end +1) = r_side+1; 
  endif
  
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

# create first constrants based on min ordering

# for every vertex left of G 
size_A = size(A);
for vertex_g=1:length(left_lis_G);
  start = -1;
  # check for all possible assigns left of H
  for vertex_h = 1:length(left_lis_H);
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


# for every vertex right of G 
size_A = size(A);
for vertex_g=1:length(right_lis_G);
  start = -1;
  # check for all possible assigns right of H
  for vertex_h = 1:length(right_lis_H);
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


# complete the constrants on A, based if there is an edge
# for each vertex in G side, you check agains all vertex in left side if there is an edge, and vice-versa

# for left of G first
for vertex_g_let = left_lis_G; 
  # check for left or H
  for vertex_h_let = left_lis_H; 
  # then for each pair, you compare with all vertex in right of G, if there is an edge
    for vertex_g_right = right_lis_G;
      if adj_matrix_g(vertex_g_let,[vertex_g_right]) ==1; # if there is edge
        pair_h_right = get_first_neighbor(vertex_h_let,adj_matrix_h); # get the first neighbor of 
        size_A = size(A);
        A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_let,[vertex_h_let] )] )= -1;
        A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_right,[pair_h_right])]) = 1;
        ctype = cstrcat(ctype,"L");
        b(end +1) = 0;
      endif
    endfor
  endfor

endfor

# now for right of G 
for vertex_g_right = right_lis_G; 
  # check for right or H
  for vertex_h_right = right_lis_H; 
  # then for each pair, you compare with all vertex in left of G, if there is an edge
    for vertex_g_left = left_lis_G;
      if adj_matrix_g(vertex_g_right,[vertex_g_left]) ==1; # if there is edge
        pair_G_right = get_first_neighbor(vertex_h_right,adj_matrix_h); # get the first neighbor of 
        size_A = size(A);
        A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_right,[vertex_h_right] )] )= -1;
        A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_left,[pair_G_right])]) = 1;
        ctype = cstrcat(ctype,"L");
        b(end +1) = 0;
      endif
    endfor
  endfor

endfor



# add constrain for a specific H graph, to see how the LP is gonna behave.  
# Xu2 - Xu3 + Xu4 >= Xv7
# Xv7 - Xu2 - Xu3 + Xu4 >= 0
# for left of G first
for vertex_g_let = left_lis_G; 
  # then for each pair, you compare with all vertex in right of G, if there is an edge
  for vertex_g_right = right_lis_G;
    if adj_matrix_g(vertex_g_let,[vertex_g_right]) ==1; # if there is edge
      size_A = size(A);
      A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_right,[7])]) = -1;
      A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_let,[2] )] )= 1;
      A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_let,[3] )] )= -1;
      A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_let,[4] )] )=  1;
      ctype = cstrcat(ctype,"L");
      b(end +1) = 0;
    endif
  endfor
endfor
    


#adj_matrix_g
#left_lis_G
#right_lis_G

#adj_matrix_h
#left_lis_H
#right_lis_H

#adj_matrix_cost_g_h

#adj_matrix_index_saving

#A
#b
#c
#vartype
#lb
#ctype

s = 1; % minimixation problem
param.msglev = 1;

[xmin] = glpk(c, A, b, lb, ub, ctype, vartype, s, param)

result = c * xmin



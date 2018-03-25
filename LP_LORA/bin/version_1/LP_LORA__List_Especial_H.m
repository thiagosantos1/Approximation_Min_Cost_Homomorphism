# The idea is to read G and H and save in matrixes, and while reading, count how many in each side of the 
# partition in G and H. That is gonna be how we create our variables
# We also have to read file that contains the cost of maping each vertex in G for a vertex in H

# This LP uses the pair list(list homomisph). We first run pair consistency in the python program
# Then, use the updated list to run our LP faster.
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
# also, include constraints based if i is available for Xu or Xv

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
    
    size_A = size(A);
    # add constraint Xui = Xu,i+1 if i not in L(u)
    # if it's the last one, Xup = 0 if p not in L(u)
    if vertex_h_let >= length(left_lis_H); # means it's the last one
      if adj_matrix_list_homom(vertex_g_let,[vertex_h_let]) != 1; # i not in L(u)
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_let,[vertex_h_let])]) = 0;
        ctype = cstrcat(ctype,"L");
        b(end +1) = 0;
      endif
    else
      if adj_matrix_list_homom(vertex_g_let,[vertex_h_let]) != 1; #Xui = Xui+1 if i not in L(u)
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_let,[vertex_h_let])]) = 1; # Xui = 1
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_let,[vertex_h_let+1])]) = 1; # Xui+1 = 1
        ctype = cstrcat(ctype,"L");
        b(end +1) = 0;
      endif  
    
    endif
    
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
    
    # Make sure about this one, cause not sure if we should add for both u and v
    size_A = size(A);
    # add constraint Xvi = Xv,i+1 if i not in L(v)
    # if it's the last one, Xvp = 0 if p not in L(v)
    if vertex_h_right >= length(right_lis_H);
      if adj_matrix_list_homom(vertex_g_right,[vertex_h_right]) != 1;
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_right,[vertex_h_right])]) = 0;
        ctype = cstrcat(ctype,"L");
        b(end +1) = 0;
      endif
    else
      if adj_matrix_list_homom(vertex_g_right,[vertex_h_right]) != 1; #Xui = Xui+1 if i not in L(u)
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_right,[vertex_h_right])]) = 1;
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_right,[vertex_h_right+1])]) = 1;
        ctype = cstrcat(ctype,"L");
        b(end +1) = 0;
      endif  
    
    endif
    
  endfor

endfor

# now add constraints based on list_pairs. 
# Xui - Xu,i+1 <= Sum(Xvj - Xv,j+1) --> For all u,v in G | (i,j) in L(u,v)
# Thus --> Sum(Xvj - Xv,j+1) - Xui + Xu,i+1 >=0
# L(u,v) = { (i,j), (i,j+1)....}
# to do that, we gonna use struct, creating a new variable everytime, with Hash, to represent the pair key
# inside, each two columns represent a pair available for that key

dict_pairs = struct();
num_vert_g = length(right_lis_G) + length(left_lis_G);
num_vert_h = length(right_lis_H) + length(left_lis_H);
# function to create a hash for a pair
function key = create_hash(x, y,N) 
  key = (x*N) + y;
  
endfunction

# function to get pair from a hash 
function pair = get_x_y_hash(key,N) 
  #pair = [0 0];
  rest = mod(key,N);
  temp = int32(key/N);
  x = y =0;
  if rest >0; # rest to make it integer value
    temp -=1;
  endif

	if temp * N == key && key!=0; # means you don't have mod - then, it means y is the last node = N
		x = temp -1;
	else
		x = temp;
  endif
	y = key - (x*N);
 
  pair(1,[1]) = x;
  pair(1,[2]) = y;
  
endfunction

# read from file pairs, create key and add all pairs for that key
list_pairs = fopen("list_pairs.mat");
if list_pairs <1;
  disp("Error opening list pairs\n"); 
  exit(1);
endif

# read list pairs and save in the structe
# the logis is: one row = key(2 columns, one for each representative pair)
#               next row = all pairs for that key, each one determined in 2 in 2 columns
#               Keep this loop
key_line = 1; # first line you know it must be a key
key = 0; # save key from previous line to save its pairs
x = y =0;
while (row = fgetl(list_pairs)) >0;
  row = strsplit(row," "); % split the string by space
  pairs = [];
  if key_line ==1; # you know it must have only 2 columns, one for x and one for y
    x = str2num(char(row(1)));
    y = str2num(char(row(2)));
    key = create_hash(x,y,num_vert_g); # create a hash for that pair
    if isfield(dict_pairs, num2str(key));
      display("Key already exist in dict_pairs, something is wrong\n") 
    endif
  else
    # do this for all pairs of that key
    for range = 1:2:length(row); # from 1, incrementing by 2, till size of row
      x = str2num(char(row(range)));
      y = str2num(char(row(range+1)));
      pairs(1,end +1) = x; # one line, 2 columns represent a pair
      pairs(1,end +1) = y;
    endfor 
    # then, add to struct, the pairs for that key
    dict_pairs.(num2str(key)) = pairs;
  endif
  
  key_line *= -1; # invert
endwhile

# function to get all paris saved in the dictionary, based on a key(pair(u,v))
function pairs = get_pairs_dict(key,dict_pairs) 
pairs = getfield (dict_pairs, num2str(key));
endfunction

# function to return if pair(i,j) is in the list(u,v) 
function isIn = is_pair_list(x_i, y_j, list_pairs)
  isIn = false;
  for range = 1:2:length(list_pairs);
    x = list_pairs(range);
    y = list_pairs(range+1); 
    if x == x_i && y == y_j;
      isIn = true;
      break;
    endif
  endfor
endfunction

# with the list, we can then run the constraints
# Xui - Xu,i+1 <= Sum(Xvj - Xv,j+1) --> For all u,v in G | (i,j) in L(u,v)
# L(u,v) = { (i,j), (i,j+1)....}
# Thus --> Sum(Xvj - Xv,j+1) - Xui + Xu,i+1 >=0
# L(u,v) = { (i,j), (i,j+1)....}

# first, to verteces U

# for left of G first
for vertex_g_let = left_lis_G; 
  # get the first 2 to start
  # for left of H
  for range = 1:length(left_lis_H); # for all i in H for u
    vertex_h_let_i = left_lis_H(range);
    vertex_h_let_i_ = -1;
    if range < length(left_lis_H);
      vertex_h_let_i_ = left_lis_H(range +1);
    endif
    # for all ui & ui+1, construct the constraint for all vj & vj+1
    # for right of G
    for vertex_g_right = right_lis_G; # for all v
      # for right of H
      size_A = size(A); # new line of constraint for every ui,u+1 v # Sum part
      for range_ = 1:length(right_lis_H); # for all j in h
        vertex_h_right_j = right_lis_H(range_); 
        vertex_h_right_j_ = -1;
        if range_ < length(right_lis_H);
          vertex_h_right_j_ = right_lis_H(range_ +1); 
        endif;
        #Sum(Xvj - Xv,j+1) - Xui + Xu,i+1 >=0
        # get if (i,j) is in the list(u,v)
        key = create_hash(vertex_g_let,vertex_g_right,num_vert_g) ;
        if isfield(dict_pairs, num2str(key));
          pairs = get_pairs_dict(key,dict_pairs);
          # if (i,j) is in the list(u,v), then construct the constraint
          if is_pair_list(vertex_h_let_i, vertex_h_right_j, pairs) > 0;
            w=1;
            range_;
            length(right_lis_H); 
            # for v # for last one, just add 1
            A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_right,[vertex_h_right_j])]) = 1;
            if range_ < length(right_lis_H);
              A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_right,[vertex_h_right_j_])]) = -1;
            endif
          endif
        else
          display("Key already exist in dict_pairs, something is wrong\n") 
        endif
        
       endfor
       # for u # for last one, just add 1
       if range < length(left_lis_H);
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_let,[vertex_h_let_i])]) = -1;
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_let,[vertex_h_let_i_])]) = 1;
       else
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_let,[vertex_h_let_i])]) = 1;
       endif
       
       ctype = cstrcat(ctype,"L");
       b(end +1) = 0; 
       
    endfor
  endfor
endfor

# Now, to verteces V

# for right of G first
for vertex_g_right = right_lis_G; 
  # for right of H
  for range = 1:length(right_lis_H); # from second one -1 lenght. cause we get ui and ui+1
    vertex_h_right_j = right_lis_H(range); 
    vertex_h_right_j_ = -1;
    if range < length(right_lis_H);
      vertex_h_right_j_ = right_lis_H(range+1);
    endif
    # for all vj & vj+1, construct the constraint for all ui & ui+1
    # for left of G
    for vertex_g_let = left_lis_G; 
      # for right of H
      size_A = size(A);
      for range_ = 1:length(left_lis_H); # from second one -1 lenght. cause we get ui and ui+1
         vertex_h_let_i = left_lis_H(range_); 
         vertex_h_let_i_ = 1;
         if range_ < length(left_lis_H);
          vertex_h_let_i_ = left_lis_H(range_+1);
         endif
        #Sum(Xui - Xu,i+1) - Xvj + Xv,j+1 >=0
        # get if (j,i) is in the list(v,u)
        key = create_hash(vertex_g_right, vertex_g_let, num_vert_g);
        if isfield(dict_pairs, num2str(key));
          pairs = get_pairs_dict(key,dict_pairs);
          # if (j,i) is in the list(v,u), then construct the constraint
          if is_pair_list(vertex_h_right_j,vertex_h_let_i, pairs) > 0;
            # for u
            A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_let,[vertex_h_let_i])]) = 1;
            if range_ < length(left_lis_H);
              A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_let,[vertex_h_let_i_])]) = -1;
              
            endif
             
          endif
        else
          display("Key already exist in dict_pairs, something is wrong\n") 
        endif
      endfor
      # for v
      if range < length(right_lis_H);
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_right,[vertex_h_right_j])]) = -1;
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_right,[vertex_h_right_j_])]) = 1;
        
      else
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_right,[vertex_h_right_j])]) = 1;
      endif
      ctype = cstrcat(ctype,"L");
      b(end +1) = 0;
    endfor
  endfor
endfor

#dict_pairs

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

# add constrain for a specific H graph, to see how the LP is gonna behave.  
# Xu2 - Xu3 + Xu4 >= Xv7
# Xv7 - Xu2 - Xu3 + Xu4 >= 0
# for left of G first
for vertex_g_let = left_lis_G; 
  # then for each pair, you compare with all vertex in right of G, if there is an edge
  for vertex_g_right = right_lis_G;
    if adj_matrix_g(vertex_g_let,[vertex_g_right]) ==1; # if there is edge
      size_A = size(A);
      A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_right,[7])]) = 1;
      A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_let,[2] )] )= -1;
      A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_let,[3] )] )= -1;
      A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_let,[4] )] )=  1;
      ctype = cstrcat(ctype,"L");
      b(end +1) = 0;
    endif
  endfor
endfor
    



s = 1; % minimixation problem
param.msglev = 1;

[xmin] = glpk(c, A, b, lb, ub, ctype, vartype, s, param)

result = c * xmin



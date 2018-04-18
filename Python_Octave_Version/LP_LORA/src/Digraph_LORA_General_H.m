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

graph_h = fopen("graph_h.mat");
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
vertices_graph_g = [1:num_vertex_g];
vertices_graph_h = [1:num_vertex_h];

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
type_solution = "C";

# reading for G
while (row = fgetl(graph_g)) >0;
  row = strsplit(row," "); % split the string by space
  x = str2num(char(row(1)));
  y = str2num(char(row(2)));
  
  adj_matrix_g(x+1, [y+1]) = 1;
  
endwhile

vertices_graph_g = sort(vertices_graph_g);

# reading for H
while (row = fgetl(graph_h)) >0;
  row = strsplit(row," "); % split the string by space
  x = str2num(char(row(1)));
  y = str2num(char(row(2)));
  

  adj_matrix_h(x+1, [y+1]) = 1;
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
        
        vartype = cstrcat(vartype,type_solution);
        lb(end +1) = 0;
        start = 1;
      else
        c(end +1) = adj_matrix_cost_g_h(vertex_g, [vertex_h]) - adj_matrix_cost_g_h(vertex_g, [vertex_h-1]);
        adj_matrix_index_saving(vertex_g, [vertex_h]) = length(c);
        vartype =cstrcat(vartype,type_solution);
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
      ctype = cstrcat(ctype,"S");
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
function result = get_first_out_neighbor(vertex, begi_, end_, matrix) 
  result =0;
  if begi_ >0 && end_ <= length(matrix);
    for vertex_pair = begi_:end_; 
      if matrix(vertex,[vertex_pair]) ==1;
        result = vertex_pair;
        break;
      endif
    endfor
  endif
endfunction

# function to return the l(i) of a vertex
# get the first neighbor of a vertex
function result = get_first_in_neighbor(vertex, begi_, end_, matrix) 
  result =0;
  if begi_ >0 && end_ <= length(matrix);
    for vertex_pair = begi_:end_; 
      if matrix(vertex_pair,[vertex]) ==1;
        result = vertex_pair;
        break;
      endif
    endfor
  endif
endfunction

# complete the constrants on A, based if there is an edge
# for each vertex in G side, you check agains all vertex in left side if there is an edge, and vice-versa
# also, include constraints based if i is available for Xu or Xv

# arc consistency
for vertex_g_u = vertices_graph_g; 
  # check for  H
  for vertex_h_i = vertices_graph_h; 
    
    size_A = size(A);
    # add constraint Xui = Xu,i+1 if i not in L(u)
    # if it's the last one, Xup = 0 if p not in L(u)
    if vertex_h_i >= length(vertices_graph_h); # means it's the last one
      if adj_matrix_list_homom(vertex_g_u,[vertex_h_i]) != 1; # i not in L(u)
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_u,[vertex_h_i])]) = 1;
        ctype = cstrcat(ctype,"S");
        b(end +1) = 0;
      endif
    else
      if adj_matrix_list_homom(vertex_g_u,[vertex_h_i]) != 1; #Xui = Xui+1 if i not in L(u)
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_u,[vertex_h_i])]) = 1; # Xui = 1
        A(size_A(1)+1,[adj_matrix_index_saving(vertex_g_u,[vertex_h_i+1])]) = -1; # Xui+1 = 1
        ctype = cstrcat(ctype,"S");
        b(end +1) = 0;
      endif  
    
    endif 
  endfor
endfor


# for in_neighbor first
for vertex_g_u = vertices_graph_g; 
  # check for  H
  for vertex_h_i = vertices_graph_h; 
  # then for each pair, you compare with all vertex in right of G, if there is an edge
    for vertex_g_v = vertices_graph_g;
      if adj_matrix_g(vertex_g_u,[vertex_g_v]) ==1; # if there is edge
        in_nbr_i = get_first_in_neighbor(vertex_h_i,1,length(adj_matrix_h),adj_matrix_h); # get the first neighbor of v
        if in_nbr_i >0;
          size_A = size(A);
          A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_u,[vertex_h_i] )] )= -1;
          A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_v,[in_nbr_i])]) = 1;
          ctype = cstrcat(ctype,"L");
          b(end +1) = 0;
        endif
      endif
    endfor
  endfor

endfor


# for out_neighbor  first
for vertex_g_u = vertices_graph_g; 
  # check for left or H
  for vertex_h_i = vertices_graph_h; 
  # then for each pair, you compare with all vertex in right of G, if there is an edge
    for vertex_g_v = vertices_graph_g;
      if adj_matrix_g(vertex_g_u,[vertex_g_v]) ==1; # if there is edge
        out_nbr_i = get_first_out_neighbor(vertex_h_i,1,length(adj_matrix_h),adj_matrix_h); # get the first neighbor of v
        if out_nbr_i > 0;
          size_A = size(A);
          A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_u,[vertex_h_i] )] )= -1;
          A(size_A(1) +1, [adj_matrix_index_saving(vertex_g_v,[out_nbr_i])]) = 1;
          ctype = cstrcat(ctype,"L");
          b(end +1) = 0;
        endif
      endif
    endfor
    
  endfor

endfor


# now add constraints based on list_pairs. 
# Xui - Xu,i+1 <= Sum(Xvj - Xv,j+1) --> For all u,v in G | (i,j) in L(u,v)
# Thus --> Sum(Xvj - Xv,j+1) - Xui + Xu,i+1 >=0
# L(u,v) = { (i,j), (i,j+1)....}
# to do that, we gonna use struct, creating a new variable everytime, with Hash, to represent the pair key
# inside, each two columns represent a pair available for that key

dict_pairs = struct();
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
    key = create_hash(x,y,num_vertex_g); # create a hash for that pair
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
for g_u = vertices_graph_g; 
   # get the first 2 to start - i & i+1
   for range = 1:length(vertices_graph_h); # for all i in H for u
      h_i = vertices_graph_h(range);
      h_i_ = -1;
      if range < length(vertices_graph_h);
        h_i_ = vertices_graph_h(range +1);
      endif
      
      i_is_in_gu = false;
      # check if i and i+1 is in the list of U first. Then, add the constraint
      if adj_matrix_list_homom(g_u,[h_i]) == 1; 
        #if  h_i_ != -1;
         # if adj_matrix_list_homom(g_u,[h_i_]) == 1; 
          #   i_is_in_gu = true;
          #endif
        #else
          i_is_in_gu = true;
        #endif
      endif
      
     if i_is_in_gu == true; 
        
        # now, for all ui & ui+1, construct the constraint for all vj & vj+1
        for g_v = vertices_graph_g; # for all v
          if g_u != g_v;
              
            size_A = size(A); # new line of constraint for every ui,u+1 v # Sum part
            for range_ = 1:length(vertices_graph_h); # for all j in h
              h_j = vertices_graph_h(range_); 
              h_j_ = -1;
              if range_ < length(vertices_graph_h);
                h_j_ = vertices_graph_h(range_ +1); 
              endif;
              #Sum(Xvj - Xv,j+1) - Xui + Xu,i+1 >=0
              # get if (h_i,h_j) is in the list(g_u,g_v)
              key = create_hash(g_u,g_v,num_vertex_g) ;
              if isfield(dict_pairs, num2str(key)); 
                  pairs = get_pairs_dict(key,dict_pairs);
                  # if (i,j) is in the list(u,v), then construct the constraint
                  if is_pair_list(h_i, h_j, pairs) > 0;
              
                    length(vertices_graph_h); 
                    A(size_A(1)+1,[adj_matrix_index_saving(g_v,[h_j])]) = 1;
                    if range_ < length(vertices_graph_h);
                      A(size_A(1)+1,[adj_matrix_index_saving(g_v,[h_j_])]) = -1;
                    endif
                  endif
              else
                display("Key already exist in dict_pairs, something is wrong\n") 
              endif
            endfor  
           
              # for u # for last one, just add 1
             if range < length(vertices_graph_h);
              A(size_A(1)+1,[adj_matrix_index_saving(g_u,[h_i])]) = -1;
              A(size_A(1)+1,[adj_matrix_index_saving(g_u,[h_i_])]) = 1;
             else
              A(size_A(1)+1,[adj_matrix_index_saving(g_u,[h_i])]) = -1;
             endif
             ctype = cstrcat(ctype,"L");
             b(end +1) = 0; 
          endif 
        endfor  
    endif    
  endfor
endfor

# add constraint for missing edges(From Mix_Max ordering)
# to any Diagraph H.
# for more details, please check the formulas at the paper, page 7

for u = vertices_graph_g; 
  for v = vertices_graph_g; 
    if adj_matrix_g(u,[v]) ==1; # for all uv | uv E A(G)
      size_list = size(adj_matrix_list_homom)(2);
      for i = 1:size_list; # for possible assigns in u
        if adj_matrix_list_homom(u,[i]) ==1; # if i is in the list of U.
        
          for j = 1:size_list; # for possible assigns in v
            if adj_matrix_list_homom(v,[j]) ==1; # if j is in the list of v.
              
              if adj_matrix_h(i,[j]) !=1; # if ij !E A(H)
                
                first_in_nbr_j = get_first_in_neighbor(j,1,length(adj_matrix_h),adj_matrix_h);
                first_out_nbr_i = get_first_out_neighbor(i,1,j,adj_matrix_h);
                if first_in_nbr_j >0 && first_out_nbr_i > 0;
                  if first_in_nbr_j < i && first_out_nbr_i <j;
                    
                    # first base constrain -- For out-neighbors
                    # first out neighbor of i, after j  
                    s = get_first_out_neighbor(i,j+1,length(adj_matrix_h),adj_matrix_h);
                    size_A = size(A);
                    if s >0;
                      # base
                      A(size_A(1)+1,[adj_matrix_index_saving(v,[s])]) = 1; 
                      A(size_A(1)+1,[adj_matrix_index_saving(u,[i])]) = -1; 
                      ctype = cstrcat(ctype,"L");
                      b(end +1) = 0;
                      
                      #sum
                      for t=1:j-1;
                        if adj_matrix_h(i,[t]) ==1;
                          A(size_A(1)+1,[adj_matrix_index_saving(v,[t])]) = 1; 
                          if t < length(adj_matrix_h);
                            A(size_A(1)+1,[adj_matrix_index_saving(v,[t+1])]) = -1; 
                          endif
                        endif
                      endfor
                    
                    else
                      # base
                      if i < length(adj_matrix_h);
                        A(size_A(1)+1,[adj_matrix_index_saving(u,[i+1])]) = 1; 
                      endif
                      A(size_A(1)+1,[adj_matrix_index_saving(u,[i])]) = -1; 
                      ctype = cstrcat(ctype,"L");
                      b(end +1) = 0;
                      #sum
                      for t=1:length(adj_matrix_h);
                        if adj_matrix_h(i,[t]) ==1;
                          A(size_A(1)+1,[adj_matrix_index_saving(v,[t])]) = 1; 
                          if t < length(adj_matrix_h);
                            A(size_A(1)+1,[adj_matrix_index_saving(v,[t+1])]) = -1; 
                          endif
                        endif
                      endfor
                    endif
                    
                    # second base constrain -- For in-neighbors
                    # first in neighbor of j, after i  
                    s = get_first_in_neighbor(j,i+1,length(adj_matrix_h),adj_matrix_h);
                    size_A = size(A);
                    if s >0;
                      # base
                      A(size_A(1)+1,[adj_matrix_index_saving(u,[s])]) = 1; 
                      A(size_A(1)+1,[adj_matrix_index_saving(v,[j])]) = -1; 
                      ctype = cstrcat(ctype,"L");
                      b(end +1) = 0;
                      #sum
                      for t=1:i-1;
                        if adj_matrix_h(t,[j]) ==1;
                          A(size_A(1)+1,[adj_matrix_index_saving(u,[t])]) = 1; 
                          if t < length(adj_matrix_h);
                            A(size_A(1)+1,[adj_matrix_index_saving(u,[t+1])]) = -1; 
                          endif
                        endif
                      endfor
                    
                    else
                      # base
                      if j < length(adj_matrix_h);
                        A(size_A(1)+1,[adj_matrix_index_saving(v,[j+1])]) = 1; 
                      endif
                      A(size_A(1)+1,[adj_matrix_index_saving(v,[j])]) = -1; 
                      ctype = cstrcat(ctype,"L");
                      b(end +1) = 0;
                      #sum
                      for t=1:i-1;
                        if adj_matrix_h(t,[j]) ==1;
                          A(size_A(1)+1,[adj_matrix_index_saving(u,[t])]) = 1; 
                          if t < length(adj_matrix_h);
                            A(size_A(1)+1,[adj_matrix_index_saving(u,[t+1])]) = -1; 
                          endif
                        endif
                      endfor
                    endif
                    
                  endif
                endif
              endif
            endif
          endfor
        endif
      endfor
    endif
  endfor
endfor



s = 1; % minimixation problem
param.msglev = 1;

[xmin] = glpk(c, A, b, lb, ub, ctype, vartype, s, param)
result = c * xmin


%open the file with the network(weighted digraph)
digraph = fopen("graph.mat")
if digraph <1;
  disp("Error opening file\n");
  exit(1);
endif

 % get the first line that presetenst how many vertexes - convert string to number
 num_vertex = str2num(fgetl(digraph));
 
c= [];
A = [];
b = [];
lb = [];
ub = [];
ctype = "";
vartype = ""; 
s = 1; % minimixation problem
param.msglev = 1;

pos = 1;

for pos=1:num_vertex;
  c(pos) = 0;
  lb(pos) = 0;
  vartype =cstrcat(vartype,"C");
endfor

% read and populate A
line=1; 
while (row = fgetl(digraph)) >0;
  row = strsplit(row," "); % split the string by space
  index_i = str2num(char(row(1)));
  index_j = str2num(char(row(2)));
  weight =  str2num(char(row(3)));
  c(line + num_vertex) = weight;
  A(line , [num_vertex+line]) = 1;
  A(line, [index_i+1]) = -1;
  A(line, [index_j+1]) = 1;
  line+=1; 
endwhile

A(line,1) = 1;
A(line,num_vertex) = -1;

% populate b - right side of the constraint
for x=1:line;
  if x==line;
    b(x) = 1; % only constraint that has to be greather than 0 - for source and sink
  else
    b(x) = 0;
    vartype =cstrcat(vartype,"C");
  endif
endfor

% populate lb 
for x=1:(line+num_vertex)-1;
  lb(x)=0;
endfor

[xmin] = glpk(c, A, b, lb, ub, ctype, vartype, s, param);

result = c * xmin

printf("The cut for the graph is %d\n", result);



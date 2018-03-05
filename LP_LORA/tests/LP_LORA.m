# doing by hand, for example in the slides - Just to be easier to undestand

# Xui 
#   X(u,5), X(u,4), X(u,1), X(u,7)

# Xvi
#   X(v,4), X(v,2), X(v,6)

# Cost
# C(u,5) = 5, C(u,4) = 8, C(u,1) = 9, C(u,7) = 4
# C(v,4) = 9, C(v,2) = 6, C(v,6) = 7

# Objective Function
# indexes = Xu5 Xu3 Xu1 Xu7 Xv4 Xv2 Xv6
# Xui = Cui - Cu,i-1
# Xu1 & Xv1 = Cu1

c = [5,(8-5),(9-8),(4-9)   3,(6-3),(7-6)]

# Constrains
# indexes = Xu5 Xu3 Xu1 Xu7 Xv4 Xv2 Xv6
#A = [ 1 0 0 0 0 0 0 # >=0  1 >= Xu5 >= Xu3 >= Xu1 >= Xu7 >= 0
#      0 0 0 0 1 0 0 # >=0  1 >= Xv4 >= Xv2 >= Xv6 >= 0
A = [ 1 0 0 0 0 0 0 # >= 1
      1 -1 0 0 0 0 0 # >=0
      0 1 -1 0 0 0 0 # >=0
      0 0 1 -1 0 0 0 # >0    
      0 0 0 0 1 0 0 # >= 1
      0 0 0 0 1 -1 0 # >=0
      0 0 0 0 0 1 -1 # >=0
      
      -1 0 0 0 1 0 0 # Xu5 <= Xv,l(5) = 4 --> xv,4 - Xu,5 >=0 first neighbor of 5(left side to right, bottom to up)
      0 -1 0 0 1 0 0 # Xu3 <= Xv,l(3) = 4 --> xv,4 - Xu,3 >=0
      0 0 -1 0 1 0 0 # Xu1 <= Xv,l(1) = 4 --> xv,4 - Xu,1 >=0
      0 0 0 -1 0 1 0 # Xu7 <= Xv,l(7) = 2 --> xv,2 - Xu,7 >=0
      1 0 0 0 -1 0 0 # Xv4 <= Xu,l(4) = 5 --> Xu,5 - xv,4 >=0
      0 1 0 0 0 -1 0 # Xv2 <= Xu,l(2) = 3 --> Xu,3 - xv,2 >=0
      0 0 1 0 0 0 -1 # Xv4 <= Xu,l(6) = 1 --> Xu,1 - xv,4 >=0
]

% right side of the constraints
b = [1;0;0;0;1;0;0;0;0;0;0;0;0;0]

ub = [];

lb = [0 0 0 0 0 0 0 ]; % lower bound - greather than 0

ctype = "LLLLLLLLLLLLLL";

vartype = "CCCCCCC"; % continuos variables
s = 1; % minimixation problem
param.msglev = 1;

[xmin] = glpk(c, A, b, lb, ub, ctype, vartype, s, param)

result = c * xmin
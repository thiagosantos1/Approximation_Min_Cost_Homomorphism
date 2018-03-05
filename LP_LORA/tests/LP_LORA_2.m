        ile# doing by hand, for example in the slides - Just to be easier to undestand

# Xui 
#   X(u,5), X(u,4), X(u,1), X(u,7)

# Xvi
#   X(v,4), X(v,2), X(v,6)

# Cost
# C(u,5) = 5, C(u,4) = 8, C(u,1) = 9, C(u,7) = 4
# C(v,4) = 9, C(v,2) = 6, C(v,6) = 7

# Objective Function
# indexes = X00 X01 X02 X03; X10 X11 X12 X13; X24 X25 X26; X34 X35 X36
# Xui = Cui - Cu,i-1
# Xu1 & Xv1 = Cu1

c = [10,(8-10),(9-8),(4-9)  3,(5-3),(8-5),(6-8)  3,(6-3),(7-6)   4,(3-4),(5-3)]

# Constrains
# indexes = Xu00 Xu01 Xu02 Xu03; Xu10 Xu11 Xu12 Xu13; Xv24 Xv25 Xv26; Xv34 Xv35 Xv36
A = [ 1 0 0 0 0 0 0 0 0 0 0 0 0 0  # Xu00 >= 1 
      1 -1 0 0 0 0 0 0 0 0 0 0 0 0 # Xu00 - Xu01 >=0
      0 1 -1 0 0 0 0 0 0 0 0 0 0 0 # Xu01 - Xu02 >=0
      0 0 1 -1 0 0 0 0 0 0 0 0 0 0 # Xu02 - Xu03 >=0
  
      0 0 0 0 1 0 0 0 0 0 0 0 0 0  # Xu10 >=1
      0 0 0 0 1 -1 0 0 0 0 0 0 0 0 # Xu10 - Xu11 >=0
      0 0 0 0 0 1 -1 0 0 0 0 0 0 0 # Xu11 - Xu12 >=0
      0 0 0 0 0 0 1 -1 0 0 0 0 0 0 # Xu12 - Xu13 >=0  
  
      0 0 0 0 0 0 0 0 1 0 0 0 0 0 # Xv24>=1
      0 0 0 0 0 0 0 0 1 -1 0 0 0 0 # Xv24 - Xv25 >=0
      0 0 0 0 0 0 0 0 0 1 -1 0 0 0 # Xv25 - Xv26 >=0
    
      0 0 0 0 0 0 0 0 0 0 0 1 0 0 # Xv34>=1
      0 0 0 0 0 0 0 0 0 0 0 1 -1 0 # Xv34 - Xv35 >=0
      0 0 0 0 0 0 0 0 0 0 0 0 1 -1 # Xv35 - Xv36 >=0  
      
      # make this 2 lanes for each U agains all V 
      # First to U0
      -1 0 0 0 0 0 0 0 1 0 0 0 0 0  # Xu00 <= Xv2,l(0) = 4 --> Xv24 - Xu00 >=0
      -1 0 0 0 0 0 0 0 0 0 0 1 0 0  # Xu00 <= Xv3,l(0) = 4 --> Xv34 - Xu00 >=0
      
      0 -1 0 0 0 0 0 0 1 0 0 0 0 0  # Xu01 <= Xv2,l(1) = 4 --> Xv24 - Xu01 >=0
      0 -1 0 0 0 0 0 0 0 0 0 1 0 0  # Xu01 <= Xv3,l(1) = 4 --> Xv34 - Xu01 >=0
      
      0 0 -1 0 0 0 0 0 1 0 0 0 0 0  # Xu02 <= Xv2,l(2) = 4 --> Xv24 - Xu02 >=0
      0 0 -1 0 0 0 0 0 0 0 0 1 0 0  # Xu02 <= Xv3,l(2) = 4 --> Xv34 - Xu02 >=0
      
      0 0 0 -1 0 0 0 0 0 1 0 0 0 0  # Xu03 <= Xv2,l(3) = 5 --> Xv25 - Xu03 >=0
      0 0 0 -1 0 0 0 0 0 0 0 0 1 0  # Xu03 <= Xv3,l(3) = 5 --> Xv35 - Xu03 >=0
      
      # now for U1
      0 0 0 0 -1 0 0 0 1 0 0 0 0 0  # Xu10 <= Xv2,l(0) = 4 --> Xv24 - Xu10 >=0
        #There is no edge
   #  0 0 0 0 -1 0 0 0 0 0 0 1 0 0  # Xu10 <= Xv3,l(0) = 4 --> Xv34 - Xu10 >=0
      
      0 0 0 0 0 -1 0 0 1 0 0 0 0 0  # Xu11 <= Xv2,l(1) = 4 --> Xv24 - Xu11 >=0
      #There is no edge
     # 0 0 0 0 0 -1 0 0 0 0 0 1 0 0  # Xu11 <= Xv3,l(1) = 4 --> Xv34 - Xu11 >=0
      
      0 0 0 0 0 0 -1 0 1 0 0 0 0 0  # Xu12 <= Xv2,l(2) = 4 --> Xv24 - Xu12 >=0
      #There is no edge
    # 0 0 0 0 0 0 -1 0 0 0 0 1 0 0  # Xu12 <= Xv3,l(2) = 4 --> Xv34 - Xu12 >=0
      
      0 0 0 0 0 0 0 -1 0 1 0 0 0 0  # Xu13 <= Xv2,l(3) = 5 --> Xv25 - Xu13 >=0
      #There is no edge
    # 0 0 0 0 0 0 0 -1 0 0 0 0 1 0  # Xu13 <= Xv3,l(3) = 5 --> Xv35 - Xu13 >=0
      
      # indexes = Xu00 Xu01 Xu02 Xu03; Xu10 Xu11 Xu12 Xu13; Xv24 Xv25 Xv26; Xv34 Xv35 Xv36
      # now do the inverse, for V to U
      # for V2
      1 0 0 0 0 0 0 0 -1 0 0 0 0 0  # Xv24 <= Xu0,l(4) =  0--> Xu00 - Xv24 >=0
      0 0 0 0 1 0 0 0 -1 0 0 0 0 0  # Xv24 <= Xu1,l(4) =  0--> Xu10 - Xv24 >=0
      
      0 1 0 0 0 0 0 0 0 -1 0 0 0 0  # Xv25 <= Xu0,l(5) =  1--> Xu01 - Xv25 >=0
      0 0 0 0 0 1 0 0 0 -1 0 0 0 0  # Xv25 <= Xu1,l(5) =  1--> Xu11 - Xv25 >=0
      
      0 0 1 0 0 0 0 0 0 0 -1 0 0 0  # Xv26 <= Xu0,l(6) =  2--> Xu02 - Xv26 >=0
      0 0 0 0 0 0 1 0 0 0 -1 0 0 0  # Xv26 <= Xu1,l(6) =  2--> Xu12 - Xv26 >=0
      
      # for V3
      1 0 0 0 0 0 0 0 0 0 0 -1 0 0  # Xv34 <= Xu0,l(4) =  0--> Xu00 - Xv34 >=0
      #There is no edge
    # 0 0 0 0 1 0 0 0 0 0 0 -1 0 0  # Xv34 <= Xu1,l(4) =  0--> Xu10 - Xv34 >=0
      
      0 1 0 0 0 0 0 0 0 0 0 0 -1 0  # Xv35 <= Xu0,l(5) =  1--> Xu01 - Xv35 >=0
      #There is no edge
    # 0 0 0 0 0 1 0 0 0 0 0 0 -1 0  # Xv35 <= Xu1,l(5) =  1--> Xu11 - Xv35 >=0
      
      0 0 1 0 0 0 0 0 0 0 0 0 0 -1  # Xv36 <= Xu0,l(6) =  2--> Xu02 - Xv36 >=0
      #There is no edge
    # 0 0 0 0 0 0 1 0 0 0 0 0 0 -1  # Xv36 <= Xu1,l(6) =  2--> Xu12 - Xv36 >=0
      
      ]

% right side of the constraints
b = [1;0;0;0;1;0;0;0;1;0;0;1;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0;0]

ub = [];

lb = [0 0 0 0 0 0 0 0 0 0 0 0 0 0 ]; % lower bound - greather than 0

ctype = "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL";

vartype = "CCCCCCCCCCCCCC"; % continuos variables
s = 1; % minimixation problem
param.msglev = 1;

[xmin] = glpk(c, A, b, lb, ub, ctype, vartype, s, param)

result = c * xmin
% example from class

% objective function
% c --> [ps,pb,pg,ph,pd,pt  Xsb,Xsg,Xgb,Xgh,Xbd,Xbh,Xhd,Xht,Xdt
c = [0 0 0 0 0 0, 5,7,5,5,2,6,2,6,5]
% constraints
% have to be in the same sequence as c, each var in its on column
% Xij - pi+pj >=0 --> ps is represented by first colum, pg second, and so on
A = [-1 1 0 0 0 0 1 0 0 0 0 0 0 0 0;
     -1 0 1 0 0 0 0 1 0 0 0 0 0 0 0;
     0 1 -1 0 0 0 0 0 1 0 0 0 0 0 0;
     0 0 -1 1 0 0 0 0 0 1 0 0 0 0 0;
     0 -1 0 0 1 0 0 0 0 0 1 0 0 0 0;
     0 -1 0 1 0 0 0 0 0 0 0 1 0 0 0;
     0 0 0 -1 1 0 0 0 0 0 0 0 1 0 0;
     0 0 0 -1 0 1 0 0 0 0 0 0 0 1 0;
     0 0 0 0 -1 1 0 0 0 0 0 0 0 0 1;
     1 0 0 0 0 -1 0 0 0 0 0 0 0 0 0]
     

% right side of the constraints
b = [0;0;0;0;0;0;0;0;0;1];

lb = [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0]; % lower bound - greather than 0

ub = [];

 
ctype = "LLLLLLLLLL";

vartype = "CCCCCCCCCCCCCCC"; % continuos variables
s = 1; % minimixation problem
param.msglev = 1;

[xmin] = glpk(c, A, b, lb, ub, ctype, vartype, s, param)

result = c * xmin
c = [40 30]; % Objective Function
A = [1 1; 2 1]; % Constraints
B = [240 320]; % array containing the right-side of each constraint

lb = [0,0]; % lower bound - greather than 0
up = []; 
ctype = "UU"; % U for uper bound --> <= 
vartype = "CC"; % continuos variables
s = -1 % Maximization problem
param.msglev = 1;

[xmin, fmin, status, extra] = glpk(c, A, B, lb, up, ctype, vartype, s, param)

%plot graph

X = 0:20:200;
Y1 = -X + 240;
plot(X,Y1,'b', 'LineWidth', 5)

hold on;

Y2 = -2 * X + 320;

plot(X,Y2,'r','LineWidth', 5)
plot(80,160,'go','LineWidth', 8)
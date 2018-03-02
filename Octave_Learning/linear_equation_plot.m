% ploting the graphic
% example 

% https://en.wikipedia.org/wiki/Linear_equation
% y = -x +5
% y = 1/2 x +2

X = -5:0.5:5 % x line, from -5 to 5, incrementing by 0.5

Y1 = -X1 +5

plot(X, Y1, 'b', 'LineWidth', 5)
hold on
Y2 = 0.5 * X + 2

plot(X, Y2, 'r', 'LineWidth', 5)

% wanna make sure it's correct?
% put both equation in other of x and y
% and solve the linear equation
% x + y = 5
% -0.5 + y = 2

A = [1,1; -0.5,1];
B = [5;2];
resul = A\B

plot(resul(1), resul(2), 'go', 'LineWidth', 8)
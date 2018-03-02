% 2x +y +3z =20
% 2x + 6y + 8z = 50
% 6x + 8y + 18z = 98

A = [2,1,3;2,6,8;6,8,18]

B = [20;50;98]

%values = B * inv(A)

values = A\B
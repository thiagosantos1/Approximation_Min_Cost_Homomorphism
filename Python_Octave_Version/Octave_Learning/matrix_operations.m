# matrix criation
a = [1 2;3 19] # 3x3

b = [10,12;8,14] # 3x3

c = a+b # simply add both matrix

d = a*b

e = 3*b

i = eye(3) # creates a matrix with diagonal equal 1 and the rest 0

z = [1 5 7; 6 4 8; 9 7 -4]
#get the diagonal
t = diag(z)
.#get the transpose of a matrix
trans = z'

#calculate the inverse of the matrix

inverse = inv(z)

# calculate the determinat of a matrix

determ = det(z)

%geting values from matrix
% remember that position goes by colum not row like in C & starts with 1 not 0
% thus position 1 is first element in first row, position 2 is first element in row 2 and so on
a(2)       # result is a scalar  - position 2 of matrix
a(1:3)     # result is a row vector - position 1 to 3
a([1; 3])  # result is a column vector - position 1 and 3

%best way to access
a(1, [1, 2])  # row 1, columns 1 and 2
a(1, 1:2)     # row 1, columns in range 1-2
a(1, :)       # row 1, all columns


columns(a) % returns the number of columns
rows(a) % return the number of rows

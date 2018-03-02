function welcome
  disp("welcome to function")
endfunction

welcome()

function sum(a, b)
  printf("Sum %d", a+b)
endfunction

sum(3,5)

function result =  fact(n)
result = 1;
  while(n>0)
    result *= n;
    n--;
  endwhile
endfunction
  
res = fact(5);
printf("Factorial of 5 is %d\n", res);

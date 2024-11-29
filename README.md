# C program for function approximation using method of the least squares
File "input.txt" contains the number of known data points and their x and y coordinates
## Algorithm
1. Ask user if he wants to use his own data set:
   
   If answer == yes then read data from user
   
   Else read data from the input.txt
3. Calculate mean values of x and y coordinates
4. Calculate estimation criterions for 9 types of regression equations and print the table of them
5. Find equation with the minimum estimation criterion
6. Print the fittest equation type and ask user which equation he wants to use
7. Build a regression model:
   1. Linearize the selected equation
   2. Solve the system of linear equations to find the unknown coefficients
   3. Print the equation
   4. Calculate the coefficient of determination and Fisher criterion
8. Print results

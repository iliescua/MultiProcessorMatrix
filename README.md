# MultiProcessorMatrix
The following code is an assignment where we were tasked with creating a matrix multiplication program were you could execute the multiplication function either on a single processor or on multiple by using the fork() command through the Linux OS terminal. Attached are also files containing the matrices that were used in implementation and testing. The makefile script has also been attached and it is used as a way to have access to easy compilation and slight error checking before more intensive measures need to be done. Also, the program has been tested using the valgrind command to ensure that no memory leaks occur.

Note for the matrix files:
  -They were created in the Linux version of notepad, mousepad, and therefore their formtting tends to break when opened in notepad or other similar programs so be warned. 
  -Also the matrix files first line contains the number of rows and columns respectively. This feature is taken advantage of when reading in the files for the multiply function execution.

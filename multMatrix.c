#include "multMatrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/**
 * This is a helper method created which is used to read in the files and
 * populate the the matricies
 * This method exists only in the .c file because it is a helper method
 * that is called on by other methods to read the files and populate
 * the matricies and only exists in the .c file because a user doesn't
 * need to use it and therefore shouldn't have access to it outside of here
 */
void readFiles(char *fOne, char *fTwo, matrix *mOne, matrix *mTwo);

/**
 * This is the implemented method to read the files and populate the matricies
 * that are to be multiplied together from the files
 *
 * Parameters (fOne, fTwo, mOne, mTwo): The parameters here are the
 * files that store the matricies which are passed in from the user in
 * main. The matricies are also passed in so that when the data is read
 * in and stored, it can be used by the calling method
 */
void readFiles(char *fOne, char *fTwo, matrix *mOne, matrix *mTwo) {
  FILE *fileOne = fopen(fOne, "r"); // Loads file one into file variable

  // Checks to make sure a valid file is loaded
  if (fileOne == NULL) {
    printf("Can't Load File\n");
    exit(0);
  }

  // The first two values in the matrix file are row,col and get loaded here
  fscanf(fileOne, "%d", &mOne->row);
  fscanf(fileOne, "%d", &mOne->col);
  // Allocates memory for the size of the matrix and creates pointer to it
  mOne->data = malloc(sizeof(int) * mOne->row * mOne->col);

  // These for loops are used to iterate through the file and store the values
  for (int i = 0; i < mOne->row; i++) {
    for (int j = 0; j < mOne->col; j++) {
      fscanf(fileOne, "%d", &mOne->data[i * mOne->col + j]);
    }
  }
  fclose(fileOne);

  FILE *fileTwo = fopen(fTwo, "r");
  if (fileTwo == NULL) {
    printf("Can't Load File\n");
    exit(0);
  }
  fscanf(fileTwo, "%d", &mTwo->row);
  fscanf(fileTwo, "%d", &mTwo->col);

  // This checks to make sure that the matricies can be muiltiplied together
  if (mOne->col != mTwo->row) {
    printf("Matricies can't be multiplied do to size\n");
    exit(0);
  }

  mTwo->data = malloc(sizeof(int) * mTwo->row * mTwo->col);
  for (int i = 0; i < mTwo->row; i++) {
    for (int j = 0; j < mTwo->col; j++) {
      fscanf(fileTwo, "%d", &mTwo->data[i * mTwo->col + j]);
    }
  }
  fclose(fileTwo);
}

/**
 * This is the method which runs the multiply matrix function as on a
 * single processor and it takes in the name of the two files which contain
 * the matrix as parameters
 */
void singProcess(char *fOne, char *fTwo) {
  // Using time.h library to calculate execution time for multiplication func
  struct timespec start, end;
  clock_gettime(CLOCK_REALTIME, &start);
  int temp = 0;
  int count = 0;
  matrix *mOne = malloc(sizeof(matrix)); // Space is set aside for matrix one
  matrix *mTwo = malloc(sizeof(matrix)); // Space is set aside for matrix two
  matrix *mProduct = malloc(sizeof(matrix)); // Space is set aside for matrix

  // Calls the read method to populate the two passed on matricies
  readFiles(fOne, fTwo, mOne, mTwo);

  // Pointer is created that will store the product matrix
  mProduct->data = malloc(sizeof(int) * mOne->row * mTwo->col);

  // These loops are used to multiply the matrix together and store the result
  for (int i = 0; i < mOne->row; i++) {
    for (int j = 0; j < mTwo->col; j++) {
      for (int k = 0; k < mTwo->row; k++) {
        temp += mOne->data[i * mOne->col + k] * mTwo->data[k * mTwo->col + j];
      }
      mProduct->data[count] = temp;
      count++;
      temp = 0;
    }
  }

  // This ends the clock time and computes the durationof the multiply func
  clock_gettime(CLOCK_REALTIME, &end);
  double totalTime = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1000000000.0;

  // This is used to display the reulting matrix in a nice and readable format
  for (int i = 0; i < mOne->row; i++) {
    for (int j = 0; j < mTwo->col; j++) {
      printf("%d\t", mProduct->data[i * mTwo->col + j]);
    }
    printf("\n");
  }

  // Prints execution time
  printf("\t\t\tExecution time: %fs\n\n", totalTime);

  // Got to free the malloc
  free(mOne->data);
  free(mTwo->data);
  free(mProduct->data);
  free(mOne);
  free(mTwo);
  free(mProduct);
}

/**
 * This is a helper method created which is used to perform the multiplication
 * function as a multi-processor operation
 * This method exists only in the .c file because it is a helper method
 * that is called on by the multProcess method to perform the matrix
 * multiplication and it only exists in the .c file because a user doesn't need
 * to use it and therefore shouldn't have access to it outside of here
 */
void child(int *toggle, matrix *mOne, matrix *mTwo, int loc, int row, int col);

/**
 * This is the child process used for multi-processing which contians the logic
 * for doing matrix multiplication
 *
 * Parameters (toggle, mOne, mTwo, loc, row, col): The parameters here are the
 * toggle function which controls if it is a read or write instruction, the two
 * matricies that were loaded, loc is a count to keep track of the cell that is
 * supposed to be calculated, and row and col are the current row and col so
 * that the calculation can be exectuted
 */
void child(int *toggle, matrix *mOne, matrix *mTwo, int loc, int row, int col) {
  childExec result;

  result.val = 0;
  result.loc = loc;

  for (int i = 0; i < mTwo->row; i++) {
    result.val +=
        mOne->data[row * mOne->col + i] * mTwo->data[mTwo->col * i + col];
  }

  write(toggle[1], &result, sizeof(result));

  close(toggle[1]); // Close Write
}

/**
 * This is the method which runs the multiply matrix function as on a
 * multiple processors and it takes in the name of the two files which
 * contains the matrix as parameters
 */
void multProcess(char *fOne, char *fTwo) {
  int count = 0;
  int complete = 0;
  childExec result;
  matrix *mOne = malloc(sizeof(matrix)); // Space is set aside for matrix one
  matrix *mTwo = malloc(sizeof(matrix)); // Space is set aside for matrix two
  matrix *mProduct = malloc(sizeof(matrix)); // Space is set aside for matrix

  // Calls the read method to populate the two passed on matricies
  readFiles(fOne, fTwo, mOne, mTwo);

  // Using the time.h library to calculate execution time
  struct timespec start, end;
  clock_gettime(CLOCK_REALTIME, &start);

  // Pointer is created that will store the product matrix
  mProduct->data = malloc(sizeof(int) * mOne->row * mTwo->col);

  // This is used to distinguish between read and write
  int toggleRW[2];

  // Pipe failed
  if (pipe(toggleRW) == -1) {
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < mOne->row; i++) {
    for (int j = 0; j < mTwo->col; j++) {
      pid_t pid = fork(); // Fork into 2 processes

      if (pid < 0) { // Error
        printf("Fork failed, terminating\n");
        exit(EXIT_FAILURE);
      } else if (pid == 0) { // Child PID is 0
        close(toggleRW[0]);  // Close Read
        child(toggleRW, mOne, mTwo, count, i, j);

        // Freeing child malloc
        free(mOne->data);
        free(mTwo->data);
        free(mProduct->data);
        free(mOne);
        free(mTwo);
        free(mProduct);
        exit(EXIT_SUCCESS);
      }
      count++;
    }
  }

  close(toggleRW[1]); // Close Write

  // Used to load in the values from the child process into the product matrix
  while ((complete = read(toggleRW[0], &result, sizeof(childExec)))) {
    mProduct->data[result.loc] = result.val;
  }

  // Busy wait, required for proper execution
  for (int i = 0; i < (mOne->row * mTwo->col); i++) {
    wait(0);
  }

  // This is used to display the reulting matrix in a nice and readable format
  for (int i = 0; i < mOne->row; i++) {
    for (int j = 0; j < mTwo->col; j++) {
      printf("%d\t", mProduct->data[i * mTwo->col + j]);
    }
    printf("\n");
  }

  // This ends the clock time and computes the duration and prints it
  clock_gettime(CLOCK_REALTIME, &end);
  double totalTime = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) / 1000000000.0;
  printf("\t\t\tExecution time: %fs\n", totalTime);

  // Freeing parent malloc
  free(mOne->data);
  free(mTwo->data);
  free(mProduct->data);
  free(mOne);
  free(mTwo);
  free(mProduct);
}
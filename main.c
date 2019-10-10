#include "multMatrix.h"
#include <stdio.h>

int main() {
  printf("\t\t\tSingle Processor Execution\n");
  /*
   * Calls the multiply matrix function to be executed on a single processor
   * and takes the fils names (paths if files are in a serperate dir as the
   * program) as parameters and executes the multiplication function
   */
  singProcess("matA", "matB");

  printf("====================================================================="
         "======\n\n\t\t\tMulti-Processor Execution\n");

  /*
   * Calls the multiply matrix function to be executed on multiple processors
   * and takes the fils names (paths if files are in a serperate dir as the
   * program) as parameters and executes the multiplication function
   */
  multProcess("matA", "matB");

  printf("====================================================================="
         "======\n\n");

  printf("\t\t\tSingle Processor Execution\n");

  singProcess("matD", "matE");

  printf("====================================================================="
         "======\n\n\t\t\tMulti-Processor Execution\n");

  multProcess("matD", "matE");

  return 0;
}
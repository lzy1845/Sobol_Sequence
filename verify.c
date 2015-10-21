// FILE: verify.c

/* verifies that the data provided by search.c is valid. */

// Mik Cieslak and Kris Luttmer
// May 22, 2001


#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "verifydata.h"
#include "sobol.h"

#define MAXPOINTS 32768

static int *matrix;
static int res;
static double *point;
static unsigned long numpoints;
static int baddata, goodcount;

int main (int argc, char **argv)
{
  int i, x, y;
  unsigned long j;

  if (!InitSobol (NEWDIM, MAXPOINTS))
  {
    printf ("Cannot initialize Sobol generator.\n");
    return (0);
  } 

  goodcount = 0;
  for (i = 0; i < VERIFYDIM; i++)
  { 
    res = 1 << (verifydata[i][0] >> 1);
    numpoints = 1 << (verifydata[i][2]); 
    matrix = (int *) malloc (sizeof(int) * res * res);
    memset (matrix, 0, sizeof(int) * res * res);

    printf ("VERIFYING DIMENSION %d:\n", i+PREVDIM+1);
    printf ("size of matrix: %d x %d\n", res, res);
    printf ("POINTS: %d\n", numpoints); 

    ResetSobol ();

    for (j = 0; j < numpoints; j++)
    {
      if ((point = Sobol()) == NULL)
      {
        printf ("Cannot generate points.\n");
        free (matrix);
        FreeSobol();
      }

      x = (int) (point[i+PREVDIM] * res);
      y = (int) (point[verifydata[i][1]] * res);

      matrix[x * res + y] += 1;
    }

    /* count the entries of the matrix which do not match matrix[0][0] */
    baddata = 0;
    for (x = 0; x < res; x++)
      for (y = 0; y < res; y++)
        if (matrix[x * res + y] != matrix[0])
	  baddata++;
 
    /* if there is no bad data this direction number is a good one */
    if (baddata == 0)
    {
      printf ("GOOD.\n\n");
      goodcount++;
    }
    else
      printf ("BAD, %.2f%%.\n\n", ((double)baddata/(res*res))*100);
    
    free (matrix);
  }

  FreeSobol();

  printf ("\nGOOD COUNT = %d\n", goodcount);
  printf ("BAD COUNT = %d\n", VERIFYDIM - goodcount);

  return (1);
}

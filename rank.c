/*
  This is a function to compute the rank of a mxn Matrix in which all the
  entries are either 0 or 1.

  The function assumes that the matrix is passed in as a array of intgers.
  The integers represent the rows. For instance

  {2,3,6} =   |010|
              |011|
	      |110|
	      
  Written by: Kris Luttmer and Mik Cieslak
  
*/

#include <stdlib.h>
#include <string.h>
#include "rank.h"

int FindRankMatBits(int * mat, int m, int n)
{

  int j = 0;
  int i = 0;
  int swap = 0; //a swapping variable
  int rank = 0;
  int * matrix = (int*) malloc(sizeof(int)*m*n);
  unsigned int bit_test = 0x80000000;
  char sorted = 0; //set the sorted variable to false
  
  memcpy(matrix, mat, sizeof(int)*m*n);
  
  //the first thing I need to do is sort the rows of the matrix
  //This may or may not be taken out later
  
  //NOTE******
  //There must never be more collums than rows or else you will
  //go out of bounds.

  for(j = 0; j < n; j++)
    {
      //if the j'th bit in the first row is not set
      //interchange this row with one that does have the j'th
      //bit set
      if(!(matrix[j] & bit_test))
	{
	  for(i = 1; i < m; i++)
	    {
	      if(matrix[i] & bit_test)
		{
		  swap = matrix[j];
		  matrix[j] = matrix[i];
		  matrix[i] = swap;
		  break;
		}
	     }
	  }

      //now we test every other row to see if the bit is set in
      //some other row. If it is we XOR the row with the first row
      //else we do nothing
      
      for(i = 0; i < m; i++)
	{
	  if(i == j)
	    continue;
	  
	  if(matrix[i] & bit_test)
	    matrix[i] ^= matrix[j];
	} 
          
      bit_test >>= 1;
    }
  
  //now all we have to do is go and find all the rows that have atleast
  //one 1 in them. This will be the rank of the matrix.
  
  for(i = 0; i < m; i++)
    if(matrix[i] != 0)
      rank++;
  
  free (matrix);
  return rank;
}   

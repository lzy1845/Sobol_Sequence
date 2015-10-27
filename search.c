// FILE: search.c

/* Searches for good coefficients to use with primitive polynomials
   in the Sobol generator. The polynomials are read from a file, and
   are in binary representation. That is, each bit of the long word
   represents x^i, where i is the ith bit. There are two independent
   searches: 1) enumerates all possible values for m(1) to m(degree), 
                and
             2) randomly chooses the vector m(1) to m(degree).
   The criterion for choosing m must be specified by the user in 
   the function Criterion. */

// Mik Cieslak and Kris Luttmer | March 25, 2001


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <malloc/malloc.h>
#include <math.h>
#include "rand.h"
#include "rank.h"
#include "search.h"


// defines
#define MAXPREVDIM 40
#define MAXDEGREE 13 // the maximum degree of any primitive polynomial
#define MAXINT 2147483648.0 // 2^31

// static variables
static int *poly = NULL;
static unsigned *minit = NULL;
static unsigned int *matrix = NULL;
static int newdim = 0;
static int prevdim = 0;
static int maxdegree = 0;
static int lag = 0;
static double weight = 0;
static int *verifydata = NULL;


// prototypes
static int ReadPolyData (void);
/* reads in the primitive polynomials upto the specified dimension. */
static int ReadMinitData (void);
/* reads in the known coefficients upto the specified dimension. */
static int WriteData (char *sobolfilename, char *verifyfilename);
/* writes all of the coefficients in C style to a file. */
static int WriteResolution (char *resfilename);
/* writes all of the resolutions in C style to a file. */


static void SearchAll (void);
/* enumerates all values of (m{1},m{2},...,m{degree}). */
static void SearchRandom (int numrandom);
/* chooses the vector m randomly numrandom times. */
static void SearchProportion (double proportion);
/* chooses the vector m randomly (proportion*sample-space-size) times */


static void Criterion (int nextdim, int m[MAXDEGREE]);
/* the criterion for choosing the coefficients. specified by user. */
static void FillMinit (int dim, int degree);
/* fill the rest of minit after finding some coefficients. */



// --------------------------------------------------------------------

int InitSearch (int Snewdim, int Sprevdim, int Slag, double Sweight)
{
  int i;

  newdim = Snewdim;

  if (Sprevdim < MAXPREVDIM+1)
    prevdim = Sprevdim;
  else
  {
    printf ("previous dimension must be <= %d", MAXPREVDIM);
    return (0);
  }

  lag = Slag;
  weight = Sweight;

  // allocate memory for poly, and read the data from the file poly.dat
  poly = (int *) malloc (sizeof(int) * newdim);
  if (!ReadPolyData ())
  {
    free (poly);
    return (0);
  }

  // calculate maxdegree
  i = 31;
  while (!((*(poly+newdim-1) >> i) & 0x1))
    i--;
  maxdegree = i;

  // allocate memory for minit, and read the data from the file minit.dat
  minit = (int *) malloc (sizeof(int) * newdim * maxdegree);
  if (!ReadMinitData ())
  {
    free (minit);
    free (poly);
    return (0);
  }

  // allocate memory for verify data
  verifydata = (int *) malloc (sizeof(int) * (newdim-prevdim) * 3);
  memset (verifydata, 0, sizeof(int) * (newdim-prevdim) * 3);

  return (1);
}

// --------------------------------------------------------------------

void StartSearch (int method, ...)
{
  va_list ap;

  va_start (ap, method);

  printf ("SEARCHING\n");

  switch (method)
    {
    case SEARCH_ALL:
      SearchAll();
      break;

    case SEARCH_RANDOM:
      SearchRandom (va_arg(ap, int));
      break;

    case SEARCH_PROPORTION:
      SearchProportion (va_arg(ap, double));
      break;
    }

  printf ("DONE\n");

  va_end (ap);

  return;
}

// --------------------------------------------------------------------

void FinishSearch (char *sobolfilename, char *verifyfilename)
{
  WriteData (sobolfilename, verifyfilename);

  free (verifydata);
  free (minit);
  free (poly); // free the memory used by poly
  if (matrix != NULL)
    free (matrix);

  return;
}

// --------------------------------------------------------------------

void FinishSearch_Res (char *resfilename)
{
    WriteResolution (resfilename);
    
    free (verifydata);
    free (minit);
    free (poly); // free the memory used by poly
    if (matrix != NULL)
        free (matrix);
    
    return;
}

// --------------------------------------------------------------------

int ReadPolyData (void)
/* Purpose: read dimension primitive polynomials for file "poly.dat".
       Pre: dimension must be < 1113
      Post: the poly array contains the long word representation of
            the first dimension polynomials. */
{
  int i; // looping variable
  FILE *infile; // the input file

  // open the file
  if ((infile = fopen ("poly.dat", "r")) == NULL)
  {
    printf ("ERROR: cannot open poly.dat file\n");
    return (0);
  }

  // read in the polys
  for (i = 0; i < newdim; i++)
    fscanf (infile, "%d", (poly+i));
    
  fclose (infile); // close the file
  return (1);
}

// --------------------------------------------------------------------

int ReadMinitData (void)
/* Purpose: Read in the known coefficients from "minit.dat".
       Pre: dimension must be < 40.
      Post: the minit array contains the first dimension initial
            values. */
{
  int i, j, k; // looping variable
  FILE *infile; // the input file
  int degree; // the degree of the coefficients in the file
  int pdegree; // the degree of a polynomial
  int temp; // read coefficients minit is not defined for
  int newminit;
  
  // open the file
  if ((infile = fopen ("minit.dat", "r")) == NULL)
  {
    printf ("ERROR: cannot open minit.dat");
    return (0);
  }

  memset (minit, 0, sizeof(int)*newdim*maxdegree);//int minit[newdim*maxdegree];

  // read the degree
  fscanf (infile, "%d", &degree);
    degree=15;
  // the first row is all ones
  for (i = 0; i < maxdegree; i++)
    *(minit+(i*newdim)) = 1;
  // read the rest
  for (i = 1; i < prevdim; i++)
  {
    for (j = 0; j < degree; j++)
      if (j < maxdegree)
        fscanf (infile, "%d", (minit+i+(j*newdim)));
      else
        fscanf (infile, "%d", &temp);

    // find the degree of the polynomial
    pdegree = 31;
    while (!(*(poly+i) >> pdegree) & 0x1)
      pdegree--;
 
    FillMinit (i, pdegree);
  }
    
  fclose (infile); // close the file
  return (1);
}

// --------------------------------------------------------------------
int WriteResolution(char *resfilename)
{
    int i;
    FILE *resfile;
    if ((resfile=fopen(resfilename,"w"))==NULL)
    {
        printf ("ERROR: cannot open output file, %s", resfilename);
        return (0);
    }
    fprintf (resfile, "/* THIS FILE WAS AUTOMATICALLY GENERATED BY ");
    fprintf (resfile, "SEARCH.C IN THE TOOLS LIBRARY */\n\n");
    
    fprintf (resfile, "#define NEWDIM %d\n", newdim);
    fprintf (resfile, "#define PREVDIM %d\n", prevdim);
    fprintf (resfile, "#define VERIFYDIM %d\n\n", newdim-prevdim);
    
    fprintf (resfile, "static int resolution[VERIFYDIM] = {");
    for (i = 0; i < newdim-prevdim; i++)
    {
        fprintf (resfile, "%d", *(verifydata+i));
        if (i != newdim-prevdim-1)
            fprintf (resfile, ",");
    }
    fprintf (resfile, "};\n");
    fclose (resfile);
    
    return (1);
}

// --------------------------------------------------------------------
int WriteData (char *sobolfilename, char *verifyfilename)
/* Purpose: to write the ploy array and the coefficients to a file
       Pre: the minit should be initialized.
      Post: the polys and minits have been written to a file. */
{
  int i, j, pdegree;
  FILE *outfile;
  FILE *verifyfile;

  // open the output file
  if ((outfile = fopen (sobolfilename, "w")) == NULL)
  {
    printf ("ERROR: cannot open output file, %s", sobolfilename);
    return (0);
  }

  fprintf (outfile, "/* THIS FILE WAS AUTOMATICALLY GENERATED BY ");
  fprintf (outfile, "SEARCH.C IN THE TOOLS LIBRARY */\n\n");

  fprintf (outfile, "#define MAXDIM %d\n", newdim);
  fprintf (outfile, "#define MAXDEGREE %d\n\n", maxdegree);

  fprintf (outfile, "static int poly[MAXDIM] = {");
  for (i = 0; i < newdim; i++)
    if (i != newdim-1)
    {
      fprintf (outfile, "%d, ", *(poly+i));
      if (i % 9 == 8)
        fprintf (outfile, "\n                           ");
    }
    else
      fprintf (outfile, "%d};\n", *(poly+i));

  fprintf (outfile, "\n");

  fprintf (outfile, "static unsigned int minit[MAXDIM-1][MAXDEGREE] = {");
  for (i = 1; i < newdim; i++)
  {
    if (i != 0)
      fprintf (outfile, "{");
    else
      fprintf (outfile, "{");

    // find the degree of the polynomial
    pdegree = 31;
    while (!(*(poly+i) >> pdegree) & 0x1)
      pdegree--;

    for (j = 0; j < pdegree; j++)
      if (j != maxdegree-1)
        fprintf (outfile, "%u, ", *(minit+i+(j*newdim)));
      else
        fprintf (outfile, "%u}", *(minit+i+(j*newdim)));
    for (j; j < maxdegree; j++)
      if (j != maxdegree-1)
        fprintf (outfile, "0, ");
      else
        fprintf (outfile, "0}"); 

    if (i != newdim-1)
      fprintf (outfile, ",\n");
  }
  fprintf (outfile, "};\n");
  fclose (outfile);

  // write the verify file
  if ((verifyfile = fopen (verifyfilename, "w")) == NULL)
  {
    printf ("cannot open verify file, %s, for writing", verifyfilename);
    return (0);
  }

  fprintf (verifyfile, "// FILE: %s\n\n", verifyfilename);
  fprintf (verifyfile, "/* THIS FILE WAS AUTOMATICALLY GENERATED BY\n");
  fprintf (verifyfile, "SEARCH.C FROM THE TOOLS LIBRARY. */\n\n");

  fprintf (verifyfile, "#define NEWDIM %d\n", newdim);
  fprintf (verifyfile, "#define PREVDIM %d\n", prevdim);
  fprintf (verifyfile, "#define VERIFYDIM %d\n\n", newdim-prevdim);

  fprintf (verifyfile, "static int verifydata[VERIFYDIM][3] = {");
  for (i = 0; i < newdim-prevdim; i++)
  {
    fprintf (verifyfile, "{%d,", *(verifydata+i));
    fprintf (verifyfile, "%d,", *(verifydata+i+newdim-prevdim));
    fprintf (verifyfile, "%d}", *(verifydata+i+((newdim-prevdim)<<1)));
    if (i != newdim-prevdim-1)
      fprintf (verifyfile, ",\n");
  }
  fprintf (verifyfile, "};\n");

  fclose (verifyfile);

  return (1);
}

// --------------------------------------------------------------------

void SearchAll ()
/* Purpose: to enumerate all values of the vector m between prevdim,
            and newdim.
       Pre: newdim > predim.
      Post: the user specified function Criterion() has been called
            with all enumerations of m.  minit contains chosen values
            for coefficents. */
{
  int i, j; // looping variable
  int m[MAXDEGREE]; // possible coefficients for poly
  int maxM[MAXDEGREE]; // maximum value for m(0)...m(degree)
  int degree; // the degree of a primitive poly

  // for the new dimensions do,
  for (i = prevdim; i < newdim; i++)
  {
    printf ("DIM: %d\n", i+1);

    // find the degree of the (i-1)th poly
    degree = maxdegree;
    while (!((*(poly+i) >> degree) & 0x1))
      degree--;
    
    // initialize m and maxM
    for (j = 0; j < degree; j++)
    {
      m[j] = 1;
      maxM[j] = 2;
    }

    // set all of the maxMs
    for (j = 1; j < degree; j++)
      maxM[j] = maxM[j-1] << 1;
    

    // loop over all possible values for m
    for (j = degree-1; j >= 0; j--)
      while (m[j]+2 < maxM[j])
      {
	Criterion (i, m); // call criterion

        m[j] += 2;
      	while (j < degree-1)
	{
          j++;
          m[j] = 1;
        }  
      }

    Criterion (i, m); // call criterion one final time
  }

  return;
}

// --------------------------------------------------------------------

void SearchRandom (int numrandom)
/* Purpose: to generate the m vector randomly numrandoms times. 
       Pre: newdim > prevdim, numrandom > 0
      Post: user defined Criterion() is called for every m vector
            generated. minit contains chosen values for coefficents. */
{
  int i, j, k; // looping variables
  int m[MAXDEGREE]; // the m vector
  int maxM[MAXDEGREE]; // the restriction that m(i) < 2^i
  int degree; // the degree of a polynomial
  RngStream GenArr; // random number generator

  Rand_CreateStream (&GenArr, "mRandom generator");

  for (i = prevdim; i < newdim; i++)
  {
    printf ("DIM: %d\n", i+1);

    // find the degree of the (i-1)th poly
    degree = maxdegree;
    while (!((*(poly+i) >> degree) & 0x1))
      degree--;
   
    // initialize m and maxM
    for (j = 0; j < degree; j++)
      maxM[j] = 2;

    // set all of the maxMs
    for (j = 1; j < degree; j++)
      maxM[j] = maxM[j-1] << 1;
     
    // randomly choose m(1)...m(degree) numrandoms times
    for (j = 0; j < numrandom; j++)
    {
      // for each m in the vector generate an odd number
      for (k = 0; k < degree; k++)
      {
          m[k] = 2 * Rand_RandInt (GenArr, 1, maxM[k]/2) - 1;
      }
      Criterion (i, m);
        
    }
  
    Rand_ResetStream (GenArr, NextBlock);
  } 

  Rand_DeleteStream (&GenArr);

  return;
}

// --------------------------------------------------------------------

void SearchProportion (double proportion)
/* Purpose: to randomly generate the vector m (proportion * size of
            sample space) times.
       Pre: newdim > predim, 0 < proportion < 1
      Post: the user defined Criterion() has been called for every
            m vector generated. So, minit contains chosen values
            for the coefficents of the primitive polynomials. */
{
  int i, j, k; // looping variables
  int m[MAXDEGREE]; // m vector
  int maxM[MAXDEGREE]; // values for restriction, m(i) < 2^i 
  int degree; // degree of polynomial
  double samplespacesize; // the size of the sample space
  RngStream GenArr; // random number generator
 
  // check the propotion is correct
  if ((proportion < 0) || (proportion > 1))
  {
    printf ("ERROR: Proportion must be > 0 and < 1\n");
    return;
  }

  Rand_CreateStream (&GenArr, "mProportion generator");

  // for all new dimensions do
  for (i = prevdim; i < newdim; i++)
  {
    printf ("DIM: %d\n", i+1);
    // find the degree of the (i-1)th poly
    degree = maxdegree;
    while (!((*(poly+i) >> degree) & 0x1))
      degree--;

    // initialize m and maxM
    for (j = 0; j < degree; j++)
      maxM[j] = 2;

    // set all of the maxMs
    for (j = 1; j < degree; j++)
      maxM[j] = maxM[j-1] << 1;
    
    // calculate the number of random choices to make
    samplespacesize = 1;
    for (j = 0; j < (degree * (degree-1) >> 1); j++)
      samplespacesize *= 2;
        
    // randomly choose m(1)...m(degree) (proportion*samplespace) times
    for (j = 0; j < ((int) (proportion * samplespacesize)); j++)
    { 
      // for each m in the vector generate an odd number
      for (k = 0; k < degree; k++)
        m[k] = 2 * Rand_RandInt (GenArr, 1, maxM[k]/2) - 1;

      Criterion (i, m);
    }
  } 

  Rand_DeleteStream (&GenArr);

  return;
}

// --------------------------------------------------------------------

void Criterion (int nextdim, int m[MAXDEGREE])
/* Purpose: to pick the best m[MAXDEGREE] for dimension nextdim
       Pre: nextdim must be < newdim
      Post: either this m is choosen or not. */
{
  int i, j, k;
  unsigned int value;
  int rank, saverank, savedim;
  int l, stop;
  double initcriterion, alpha;
  static int currdim = 0;
  static int degree = 0; 
  static int maxres;
  static double criterion = MAXINT;
  
  // if a new dimension is given reset the search variables
  if (currdim != nextdim)
  {
    criterion = MAXINT;
    currdim = nextdim; // set the current dimension to search for =8
  
    // find the degree of the currdim(th) primative polynomial
    j = *(poly+currdim);
    degree = maxdegree;
    while (!((*(poly+currdim) >> degree) & 0x1))
      degree--;

    maxres = degree / 2; // number of columns / 2 needed for *matrix

    if (matrix != NULL)
      free (matrix);
   
    // allocate memory for the matrix
    matrix = (int *) malloc (sizeof(int)*degree);
  }
  initcriterion = 0.0;
  saverank = 0;
  savedim = 0;
  for (i = currdim-1; i >= currdim-lag; i--)
  {
    l = maxres;
    stop = 0;
    rank = 0;
    while ((l >= 1) && (stop == 0))
    {
      memset (matrix, 0, sizeof(int)*degree);

      // compute the matrix, for every row
      for (j = 0; j < degree; j++)
      {
        // compute the known half of the matrix
        value = *(minit+i+(j*newdim));//minit[i+j*newdim]

        // find the number of bits in value
        k = 31;
        while (!((value >> k) & 0x1)) //value shift k bits to the right, if the last bit of value>>k is 0, then k--
          k--;

        // get rid of the bits that aren't needed
        if (k >= l)
        {
          k -= l;
          value >>= (k+1); // value = value>>(k+1) = value/2^(k+1)
        }

        // shift the value to the left
        while (!((value >> 31) & 0x1))
          value <<= 1;

        // store the known half of the matrix
        *(matrix+j) = value;

        // compute the new half of the matrix 
        value = m[j];
 
        // find the number of bits in value
        k = 31;
        while ((!((value >> k) & 0x1)) && k>0)
          k--;
 
        // get rid of the bits that are not needed
        if (k >= l)
        {
          k -= l;
          value >>= (k+1);
        }

        // shift the value over to the left
        while (!((value >> 31-l) & 0x1))
          value <<= 1;

        // store the new part of the matrix
        *(matrix+j) += value;
      }

      // compute the rank
      rank = FindRankMatBits (matrix, degree, l << 1);

      if (rank == l << 1)
      {
        stop = 1;
	saverank = rank;
	savedim = i;
      }
      else
        l--;
    }

    // alpha = pow (weight, currdim-i);
    alpha = 1.0;
   
    if (initcriterion < (((double)((maxres<<1)-rank)) * alpha))
    {
      initcriterion = ((double)((maxres<<1)-rank)) * alpha;
    }

    if (initcriterion >= criterion)	
      break;
  }
 
  // if this m was not the worst save it
  if ((initcriterion < criterion) && (rank != 0))
  {
    criterion = initcriterion;
  
    for (j = 0; j < degree; j++)
      *(minit+currdim+(j*newdim)) = m[j];

    FillMinit (currdim, degree); 
    
    *(verifydata+currdim-prevdim) = saverank;
    *(verifydata+currdim-prevdim+(newdim-prevdim)) = savedim;
    *(verifydata+currdim-prevdim+((newdim-prevdim)<<1)) = degree;
  }

  return;  
}

// --------------------------------------------------------------------

void FillMinit (int dim, int degree)
/* fill the coefficients of the (dim)th m in minit with the recurrence
   defined by bratley and fox on page 90. */
{
  int i, j, k;
  int newminit;
  int temp;

  // calculate the missing degrees of minit
  for (j = degree; j < maxdegree; j++)
  {
    newminit = *(minit+dim+((j-degree)*newdim));
      
    temp = 1;
    for (k = degree-1; k >= 0; k--)
    {
      if ((*(poly+dim) >> k) & 0x1)
        newminit = newminit ^ (*(minit+dim+((j-(degree-k))*newdim)) << temp); //XOR
      temp++;
    }

    *(minit+dim+(j*newdim)) = newminit;
  }

  return;
}

// --------------------------------------------------------------------

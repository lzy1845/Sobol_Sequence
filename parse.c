// FILE: main.c

/* */

#include <stdio.h>

int poly[39] = {3,7,11,13,19,25,37,59,47,
		       61,55,41,67,97,91,109,103,115,131,
		       193,137,145,143,241,157,185,167,229,171,
		       213,191,253,203,211,239,247,285,369,299};


int main (void)
{
  int i, j;
  FILE *infile;
  FILE *outfile;
  char oneline[80];
  char skip[80];
  int bits[32];
  int index;
  int number;
    if((infile = fopen ("prim.dat", "r")) == NULL){
        fprintf(stderr,"Cannot open prim.dat\n");
        return -1;
    }
    
    if((outfile = fopen ("poly.dat", "w")) == NULL){
        fprintf(stderr, "Cannot open pply.dat\n");
        return -1;
    }
  fprintf (outfile, "%d\n", 1);

    for (i = 0; i < 39; i++){
    fprintf (outfile, "%d\n", poly[i]);
    }

  for (i = 0; i < 1079; i++)
  {
    number = 0;
      for (j = 0; j < 32; j++){
          bits[j] = 0;
      }
    fgets (skip, 11, infile);

    if (skip[0] == '=')
    {
      fgets (skip, 80, infile);
      continue;
    }

    fgets (skip, 3, infile);
  
    while (skip[0] != 'S')
    {
      if (!(strcmp (skip, "X^")))
      {
        fscanf (infile, "%d", &index);
        bits[index] = 1;
      }

      fgets (skip, 2, infile);
      fgets (skip, 3, infile);

      if ((skip[0] == 'X') && (skip[1] == '+'))
      {
        bits[1] = 1;
        bits[0] = 1;
        skip[0] = 'S';
      }
      if (skip[0] == '1')
      {
        bits[0] = 1;
        skip[0] = 'S';
      }
    }

    fgets (skip, 80, infile);

    for (j = 31; j >= 0; j--)
    {
      number = number << 1;
      number = number + bits[j];
    }

    fprintf (outfile, "%d\n", number);
  }

  fclose (infile);
  fclose (outfile);

  return 0;
}

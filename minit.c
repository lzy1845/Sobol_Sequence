
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main ()
{
  int i;
  char str[80];
  FILE *infile, *outfile;

  if ((infile = fopen ("old.dat", "r")) == NULL)
  {
    fprintf (stderr, "Cannot open file 'old.dat'\n");
    return (0);
  }
  if ((outfile = fopen ("minit.dat", "w")) == NULL)
  {
    fprintf (stderr, "Cannot open file 'minit.dat'\n");
    return (0);
  }

  fprintf (outfile, "8\n");

  while (!feof(infile))
    {
      memset (&str, '\0', 80);

      fgets (str, 80, infile);

      for (i = 0; i < 80; i++)
        if (isdigit(str[i]))
	  {
            while (isdigit(str[i]))
	    {
              fprintf (outfile, "%c", str[i]);
	      i++;
	    }
	    fprintf (outfile, " ");
	  }
      fprintf (outfile, "\n");
    }

  fclose (outfile);
  fclose (infile);

  return (0);
}

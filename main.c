
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "search.h"


int main (int argc, char **argv)
{
  int newdim = 127;
  int prevdim = 8;
  int lag = 8;
  int random_nums = 10000;
  double weight = 1;
  int num_args = argc;
  int value_next_arg = 0;

  while (num_args > 1)
  {
    num_args--;

    if (argv[num_args][0] == '-')
    {
      switch (argv[num_args][1])
      {
        case 'n':
          if (value_next_arg)
            newdim = atoi (argv[num_args+1]);
          else
            newdim = atoi (&argv[num_args][2]);
	  if (newdim < prevdim)
            prevdim = newdim;
          break;

        case 'l':
          if (value_next_arg)
            lag = atoi (argv[num_args+1]);
          else
            lag = atoi (&argv[num_args][2]);
          if (lag > prevdim)
	  {
            fprintf (stderr, "Lag must be <= %d\n", prevdim);
            return (0);
	  }
          break;

        case 'w':
          if (value_next_arg)
            weight = atof (argv[num_args+1]);
          else
            weight = atof (&argv[num_args][2]);
          if ((weight <= 0.0) || (weight > 1.0))
	  {
            fprintf (stderr, "Weight must be > 0.0 and <= 1.0\n");
            return (0);
	  }
          break;

        case 'r':
          if (value_next_arg)
            random_nums = atoi (argv[num_args+1]);
          else
            random_nums = atoi (&argv[num_args][2]);
          if (random_nums <= 0)
	  {
            fprintf (stderr, "Amount of random numbers must be > 0\n");
            return (0);
	  }
          break;

        default:
          fprintf (stderr, "Unknown argument: %s\n", argv[num_args]);
          fprintf (stderr, "Please read the README file\n");
          return (0);
      }
      value_next_arg = 0;
    }
    else if (isdigit(argv[num_args][0]))
    {
      value_next_arg = 1;
    }
    else
    {
      fprintf (stderr, "Unknown argument: %s\n", argv[num_args]);
      fprintf (stderr, "Please read the README file\n");
      return (0);
    }
  }
 
  InitSearch (newdim, prevdim, lag, weight);

  StartSearch (SEARCH_RANDOM, random_nums);

    FinishSearch_Res("resolutiondata.h");

  //FinishSearch ("soboldata.h", "verifydata.h");

  return (0);
}



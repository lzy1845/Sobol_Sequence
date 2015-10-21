README file for Sobol Search - Mik Cieslak


1) Compiling

The following only has to been done once.
Run 'make'. This will create three files 'search', 'minit', and 'parse'.
Run 'minit' and run 'parse'.



2) Searching

Run 'search'.

As an option, 'search' accepts four arguments:
SEARCH DIMENSION is the upper bound on the search.
LAG is how far back the search will compare dimensions.
WEIGHT is the weight each dimension compared has.
RANDOM NUMBERS is the amount of random tries per dimension.   

- the default arguments are -
search dimension = 360
             lag = 8
          weight = 1.0
  random numbers = 10,000

- passing arguments through the command line -

  replace (x) with an appropriate value 

  a) -n(x)
    search dimension = x

  b) -l(x)
    lag = x (0 < x <= 40)

  c) -w(x)
    weight = x (0.0 < x <= 1.0)

  d) -r(x)
    random numbers = x

NOTE:
 - arguments can be in any order and not all of them must be specified



3) After Searching

Run 'make verify'. This will create a binary 'verify'.
Run 'verify'.
The search data generated will be verified and the amount of 'good data'
versus 'bad data' will be displayed.

NOTE:
 - 'verify' must be recomplied after every search!!!



4) Additional Notes

Running 'make clean' will remove all of the unnecessary files (object files).
Running 'make delete' will remove all of the files created (not source files).

There are three types of searches possible:
 1) search through all numbers
 2) search through N random numbers
 3) search through a proportion P of numbers

Only number 2 can be run through the command line.  For the other methods,
the file 'main.c' must be changed.  The call to StartSearch() must be
changed.

// FILE: search.c

/* */

// Mik Cieslak and Kris Luttmer

#ifndef _SEARCH_H
#define _SEARCH_H


#define SEARCH_ALL        1
#define SEARCH_RANDOM     2
#define SEARCH_PROPORTION 3

// ---------------------------------------------------------------------

int InitSearch (int newdim, int prevdim, int lag, double weight);

void StartSearch (int method, ...);

void FinishSearch (char *sobolfilename, char *verifyfilename);

void FinishSearch_Res (char *resfilename);

// ---------------------------------------------------------------------


#endif

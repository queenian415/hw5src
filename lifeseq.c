/*****************************************************************************
 * life.c
 * The original sequential implementation resides here.
 * Do not modify this file, but you are encouraged to borrow from it
 ****************************************************************************/
#include "life.h"
#include "util.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


/**
 * Swapping the two boards only involves swapping pointers, not
 * copying values.
 */
#define SWAP_BOARDS( b1, b2 )  do { \
  char* temp = b1; \
  b1 = b2; \
  b2 = temp; \
} while(0)

#define BOARD( __board, __i, __j )  (__board[(__i) + LDA*(__j)])

#define NUM_THREADS 16
#define BLOCK_SIZE 64

int nrows, ncols, LDA;
char* outboard;
char* inboard;
int slice;


void*
parallel_run(void* args) {
  int n = (intptr_t) args;
  int rows_from = n * slice;
  int rows_to = rows_from + slice;
  int i, j, ii, jj, inorth, isouth, jwest, jeast;

      for (j = 0; j < ncols; j += BLOCK_SIZE) {

  for (i = rows_from; i < rows_to; i += BLOCK_SIZE) {
      for (ii = i; ii < i + BLOCK_SIZE; ii++) {
	if (ii-1 < 0)
	  inorth = nrows;
	else if (ii - 1 > nrows - 1)
	  inorth = 0;
	else
	  inorth = ii - 1;

	if (ii + 1 < 0)
	  isouth = nrows;
	else if (ii + 1 > nrows - 1)
	  isouth = 0;
	else
	  isouth = ii + 1;
	
	  for (jj = j; jj < j + BLOCK_SIZE; jj++) {
	    if (jj - 1 < 0)
	      jwest = ncols;
	    else if (jj - 1 > ncols - 1)
	      jwest = 0;
	    else
	      jwest = jj - 1;

	    if (jj + 1 < 0)
	      jeast = ncols;
	    else if (jj + 1 > ncols - 1)
	      jeast = 0;
	    else
	      jeast = jj + 1;
	    
                const char neighbor_count =
                    BOARD (inboard, inorth, jwest) +
		    BOARD (inboard, inorth, jj) +
		    BOARD (inboard, inorth, jeast) +
		    BOARD (inboard, isouth, jwest) +
		    BOARD (inboard, isouth, jj) +
		    BOARD (inboard, isouth, jeast) +
		    BOARD (inboard, ii, jwest) +
                    BOARD (inboard, ii, jeast);

                BOARD(outboard, ii, jj) = alivep (neighbor_count, BOARD (inboard, ii, jj));

            }
	}
    }
  }
}

    char*
sequential_game_of_life (char* outboard_, 
        char* inboard_,
        const int nrows_,
        const int ncols_,
        const int gens_max)
{
    /* HINT: in the parallel decomposition, LDA may not be equal to
       nrows! */
  nrows = nrows_;
  ncols = ncols_;
  outboard = outboard_;
  inboard = inboard_;
  LDA = nrows;
  slice =  (nrows / NUM_THREADS);
  
  if (nrows_ <= 32 && ncols_ <= 32) {
    //const int LDA = nrows;
    int curgen, i, j;

    for (curgen = 0; curgen < gens_max; curgen++)
    {
        for (i = 0; i < nrows; i++)
        {
            for (j = 0; j < ncols; j++)
            {
                const int inorth = mod (i-1, nrows);
                const int isouth = mod (i+1, nrows);
                const int jwest = mod (j-1, ncols);
                const int jeast = mod (j+1, ncols);

                const char neighbor_count = 
                    BOARD (inboard, inorth, jwest) +
		    BOARD (inboard, i, jwest) +
		    BOARD (inboard, isouth, jwest) +
                    BOARD (inboard, inorth, j) + 
                    BOARD (inboard, i, jeast) +
		    BOARD (inboard, isouth, jeast) +
		    BOARD (inboard, inorth, jeast) +
                    BOARD (inboard, isouth, j);

                BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));

            }
        }
        SWAP_BOARDS( outboard, inboard );

    }
  } else {
    int curgen, i;
    pthread_t *thread = (pthread_t*)malloc(NUM_THREADS * sizeof(pthread_t));

    for (curgen = 0; curgen < gens_max; curgen++)
      {
	for (i = 0; i < NUM_THREADS; i++)
	   pthread_create (&thread[i], NULL, parallel_run, (void*)i);
	
	
	for (i = 0; i < NUM_THREADS; i++) pthread_join (thread[i], NULL);

       
        SWAP_BOARDS( outboard, inboard );

      }
    }
    /* 
     * We return the output board, so that we know which one contains
     * the final result (because we've been swapping boards around).
     * Just be careful when you free() the two boards, so that you don't
     * free the same one twice!!! 
     */
    return inboard;
}



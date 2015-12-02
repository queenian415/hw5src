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

#define BOARD( __board, __i, __j )  (__board[LDA*(__i) + (__j)])

#define NUM_THREADS 16

int nrows, ncols, LDA;
char* outboard;
char* inboard;
int pid[NUM_THREADS] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
int slice;


void*
parallel_run(void* args) {
  int n = (intptr_t) args;
  int rows_from = n * slice;
  int rows_to = rows_from + slice;
  int i, j;

   for (i = rows_from; i < rows_to; i++)
        {
            for (j = 0; j < ncols; j++)
            {
                int inorth = mod (i-1, nrows);
                int isouth = mod (i+1, nrows);
                int jwest = mod (j-1, ncols);
                int jeast = mod (j+1, ncols);

                const char neighbor_count =
                    BOARD (inboard, inorth, jwest) +
		    BOARD (inboard, inorth, j) +
		    BOARD (inboard, inorth, jeast) +
		    BOARD (inboard, isouth, jwest) +
		    BOARD (inboard, isouth, j) +
		    BOARD (inboard, isouth, jeast) +
		    BOARD (inboard, i, jwest) +
                    BOARD (inboard, i, jeast);

                BOARD(outboard, i, j) = alivep (neighbor_count, BOARD (inboard, i, j));

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
  LDA = ncols;
  slice =  (nrows / NUM_THREADS);
  
  if (nrows_ <= 32 && ncols_ <= 32) {
    const int LDA = nrows;
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
	pthread_create (&thread[1], NULL, parallel_run, (void*)pid[1]);
        pthread_create (&thread[2], NULL, parallel_run, (void*)pid[2]);
        pthread_create (&thread[3], NULL, parallel_run, (void*)pid[3]);
        pthread_create (&thread[4], NULL, parallel_run, (void*)pid[4]);
        pthread_create (&thread[5], NULL, parallel_run, (void*)pid[5]);
        pthread_create (&thread[6], NULL, parallel_run, (void*)pid[6]);
        pthread_create (&thread[7], NULL, parallel_run, (void*)pid[7]);
        pthread_create (&thread[8], NULL, parallel_run, (void*)pid[8]);
        pthread_create (&thread[9], NULL, parallel_run, (void*)pid[9]);
        pthread_create (&thread[10], NULL, parallel_run, (void*)pid[10]);
        pthread_create (&thread[11], NULL, parallel_run, (void*)pid[11]);
        pthread_create (&thread[12], NULL, parallel_run, (void*)pid[12]);
        pthread_create (&thread[13], NULL, parallel_run, (void*)pid[13]);
        pthread_create (&thread[14], NULL, parallel_run, (void*)pid[14]);
        pthread_create (&thread[15], NULL, parallel_run, (void*)pid[15]);
      
	/* master thread is thread 0 so: */
	parallel_run(0);
	
	
	for (i = 1; i < NUM_THREADS; i++) pthread_join (thread[i], NULL);

       
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



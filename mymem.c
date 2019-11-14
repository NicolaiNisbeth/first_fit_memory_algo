#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#include <time.h>
#define MATH_MAX(a, b) ((a > b) ? a : b)



/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */

struct memory_block {
    // doubly-linked list
    struct memory_block *prev;
    struct memory_block *next;

    int size;            // How many bytes in this block?
    char alloc;          // 1 if this block is allocated,
    // 0 if this block is free.
    void *ptr;           // location of block in memory pool.
};


strategies myStrategy = First   ;    // Current strategy
size_t mySize;

void *myMemory = NULL;
static struct memory_block *head;

/* initmem must be called prior to mymalloc and myfree.

   initmem may be called more than once in a given exeuction;
   when this occurs, all memory you previously malloc'ed  *must* be freed,
   including any existing bookkeeping data.

   strategy must be one of the following:
		- "best" (best-fit)
		- "worst" (worst-fit)
		- "first" (first-fit)
		- "next" (next-fit)
   sz specifies the number of bytes that will be available, in total, for all mymalloc requests.
*/

void initmem(strategies strategy, size_t sz){
	myStrategy = strategy;

	/* all implementations will need an actual block of memory to use */
	mySize = sz;

	if (myMemory != NULL) free(myMemory); /* in case this is not the first time initmem2 is called */

	/* TODO: release any other memory you were using for bookkeeping when doing a re-initialization! */

	while (head != NULL){
	    head->size = 0;
	    head->alloc = 0;
	    free(head->ptr);
	    head = head->next;
	}


	myMemory = malloc(sz);
	
	/* TODO: Initialize memory management structure. */


    head = (struct memory_block*) malloc(sizeof (struct memory_block));
    head->prev = NULL;
    head->next = NULL;
    head->size = sz; // initialy the first block size is equals to the memory pool size.
    head->alloc = '0';  // not allocated
    head->ptr = myMemory;  // points to the same memory adress as the memory pool
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested){
	assert((int)myStrategy > 0);
    struct memory_block *fit = head;

    switch (myStrategy){
        case NotSet:
            return NULL;
        case First:

            while (fit != NULL){
                // fit = find first fit (requested)
                // newBlock = alloc block(fit, requested)
                if (fit->size >= requested && fit->alloc == '0'){
                    struct memory_block *newBlock = (struct memory_block*) malloc(sizeof (struct memory_block));
                    int fit_is_head = fit->prev == NULL, fit_is_tail = fit->next == NULL, fit_is_empty_after_alloc = ((fit->size -= requested) == 0);

                    newBlock->size = requested;
                    newBlock->ptr = malloc(requested);
                    newBlock->alloc = '1';

                    if (fit_is_empty_after_alloc){
                        if (fit_is_head){
                            if (fit->next != NULL){
                                newBlock->next = fit->next;
                                fit->next->prev = newBlock;
                            }
                            head = newBlock;
                        }
                        else if (fit_is_tail){
                            newBlock->prev = fit->prev;
                            fit->prev->next = newBlock;
                        }
                        else {
                            newBlock->prev = fit->prev;
                            fit->prev->next = newBlock;

                            newBlock->next = fit->next;
                            fit->next->prev = newBlock;
                        }
                    }
                    else {
                        newBlock->prev = fit;

                        if (fit->next != NULL){
                            fit->next->prev = newBlock;
                            newBlock->next = fit->next;
                        }

                        fit->next = newBlock;
                    }

                    return newBlock->ptr;

                }
                fit = fit->next;
            }
            return NULL;

	  case Best:
	            return NULL;
	  case Worst:
	            return NULL;
	  case Next:
	            return NULL;
	}
	return NULL;
}


/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block){

    struct memory_block *temp = head;

    while (temp != NULL){
        if (temp->ptr == block){
            // TODO: update temp ptr with prev ptr and next ptr
            int flag = 0;

            // free in front
            if (temp->next != NULL && temp->next->alloc == '0'){
                struct memory_block *front = temp->next;
                temp->size += front->size;
                temp->ptr = realloc(temp->ptr, temp->size);

                if (front->next != NULL){
                    temp->next = front->next;
                    front->next->prev = temp;
                }
                else {
                    temp->next = NULL;
                    flag++;
                }

                free(front->ptr);
            }

            // free behind
            if (temp->prev != NULL && temp->prev->alloc == '0'){
                struct memory_block *behind = temp->prev;
                temp->size += behind->size;
                temp->ptr = realloc(temp->ptr, temp->size);

                if (behind->prev != NULL){
                    temp->prev = behind->prev;
                    behind->prev->next = temp;
                }
                else {
                    temp->prev = NULL;
                    flag++;
                }

                free(behind->ptr);
            }

            if (flag == 2) head = temp;

            free(temp->ptr);

            temp->alloc = '0';
            return;
        }

        temp = temp->next;
    }

}

/****** Memory status/property functions ******
 * Implement these functions.
 * Note that when refered to "memory" here, it is meant that the 
 * memory pool this module manages via initmem/mymalloc/myfree. 
 */

/* Get the number of contiguous areas of free space in memory. */
int mem_holes(){
    int holes = 0;
    struct memory_block *temp = head;

    while (temp != NULL){
        if (temp->alloc == '0') holes++;
        temp = temp->next;
    }
    return holes;
}

/* Get the number of bytes allocated */
int mem_allocated(){
    int bytes = 0;
    struct memory_block *temp = head;
    while (temp != NULL){
        if (temp->alloc == '1'){
            bytes += temp->size;
        }
        temp = temp->next;
    }
    return bytes;
}

/* Number of non-allocated bytes */
int mem_free(){
    int bytes = 0;
    struct memory_block *temp = head;
    while (temp != NULL){
        if (temp->alloc == '0') bytes += temp->size;
        temp = temp->next;
    }
    return bytes;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free(){
    int current = 0, max = 0;
    struct memory_block *temp = head;

    while (temp != NULL){
        if (temp->alloc == '0') current += temp->size;
        else {
            max = MATH_MAX(max, current);
            current = 0;
        }
        temp = temp->next;
    }
    return MATH_MAX(max, current);
}

/* Number of free blocks smaller than "size" bytes. */
int mem_small_free(int size){
    int blocks = 0;
    struct memory_block *temp = head;
    while (temp != NULL){
        if (temp->alloc == '0' && temp->size < size) blocks++;
        temp = temp->next;
    }
    return blocks;

}

/* Is a particular byte allocated or not */
char mem_is_alloc(void *ptr){
    struct memory_block *temp = head;
    while (temp != NULL){
        if (temp->ptr == ptr)return '1';
        temp = temp->next;
    }
    return '0';
}

/* 
 * Feel free to use these functions, but do not modify them.  
 * The test code uses them, but you may find them useful.
 */


//Returns a pointer to the memory pool.
void *mem_pool(){
	return myMemory;
}

// Returns the total number of bytes in the memory pool. */
int mem_total(){
	return mySize;
}


// Get string name for a strategy. 
char *strategy_name(strategies strategy){
	switch (strategy){
		case Best:
			return "best";
		case Worst:
			return "worst";
		case First:
			return "first";
		case Next:
			return "next";
		default:
			return "unknown";
	}
}

// Get strategy from name.
strategies strategyFromString(char * strategy){
	if (!strcmp(strategy,"best")){
		return Best;
	}
	else if (!strcmp(strategy,"worst")){
		return Worst;
	}
	else if (!strcmp(strategy,"first")){
		return First;
	}
	else if (!strcmp(strategy,"next")){
		return Next;
	}
	else{
		return 0;
	}
}


/* 
 * These functions are for you to modify however you see fit.  These will not
 * be used in tests, but you may find them useful for debugging.
 */

/* Use this function to print out the current contents of memory. */
void print_memory(){
    struct memory_block *temp = head;

    /*
    while (temp != NULL){
        printf("%p <- (%p) -> %p || ", temp->prev, temp, temp->next);
        temp = temp->next;
    }
    */

    printf("\n");

    temp = head;

    while (temp != NULL){
        printf("%s %d(%s) %s", temp->prev ? "<-" : "null <-", temp->size, temp->alloc == '1' ? "alloc" : "free", temp->next != NULL ? "->" : "-> null");
        temp = temp->next;
    }
    printf("\n\n");
}

/* Use this function to track memory allocation performance.  
 * This function does not depend on your implementation, 
 * but on the functions you wrote above.
 */ 
void print_memory_status(){
	printf("%d out of %d bytes allocated.\n",mem_allocated(), mem_total());
	printf("%d bytes are free in %d holes; maximum allocatable block is %d bytes.\n",mem_free(),mem_holes(),mem_largest_free());
	printf("Average hole size is %f.\n\n",((float)mem_free())/(float)mem_holes());
}

/* Use this function to see what happens when your malloc and free
 * implementations are called.  Run "mem -try <args>" to call this function.
 * We have given you a simple example to start.
 */
void try_mymem(int argc, char **argv) {
        strategies strat;
	void *a, *b, *c, *d, *e, *f, *g, *h, *k, *l;
	if(argc > 1)
	  strat = strategyFromString(argv[1]);
	else
	  strat = First;
	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */
	
	initmem(strat,500);
	
	a = mymalloc(45);
	b = mymalloc(100);
    c = mymalloc(310);
    myfree(b);
	d = mymalloc(90);
	e = mymalloc(5);
	myfree(d);
	g = mymalloc(90);

	myfree(a);
	h = mymalloc(41);
	k = mymalloc(39);
	myfree(g);
	myfree(h);
	l = mymalloc(142);

	
	print_memory();
	print_memory_status();
	
}

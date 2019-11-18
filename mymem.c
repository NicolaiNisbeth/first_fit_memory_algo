#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "mymem.h"
#define MATH_MAX(a, b) ((a > b) ? a : b)

/* The main structure for implementing memory allocation.
 * You may change this to fit your implementation.
 */
struct memory_block {
    // doubly-linked list
    struct memory_block *prev;
    struct memory_block *next;

    int size;               // Bytes in block
    char alloc;             // 1 if allocated, 0 if free.
    void *ptr;              // location of block in memory pool.
};

strategies myStrategy = First   ;    // Current strategy
size_t mySize;

void *myMemory = NULL;
static struct memory_block *head;
static struct memory_block *temp;
static struct memory_block *lastAlloc;

void *findFirstFit(size_t requested);
void *findBestFit(size_t requested);
void *findWorstFit(size_t requested);
void *findNextFit(size_t requested);
void *allocBlock(struct memory_block *fit, size_t requested);

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

    while (head != NULL){
        temp = head;
        head = head->next;
        free(temp);
    }
    lastAlloc = NULL;


    myMemory = malloc(sz);

    // taken from Armandas Rokas
    head = (struct memory_block*) malloc(sizeof (struct memory_block));
    head->prev = NULL;
    head->next = NULL;
    head->size = sz;
    head->alloc = '0';
    head->ptr = myMemory;  // points to the same memory address as the memory pool
}

/* Allocate a block of memory with the requested size.
 *  If the requested block is not available, mymalloc returns NULL.
 *  Otherwise, it returns a pointer to the newly allocated block.
 *  Restriction: requested >= 1 
 */

void *mymalloc(size_t requested){
    assert((int)myStrategy > 0);
    struct memory_block *fit;

    switch (myStrategy){
        case NotSet:
            return NULL;
        case First:
            fit = findFirstFit(requested);
            return allocBlock(fit, requested);
        case Best:
            fit = findBestFit(requested);
            return allocBlock(fit, requested);
        case Worst:
            fit = findWorstFit(requested);
            return allocBlock(fit, requested);
        case Next:
            fit = findNextFit(requested);
            return allocBlock(fit, requested);
        default:
            return NULL;
    }
}

void *findFirstFit(size_t requested){
    temp = head;
    while (temp != NULL){
        if (temp->alloc == '0' && temp->size >= requested) return temp;
        temp = temp->next;
    }
    return NULL;
}

void *findBestFit(size_t requested){
    temp = head;
    struct memory_block *best = NULL;

    while (temp != NULL){
        if (temp->alloc == '0' && temp->size >= requested){
            if (best == NULL) best = temp;

            if (temp->size < best->size) best = temp;
        }
        temp = temp->next;
    }
    return best;
}

void *findWorstFit(size_t requested){
    temp = head;
    struct memory_block *worst = NULL;

    while (temp != NULL){
        if (temp->alloc == '0' && temp->size >= requested){
            if (worst == NULL) worst = temp;

            if (temp->size > worst->size) worst = temp;
        }
        temp = temp->next;
    }
    return worst;
}

// TODO: bug
void *findNextFit(size_t requested){
    if (lastAlloc != NULL) {
        temp = lastAlloc->next;

        // traverse from lastAlloc till end
        while (temp != NULL){
            if (temp->alloc == '0' && temp->size >= requested) return temp;
            temp = temp->next;
        }
    }

    temp = head;

    // traverse from head till lastAlloc
    while (temp != lastAlloc){
        if (temp->alloc == '0' && temp->size >= requested) return temp;
        temp = temp->next;
    }

    return NULL;
}


void *allocBlock(struct memory_block *fit, size_t requested){
    if (fit == NULL) return NULL;

    int memo_remainder = fit->size - (int)requested;

    if (memo_remainder > 0){
        struct memory_block *freeBlock = (struct memory_block*) malloc(sizeof (struct memory_block));
        freeBlock->ptr = fit->ptr + requested; // ensures correct offset
        freeBlock->size = memo_remainder;
        freeBlock->alloc = '0';

        // position freeBlock to the right of fit
        freeBlock->prev = fit;
        freeBlock->next = fit->next;

        if (fit->next != NULL)
            fit->next->prev = freeBlock;

        fit->next = freeBlock;
    }

    fit->alloc = '1';
    fit->size = requested;

    lastAlloc = fit;

    return fit->ptr;
}

/* Frees a block of memory previously allocated by mymalloc. */
void myfree(void* block){
    temp = head;

    while (temp != NULL){
        if (temp->ptr == block){
            int nextBlockIsFree = (temp->next != NULL) && (temp->next->alloc == '0');
            int prevBlockIsFree = (temp->prev != NULL) && (temp->prev->alloc == '0');

            if (nextBlockIsFree){
                struct memory_block *nextFree = temp->next;
                temp->size += nextFree->size;

                // change pointers to skip the node to be freed
                temp->next = nextFree->next;
                if (nextFree->next != NULL) nextFree->next->prev = temp;

                free(nextFree);
            }

            if (prevBlockIsFree){
                struct memory_block *prevFree = temp->prev;
                temp->size += prevFree->size;

                // change pointers to skip the node to be freed
                temp->prev = prevFree->prev;
                if (prevFree->prev != NULL) prevFree->prev->next = temp;
                else head = temp;

                free(prevFree);
            }

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
    temp = head;
    while (temp != NULL){
        if (temp->alloc == '0') holes++;
        temp = temp->next;
    }
    return holes;
}

/* Get the number of bytes allocated */
int mem_allocated(){
    int bytes = 0;
    temp = head;
    while (temp != NULL){
        if (temp->alloc == '1') bytes += temp->size;
        temp = temp->next;
    }
    return bytes;
}

/* Number of non-allocated bytes */
int mem_free(){
    int bytes = 0;
    temp = head;
    while (temp != NULL){
        if (temp->alloc == '0') bytes += temp->size;
        temp = temp->next;
    }
    return bytes;
}

/* Number of bytes in the largest contiguous area of unallocated memory */
int mem_largest_free(){
    int current = 0, max = 0;
    temp = head;
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
    int free_blocks = 0;
    temp = head;
    while (temp != NULL){
        if (temp->alloc == '0' && temp->size <= size) free_blocks++;
        temp = temp->next;
    }
    return free_blocks;
}

/* Is a particular byte allocated or not */
char mem_is_alloc(void *ptr){
    temp = head;
    while (temp != NULL){
        if (temp->ptr == ptr) return '1';
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
    /*
    temp = head;
    while (temp != NULL){
        printf("%p <- (%p)%d -> %p || ", temp->prev, temp, temp->size, temp->next);
        temp = temp->next;
    }
     */
    printf("\n");
    temp = head;
    while (temp != NULL){
        printf("%s %d(%s) %s", temp->prev != NULL ? "<-" : "NULL <-",
                                      temp->size,
                                      temp->alloc == '1' ? "alloc" : "free",
                                      temp->next != NULL ? "->" : "-> NULL");
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
	  strat = Next;
	
	
	/* A simple example.  
	   Each algorithm should produce a different layout. */

    initmem(strat,1000);

    a = mymalloc(10);
    b = mymalloc(20);
    c = mymalloc(30);
    d = mymalloc(100);
    e = mymalloc(840);

    myfree(b);
    myfree(d);

    f = mymalloc(20);

    myfree(a);

    g = mymalloc(100);
    h = mymalloc(5);

    /*
    a = mymalloc(10);
    b = mymalloc(2);
    myfree(a);
    c = mymalloc(1);
    myfree(c);
	d = mymalloc(3);
    e = mymalloc(1);
    myfree(d);
    myfree(e);
    g = mymalloc(90);
    //myfree(a);
    h = mymalloc(41);
    k = mymalloc(39);
    //myfree(k);
    myfree(g);
    //myfree(h);
    f = mymalloc(495);
    //myfree(e);

    l = mymalloc(142);

    myfree(b);
     */

	print_memory();
	print_memory_status();
	
}

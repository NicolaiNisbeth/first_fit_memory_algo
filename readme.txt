User manual
    download the zip file firstfit.zip
    change directory to where the downloaded zip file is located
    write the command "unzip firstfit.zip" to unzip the file
    write the command "make" to create required executable files
    write the command "./mem -try first" to execute my test case
    write the command "./mem -test all first" to execute Bhupjit's test cases

The application is implemented in the programming language C (version C11) and is intended for Unix systems.

I was given the task to implement the first fit algorithm.
The implementation utilizes the data structure memory_block - designed by Bhupjit - for implementing memory allocation.
I have written two methods:
    (1) findFirstFit(...) loops through the double linked list to find the first block that is free and equal or
    bigger than the requested size.
    (2) allocBlock(...) allocates the requested memory in the free block returned by findFirstFit(...).

My implementation divides the fitted block into partitions to satisfy a memory request as suitable as possible. The
newly allocated block takes the place of the fitted block to ensure the lowest memory address. This results in the
fitted block being recreated to the right of the newly allocated block. An example is given below.

NULL <- 500(free) -> NULL
NULL <- 100(alloc) <-> 400(free) -> NULL                        // mymalloc(100)
NULL <- 100(alloc) <-> 50(alloc) <-> 350(free) -> NULL          // mymalloc(50)

/* Shout out to Armandas Rokas */

Furthermore my implementation handles subsequent free blocks by merging them. An example is given below.

NULL <- 70(free) <-> 50(alloc) <-> 200(alloc) <-> 100(free)
NULL <- 120(free) <-> 200(alloc) <-> 100(free) -> NULL          // myfree(50), 50 as arg is for illustration purposes.
NULL <- 400(free) -> NULL                                       // myfree(200)


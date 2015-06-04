/*
Name: Justin Ting
SID: 430203826
Tutor: Jeshua
Tutorial Time: Monday 10am
*/

#ifndef mab_h
#define mab_h

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

/* memory management *******************************/

#define MEMORY_SIZE       1024
#define RT_MEMORY_SIZE    64
#define USER_MEMORY_SIZE  (MEMORY_SIZE - RT_MEMORY_SIZE)

struct mab
{
	int offset;
	int size;
	int allocated;
	struct mab * left;
	struct mab * right;
	struct mab * parent;
};

typedef struct mab Mab;

typedef Mab * MabPtr;

int blockSizeNeeded(int size);				//Get smallest power of 2 larger than memory requirement


/* memory management function prototypes ********/

Mab * memChk(Mab * m, int size);		// check if memory available
Mab * memAlloc(Mab * m, int size);	// allocate memory block
Mab * memFree(Mab * m);							// free memory block

Mab * memMerge(Mab * m);						// merge two memory blocks
Mab * memSplit(Mab * m, int size);	// split a memory block

Mab * createUserMem(void);

Mab * createRTMem(void);

void printBuddyTree(Mab * m);

#endif

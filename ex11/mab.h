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

MabPtr memChk(MabPtr m, int size);		// check if memory available
MabPtr memAlloc(MabPtr m, int size);	// allocate memory block
MabPtr memFree(MabPtr m);							// free memory block

MabPtr memMerge(MabPtr m);						// merge two memory blocks
MabPtr memSplit(MabPtr m, int size);	// split a memory block

MabPtr createUserMem(void);

MabPtr createRTMem(void);

void printBuddyTree(MabPtr m);

#endif
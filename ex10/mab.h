#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

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

MabPtr memChk(MabPtr m, int size);		// check if memory available
MabPtr memAlloc(MabPtr m, int size);	// allocate memory block
MabPtr memFree(MabPtr m);							// free memory block

MabPtr memMerge(MabPtr m);						// merge two memory blocks
MabPtr memSplit(MabPtr m, int size);	// split a memory block

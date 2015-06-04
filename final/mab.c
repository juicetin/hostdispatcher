/*
Name: Justin Ting
SID: 430203826
Tutor: Jeshua
Tutorial Time: Monday 10am
*/

#include "mab.h"

/*************************************************
 * int BlockSizeNeeded (int size):
 * 		given the actual block size needed for a process,
 * returns:
 * 		the smallest power of 2 that equates/exceeds
 * 		it
 * ***********************************************/
int blockSizeNeeded(int size)
{
	int block_size = 1;
	while (block_size < size)
	{
		block_size <<= 1;
	}

	return block_size;
}

/*************************************************
 * Mab * memChk(Mab * m, int size):
 * 		find if there is a correctly size block available
 * 		to allocate to the process in question
 * returns:
 * 		Mab * containing info on a correctly sized
 * 		block
 * ***********************************************/
Mab * memChk(Mab * m, int size)
{
	int block_size = blockSizeNeeded(size);
	Mab * valid = NULL;

	/* Return current block if size is already correct,
	 * or if children have been spawned. The fact that
	 * children will have spawned indicates that at least
	 * one block will have been allocated at some leaf
	 * node stemming from the current Mab node, meaning
	 * there isn't enough free space*/
	if (m->size == block_size && !m->left && !m->right)
	{
		if (m->allocated == 0)
		{
			return m;
		}
		else
		{
			return NULL;
		}
	}
	/* Continue splitting if right block size hasn't been
	 * reached yet, child buddies haven't been created yet,
	 * and current block size is still too large */
	else if (!m->left && !m->right && m->size > block_size)
	{
		m = memSplit(m, m->size);

		/* Halt attempt if unable to split*/
		if (!m)
		{
			return NULL;
		}
	}

	/* Recurse into left and right branches
	 * until appropriate block can be allocated */
	while (1)
	{
		if (m->left)
		{
			valid = memChk(m->left, block_size);
		}

		/* Only recurse into the right side if none
		 * has been found on the left */
		if (m->right && !valid)
		{
			valid = memChk(m->right, block_size);
		}

		return valid;
	}
}

/*************************************************
 * Mab * memSplit(Mab * m, int size):
 * 		splits an unused memory block into two equal 
 * 		left and right children
 * returns:
 * 		a pointer to the successfully (original)
 * 		parent of the two split blocks
 *
 * 		NULL if no further split is possible (i.e.
 * 		if all blocks are fully allocated or if
 * 		smallest block has been reached)
 * ***********************************************/
Mab * memSplit(Mab * m, int size)
{
	if (size >= 1 && m->allocated != m->size)
	{
		int child_size = m->size/2;
		Mab *left_child, *right_child;
		left_child = malloc(sizeof(Mab));
		right_child = malloc(sizeof(Mab));

		left_child->size = child_size;
		left_child->allocated = 0;
		left_child->parent = m;
		left_child->offset = m->offset;

		right_child->size = child_size;
		right_child->allocated = 0;
		right_child->parent = m;
		right_child->offset = m->offset + child_size;

		m->left = left_child;
		m->right = right_child;

		return m;
	}
	return NULL;
}

/*************************************************
 * Mab * memAlloc(Mab * m, int size):
 * 		allocates memory for a process given a size 
 * returns:
 * 		Mab * to Mab of successfully allocated mem.
 * 		NULL is malloc failed
 * ***********************************************/
Mab * memAlloc(Mab * m, int size)
{
	Mab * needed = memChk(m, size);
	if (needed)
	{
		needed->allocated = blockSizeNeeded(size);
	}
	return needed;
}

/*************************************************
 * Mab * memFree(Mab * m):
 * 		frees the memory indicated by the info inside 
 * 		the Mab at m by marking it as free
 * returns:
 * 		Mab * of the freed block
 * ***********************************************/
Mab * memFree(Mab * m)
{
	if (m)
	{
		m->allocated = 0;
		//printf("freed %d\n", m->size);

		/* Recursively merge buddies together until
		 * a buddy is found that has allocated memory */
		while(m)
		{
			m = memMerge(m);
		}
	}
	return m;
}

/*************************************************
 *	Mab * memMerge(Mab * m):
 *		merges two unused
 *		blocks back into a larger block
 *	returns:
 *		Mab * of parent block 
 *
 *		NULL if can't go further up the tree,
 *		i.e., current Mab block has a NULL
 *		parent
 *
 *		NULL if it's buddy isn't also
 *		unallocated, as an indicator from the
 *		function from which memMerge is called
 *		that it should stop recursing up the
 *		tree
 * ***********************************************/
Mab * memMerge(Mab * m)
{
	if (!m->parent || !m->parent->left || m->parent->right)
	{
		return NULL;
	}

	Mab * parent = m->parent;
	if (parent->left->left || parent->left->right || 	  /* Check that parent's left child has no children */
			parent->right->left || parent->right->right)  /* Check that parent's right child has no children */
	{
		return NULL;
	}

	// Free allocated memory and mark absence of child nodes
	free(parent->left);
	free(parent->right);
	parent->left = NULL;
	parent->right = NULL;

	return parent;
}


/*************************************************
 * Mab * createUserMem(void):
 * 		allocates the main memory block - root 
 * 		node of buddy tree
 * returns:
 * 		pointer to the root buddy node
 * ***********************************************/
Mab * createUserMem(void)
{
	Mab * user_mem;
	user_mem = malloc(sizeof(Mab));
	user_mem->size = MEMORY_SIZE;
	user_mem->allocated = 0;
	user_mem->offset = 0;
	user_mem->left = NULL;
	user_mem->right = NULL;
	user_mem->parent = NULL;
	return user_mem;
}

/*************************************************
 * Mab * createRTMem(void):
 * 		allocates the realtime memory block - root
 * 		node of buddy tree
 * returns:
 * 		pointer to the root buddy node
 * ***********************************************/
Mab * createRTMem(void)
{
	Mab * rt_mem;
	rt_mem = malloc(sizeof(Mab));
	rt_mem->size = RT_MEMORY_SIZE;
	rt_mem->allocated = 0;
	rt_mem->offset = 0;
	rt_mem->left = NULL;
	rt_mem->right = NULL;
	rt_mem->parent = NULL;
	return rt_mem;
}

/*************************************************
 * void printBuddyTree(Mab * m)
 * 		takes the root node of a buddy tree and
 * 		prints all the memory allocated at all nodes
 * ***********************************************/
void printBuddyTree(Mab * m)
{
	if (m->left && m->right)
	{
		printf("[%d]%d/%d [%d]%d/%d\n", 
				m->left->offset, m->left->allocated, m->left->size, 
				m->right->offset, m->right->allocated, m->right->size); 
		printBuddyTree(m->left);
		printBuddyTree(m->right);
	}
}

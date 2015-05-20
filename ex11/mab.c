#include "mab.h"

/*************************************************
 * int BlockSizeNeeded (int size) - given the actual
 * 		block size needed for a process,
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
 * MabPtr memChk(MabPtr m, int size) - find if
 * 		there is a correctly size block available
 * 		to allocate to the process in question
 * returns:
 * 		MabPtr containing info on a correctly sized
 * 		block
 * ***********************************************/
MabPtr memChk(MabPtr m, int size)
{
	int block_size = blockSizeNeeded(size);
	MabPtr valid = NULL;

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
 * MabPtr memAlloc(MabPtr m, int size) - allocates
 * 		memory for a process given a size (NOT SURE;
 * 		COULD BE JUST FOR THE MAB ITSELF?)
 * returns:
 * 		MabPtr to Mab of successfully allocated mem.
 * 		NULL is malloc failed
 * ***********************************************/
MabPtr memAlloc(MabPtr m, int size)
{
	MabPtr needed = memChk(m, size);
	if (needed)
	{
		needed->allocated = blockSizeNeeded(size);
	}
	return needed;
}

/*************************************************
 * MabPtr memFree(MabPtr m) - frees the memory 
 * 		indicated by the info inside the Mab at m
 * 		by marking it as free
 * returns:
 * 		MabPtr of the freed block
 * ***********************************************/
MabPtr memFree(MabPtr m)
{
	m->allocated = 0;

	/* Recursively merge buddies together until
	 * a buddy is found that has allocated memory */
	while(m)
	{
		m = memMerge(m);
	}

	return m;
}

/*************************************************
 *	MabPtr memMerge(MabPtr m) - merges two unused
 *			blocks back into a larger block
 *	returns:
 *			MabPtr of parent block 
 *
 *			NULL if can't go further up the tree,
 *			i.e., current Mab block has a NULL
 *			parent
 *
 *			NULL if it's buddy isn't also
 *			unallocated, as an indicator from the
 *			function from which memMerge is called
 *			that it should stop recursing up the
 *			tree
 * ***********************************************/
MabPtr memMerge(MabPtr m)
{
	if (!m->parent || !m->parent->left || !m->parent->right)
	{
		return NULL;
	}

	MabPtr parent = m->parent;
	if (parent->left->allocated != 0 || parent->right->allocated != 0)
	{
		return NULL;
	}
	free(parent->left);
	free(parent->right);
	parent->left = NULL;
	parent->right = NULL;

	return m->parent;
}

/*************************************************
 * MabPtr memSplit(MabPtr m, int size) - splits an
 * 		unused memory block into two equal left and
 * 		right children
 * returns:
 * 		a pointer to the successfully (original)
 * 		parent of the two split blocks
 *
 * 		NULL if no further split is possible (i.e.
 * 		if all blocks are fully allocated or if
 * 		smallest block has been reached)
 * ***********************************************/
MabPtr memSplit(MabPtr m, int size)
{
	if (size >= 1 && m->allocated != m->size)
	{
		int child_size = m->size/2;
		MabPtr left_child, right_child;
		left_child = malloc(sizeof(Mab));
		right_child = malloc(sizeof(Mab));

		left_child->size = child_size;
		left_child->allocated = 0;
		left_child->parent = m;

		right_child->size = child_size;
		right_child->allocated = 0;
		right_child->parent = m;

		m->left = left_child;
		m->right = right_child;

		return m;
	}
	return NULL;
}

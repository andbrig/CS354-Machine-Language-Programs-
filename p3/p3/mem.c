////////////////////////////////////////////////////////////////////////////////
// Main File:        mem.c
// This File:        mem.c
// Other Files:      dump.c mem.h mem.o
// Semester:         CS 354 Fall 2016
//
// Author:           Andrew Briggs
// Email:            briggs@cs.wisc.edu
// CS Login:         briggs
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"

/* this structure serves as the header for each block */
typedef struct block_hd{
	/* The blocks are maintained as a linked list */
	/* The blocks are ordered in the increasing order of addresses */
	struct block_hd* next;

	/* The size_status field is the size of the payload+padding and is always a multiple of 4 */
	/* ie, last two bits are always zero - can be used to store other information*/
	/* LSB = 0 => free block */
	/* LSB = 1 => allocated/busy block */

	/* So for a free block, the value stored in size_status will be the same as the block size*/
	/* And for an allocated block, the value stored in size_status will be one more than the block size*/

	/* The value stored here does not include the space required to store the header */

	/* Example: */
	/* For a block with a payload of 24 bytes (ie, 24 bytes data + an additional 8 bytes for header) */
	/* If the block is allocated, size_status should be set to 25, not 24!, not 23! not 32! not 33!, not 31! */
	/* If the block is free, size_status should be set to 24, not 25!, not 23! not 32! not 33!, not 31! */
	int size_status;

}block_header;

/* Global variable - This will always point to the first block */
/* ie, the block with the lowest address */
block_header* list_head = NULL;


/* Function used to Initialize the memory allocator */
/* Not intended to be called more than once by a program */
/* Argument - sizeOfRegion: Specifies the size of the chunk which needs to be allocated */
/* Returns 0 on success and -1 on failure */
int Mem_Init(int sizeOfRegion)
{
	int pagesize;
	int padsize;
	int fd;
	int alloc_size;
	void* space_ptr;
	static int allocated_once = 0;

	if(0 != allocated_once)
	{
		fprintf(stderr,"Error:mem.c: Mem_Init has allocated space during a previous call\n");
		return -1;
	}
	if(sizeOfRegion <= 0)
	{
		fprintf(stderr,"Error:mem.c: Requested block size is not positive\n");
		return -1;
	}

	/* Get the pagesize */
	pagesize = getpagesize();

	/* Calculate padsize as the padding required to round up sizeOfRegio to a multiple of pagesize */
	padsize = sizeOfRegion % pagesize;
	padsize = (pagesize - padsize) % pagesize;

	alloc_size = sizeOfRegion + padsize;

	/* Using mmap to allocate memory */
	fd = open("/dev/zero", O_RDWR);
	if(-1 == fd)
	{
		fprintf(stderr,"Error:mem.c: Cannot open /dev/zero\n");
		return -1;
	}
	space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (MAP_FAILED == space_ptr)
	{
		fprintf(stderr,"Error:mem.c: mmap cannot allocate space\n");
		allocated_once = 0;
		return -1;
	}

	allocated_once = 1;

	/* To begin with, there is only one big, free block */
	list_head = (block_header*)space_ptr;
	list_head->next = NULL;
	/* Remember that the 'size' stored in block size excludes the space for the header */
	list_head->size_status = alloc_size - (int)sizeof(block_header);

	return 0;
}


/* Function for allocating 'size' bytes. */
/* Returns address of allocated block on success */
/* Returns NULL on failure */
/* Here is what this function should accomplish */
/* - Check for sanity of size - Return NULL when appropriate */
/* - Round up size to a multiple of 4 */
/* - Traverse the list of blocks and allocate the best free block which can accommodate the requested size */
/* -- Also, when allocating a block - split it into two blocks when possible */
/* Tips: Be careful with pointer arithmetic */
void* Mem_Alloc(int size)
{
	if(size <= 0) {
		return NULL;
	}
	block_header* curr = NULL;
	curr = list_head;
	int tempStatus = 0;
	int loopCounter = 0;
	block_header *compareBlock = list_head;
	int compareBlockStatus = 0;
	int loopDone = 0;
	// Check size and round it up to a multiple of 4
	while(size % 4 != 0) {
		size++;
	}
	// Search for the best fit block in the free list
	while(loopDone == 0) {
		tempStatus = curr->size_status;
		compareBlockStatus = compareBlock->size_status;
		if((tempStatus >= size) && (!(tempStatus % 2))) {
			if(size == tempStatus) {
				(curr->size_status)++;
				return ((void*)curr + sizeof(block_header));
			}
			//Check if it is first iteration
			if(loopCounter == 0) {
				compareBlock = curr;
				loopCounter++;
			}
			else {
				if(tempStatus < compareBlockStatus) {
					compareBlock = curr;
				}	
			}			
		}
		if(curr->next!=NULL) {
			curr = curr->next;
		}
		else {
			loopDone++;
		}
	}
	//Code to see if splitting is possible
	void *blockSplit = (void*)compareBlock + sizeof(block_header) + size;
	int difference = compareBlock->size_status - size;
	// If a block is found, check to see if we can split it,
	// i.e it has space leftover for a new block(header + payload)
	if((difference - size) < 12) {
		(compareBlock->size_status)++;
		return ((void*)compareBlock + sizeof(block_header));
	}
	// If split, update the size of the resulting blocks
	block_header *block1 = (block_header*)blockSplit;
	compareBlock->size_status = size + 1;
	//set new next variables and size status
	block1->size_status = difference - sizeof(block_header);
	block1->next = compareBlock->next;
	compareBlock->next = block1;
	// Mark the allocated block and return it 
	return ((void*)compareBlock + sizeof(block_header));
}

/* Function for freeing up a previously allocated block */
/* Argument - ptr: Address of the block to be freed up */
/* Returns 0 on success */
/* Returns -1 on failure */
/* Here is what this function should accomplish */
/* - Return -1 if ptr is NULL */
/* - Return -1 if ptr is not pointing to the first byte of a busy block */
/* - Mark the block as free */
/* - Coalesce if one or both of the immediate neighbours are free */
int Mem_Free(void *ptr)
{
	if(ptr == NULL) {
		return -1;
	}
	block_header *left = NULL;

	block_header *blockToFree = (block_header*)(ptr - sizeof(block_header));
	int freeBlockStatus = blockToFree->size_status;
	
	// Check if the pointer is pointing to the start of the payload of an allocated block
	if(!(freeBlockStatus % 2)) {
		return -1;
	}
	// Free the block
	blockToFree->size_status = blockToFree->size_status - 1;
	//Check if blocks can be coalesced
	if(blockToFree != list_head) {
		left = list_head;
		//continually loop until left block is correct, or one before current block
		while(left->next != blockToFree) {
			left = left->next;
		}
	}
	//Make sure block has a right block and is not the last block
	if(blockToFree->next != NULL) {
		block_header *right = blockToFree->next;
		//checks to see if right block can be coalesced
		if(!(right->size_status % 2)) {
			blockToFree->size_status = blockToFree->size_status + right->size_status + (int)sizeof(block_header);
			blockToFree->next = right->next;
		}
	}
	//checks to see if left block can be coalesced
	if(left != NULL) {
		if(!(left->size_status % 2)) {
			left->size_status = left->size_status + blockToFree->size_status + (int)sizeof(block_header);
			left->next = blockToFree->next;
		}
	}
	return 0;
}

/* Function to be used for debug */
/* Prints out a list of all the blocks along with the following information for each block */
/* No.      : Serial number of the block */
/* Status   : free/busy */
/* Begin    : Address of the first useful byte in the block */
/* End      : Address of the last byte in the block */
/* Size     : Size of the block (excluding the header) */
/* t_Size   : Size of the block (including the header) */
/* t_Begin  : Address of the first byte in the block (this is where the header starts) */
void Mem_Dump()
{
	int counter;
	block_header* current = NULL;
	char* t_Begin = NULL;
	char* Begin = NULL;
	int Size;
	int t_Size;
	char* End = NULL;
	int free_size;
	int busy_size;
	int total_size;
	char status[5];

	free_size = 0;
	busy_size = 0;
	total_size = 0;
	current = list_head;
	counter = 1;
	fprintf(stdout,"************************************Block list***********************************\n");
	fprintf(stdout,"No.\tStatus\tBegin\t\tEnd\t\tSize\tt_Size\tt_Begin\n");
	fprintf(stdout,"---------------------------------------------------------------------------------\n");
	while(NULL != current)
	{
		t_Begin = (char*)current;
		Begin = t_Begin + (int)sizeof(block_header);
		Size = current->size_status;
		strcpy(status,"Free");
		if(Size & 1) /*LSB = 1 => busy block*/
		{
			strcpy(status,"Busy");
			Size = Size - 1; /*Minus one for ignoring status in busy block*/
			t_Size = Size + (int)sizeof(block_header);
			busy_size = busy_size + t_Size;
		}
		else
		{
			t_Size = Size + (int)sizeof(block_header);
			free_size = free_size + t_Size;
		}
		End = Begin + Size - 1;
		fprintf(stdout,"%d\t%s\t0x%08lx\t0x%08lx\t%d\t%d\t0x%08lx\n",counter,status,(unsigned long int)Begin,(unsigned long int)End,Size,t_Size,(unsigned long int)t_Begin);
		total_size = total_size + t_Size;
		current = current->next;
		counter = counter + 1;
	}
	fprintf(stdout,"---------------------------------------------------------------------------------\n");
	fprintf(stdout,"*********************************************************************************\n");

	fprintf(stdout,"Total busy size = %d\n",busy_size);
	fprintf(stdout,"Total free size = %d\n",free_size);
	fprintf(stdout,"Total size = %d\n",busy_size+free_size);
	fprintf(stdout,"*********************************************************************************\n");
	fflush(stdout);
	return;
}

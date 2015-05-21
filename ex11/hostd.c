/*******************************************************************
	printf("%s\n", inputliststream);

	OS Eercises - Project 2 - HOST dispatcher - Exercise 7

	hostd

	hostd is fcfs 'dispatcher' that reads in a list of 'jobs' from a file
	and 'dispatches' them in a first-come-first-served manner.

	time resolution is one second (although this can be changed). 

	usage

	hostd <dispatch file>

	where
	<dispatch file> is list of process parameters as specified 
	for assignment 2.

	functionality

	1. Initialize dispatcher queue;
	2. Fill dispatcher queue from dispatch list file;
	3. Start dispatcher timer;
	4. While there's anything in the queue or there is a currently running process:
	i. If a process is currently running;
	a. Decrement process remainingcputime;
	b. If times up:
	A. Send SIGINT to the process to terminate it;
	B. Free up process structure memory
	ii. If no process currently running &&
	dispatcher queue is not empty &&
	arrivaltime of process at head of queue is <= dispatcher timer:
	a. Dequeue process and start it (fork & exec)
	b. Set it as currently running process;
	iii. sleep for one second;
	iv. Increment dispatcher timer;
	v. Go back to 4.
	5. Exit

 ********************************************************************

version: 1.0 (exercise 7)
date:    December 2003
author:  Dr Ian G Graham, ian.graham@griffith.edu.au

 *******************************************************************/

#include "hostd.h"

#define VERSION "1.0"

/******************************************************

	internal functions

 ******************************************************/

char * StripPath(char*);
void PrintUsage(FILE *, char *);
void SysErrMsg(char *, char *);
void ErrMsg(char *, char *);
bool fbQueuesNotNull (PcbPtr *fbqueue);

/******************************************************/

int main (int argc, char *argv[]) {
	
	char * inputfile;				// job dispatch file
	FILE * inputliststream;
	PcbPtr inputqueue = NULL;     	// input queue 
	PcbPtr realtimebuffer = NULL;
	PcbPtr realtimequeue = NULL;	// real time job queue buffer
	PcbPtr userjobqueue = NULL;	 	// user job queue 
	PcbPtr fbqueue[3];				// feedback queues 
	for (int i = 0; i < 3; ++i)
	{
		fbqueue[i] = NULL;
	}
	PcbPtr currentprocess = NULL; // current process
	PcbPtr process = NULL;        // working pcb pointer
	int timer = 0;                // dispatcher timer 

	//  0. Parse command line
	if (argc == 2) inputfile = argv[1];
	else PrintUsage (stderr, argv[0]);

	//  1. Initialize dispatcher queue;
	//     (already initialised in assignments above)

	//	2. Initialise memory and resource allocation structures
	//	Create root node of memory allocation blocks
	//	for buddy system
	MabPtr memory = malloc(sizeof(Mab));
	memory->size = 1024;
	memory->allocated = 0;
	memory->offset = 0;
	memory->left = NULL;
	memory->right = NULL;
	memory->parent = NULL;

	//	Create node to hold available I/O resources
	RsrcPtr resources = malloc(sizeof(Rsrc));
	resources->printers = 2;
	resources->scanners = 1;
	resources->modems = 1;
	resources->cds = 2;

	//  3. Fill dispatcher queue from dispatch list file;
	if (!(inputliststream = fopen(inputfile, "r"))) { // open it
		SysErrMsg("could not open dispatch list file:", inputfile);
		exit(2);
	}

	while (1) {
		int val_check;
		process = createnullPcb();
		if ((val_check = fscanf(inputliststream, "%d, %d, %d, %d, %d, %d, %d, %d",
						&(process->arrivaltime), &(process->priority),
						&(process->remainingcputime), &(process->mbytes),
						&(process->req.printers), &(process->req.scanners),
						&(process->req.modems), &(process->req.cds))) == 8) {
			process->status = PCB_INITIALIZED;
			process->mab_block = NULL;
			inputqueue = enqPcb(inputqueue, process);
		}
		else if (val_check >= 0){
			free(process);
			process = NULL;
			ErrMsg("Invalid input file", inputfile);
			exit(2);
		}
		else if (ferror(inputliststream)) {
			free(process); process = NULL;
			SysErrMsg("Error reading input file", inputfile);
			exit(2);
		}
		else {
			free(process);
			break;
		}
	}
	//  4. Start dispatcher timer;
	//     (already set to zero above)

	//	5. While there's anything in any of the queues or there is a currenly running process
	while (inputqueue || fbQueuesNotNull(fbqueue) || userjobqueue
			|| currentprocess /*|| realtimequeue*/)
	{
		// i. Unload pending processes from the input queue
		// 		While (head-of-input-queue.arrival-time <= dispatch timer)
		// 		dequeue process from input queue and enqueue on user job queue
		while (inputqueue && inputqueue->arrivaltime <= timer)
		{
			PcbPtr tmp = deqPcb(&inputqueue);
			
			//	a. Real-time queue or
			if (tmp->priority == 0)
			{
				realtimequeue = enqPcb(realtimequeue, tmp);
			}

			//	b. User job queue
			else
			{
				userjobqueue = enqPcb(userjobqueue, tmp);
			}
		}

		// ii. Unload pending processes from user job queue:
		//	while (head-of-user-job-queue.mbytes can be allocated and resources
		//	available)

		while (userjobqueue)
		{
			//TODO - MAJOR
			/*	Change rsrChk and memChk to do just that - only CHECK, so that
			 *	withi nthe if loop, the actual allocation can do the checking so
			 *	I can remove this shitty bandage solution of preventing a pass
			 *	and fail from allocaint mem/rsrcs*/

			MabPtr allocated_mem = memAlloc(memory, userjobqueue->mbytes);

			if (allocated_mem && rsrcChk(resources,userjobqueue->req))
			{
				int priority = userjobqueue->priority - 1;
				//	a. dequeue process from user job queue
				PcbPtr tmp = deqPcb(&userjobqueue);

				//	b. allocate memory to the process
				tmp->mab_block = allocated_mem;

				//	c. allocate resources to the process
				RsrcPtr allocated_Rsrc = rsrcAlloc(resources, tmp->req);

				//	d. enqueue on appropriate priority feedback queue
				fbqueue[priority] = enqPcb(fbqueue[priority], tmp);
			}
			else
			{
				break;
			}
		}

		//	iii. if a process is currently running
		if (currentprocess && currentprocess->status == PCB_RUNNING)
		{
			// a. Decrement remainingcputime;
			currentprocess->remainingcputime--;

			// b. if time's up
			if (currentprocess->remainingcputime == 0)
			{
				// A. terminate process
				terminatePcb(currentprocess);

				// B. Free memory and resources allocated to process (resources
				// user only)
				rsrcFree(resources, currentprocess->req);
				memFree(currentprocess->mab_block);

				// C. free up process structure memory
				free(currentprocess);
				currentprocess = NULL;
			}

			// TODO (requirement changed)
			//	c. else if it is a user process and other processes are waiting
			//	in any of the queues
			else if (currentprocess->priority != 0 || fbQueuesNotNull(fbqueue) || realtimequeue)
			{
				// A. suspend process
				suspendPcb(currentprocess);
				// B. reduce priority of process (if possible) and enqueue it on appropriate fbqueue
				if (currentprocess->priority <= 2)
				{
					currentprocess->priority = currentprocess->priority + 1;
				}
				int index = currentprocess->priority - 1;
				fbqueue[index] = enqPcb(fbqueue[index], currentprocess);
				currentprocess = NULL;
			}
		}

		//	iv. If no process currently running and real time queue and feedback
		//	queue are not all empty
		if (!currentprocess && (fbQueuesNotNull(fbqueue)/* || realtimequeue*/))
		{
			// a. Dequeue a process from the highest priority feedback queue that is not empty
			//	A. Real time queue takes priority 
			//if (realtimequeue)
			//{
			//	MabPtr allocated_mem = memAlloc(memory, realtimequeue->mbytes);
			//	if (allocated_mem)
			//	{
			//		currentprocess = deqPcb(&realtimequeue);
			//		currentprocess->mab_block = allocated_mem;
			//	}
			//}
			//	B. Once real time queue is cleared, then give feedback
			//	queues service
			if (fbQueuesNotNull(fbqueue))
			{
				int fb = -1;
				for (int i = 0; i < 3; ++i)
				{
					if (fbqueue[i] != NULL)
					{
						fb = i;
						break;
					}
				}
				if (fb != -1)
				{
					currentprocess = deqPcb(&fbqueue[fb]);	
				}
			}

			// b. If already started but suspended, resume process
			// else start process
			// c. Set it as currently running process
			startPcb(currentprocess);
			currentprocess->status = PCB_RUNNING;
		}



		// v. sleep for one second
		sleep(1);

		// vi. increment dispatch timer
		timer++;
		// vii. go back to 4
	}

	//	5. Exit
	exit (0);
}

/****

  Checks whether any feedback queues contain queued processes

 ****/
bool fbQueuesNotNull (PcbPtr *fbqueue)
{
	for (int i = 0; i < 3; ++i)
	{
		if (fbqueue[i])
		{
			return true;
		}
	}
	return false;
}

/*******************************************************************

  char * StripPath(char * pathname);

  strip path from file name

  pathname - file name, with or without leading path

  returns pointer to file name part of pathname
  if NULL or pathname is a directory ending in a '/'
  returns NULL
 *******************************************************************/

char * StripPath(char * pathname) {
	char * filename = pathname;

	if (filename && *filename) {           // non-zero length string
		filename = strrchr(filename, '/'); // look for last '/'
		if (filename)                      // found it
			if (*(++filename))             //  AND file name exists
				return filename;
			else
				return NULL;
		else
			return pathname;               // no '/' but non-zero length string
	}                                      // original must be file name only
	return NULL;
}

/*******************************************************
 * print usage
 ******************************************************/
void PrintUsage(FILE * stream, char * progname) {
	if (!(progname = StripPath(progname))) {
		progname = DEFAULT_NAME;
	}

	fprintf(stream,"\n"
			"%s process dispatcher (version " VERSION "); usage:\n\n"
			"  %s <dispatch file>\n"
			" \n"
			"  where <dispatch file> is list of process parameters \n\n",
			progname,progname);

	exit(127);
}
/********************************************************
 * print an error message on stderr
 *******************************************************/

void ErrMsg(char * msg1, char * msg2) {
	if (msg2) {
		fprintf(stderr,"ERROR - %s %s\n", msg1, msg2);
	}
	else {
		fprintf(stderr,"ERROR - %s\n", msg1);
	}
	return;
}

/*********************************************************
 * print an error message on stderr followed by system message
 *********************************************************/

void SysErrMsg(char * msg1, char * msg2) {
	if (msg2) {
		fprintf(stderr,"ERROR - %s %s; ", msg1, msg2);
	}
	else {
		fprintf(stderr,"ERROR - %s; ", msg1);
	}
	perror(NULL);
	return;
}


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
bool fbQueuesNotNull (Pcb * *fbqueue);

/******************************************************/

int main (int argc, char *argv[]) {

	char * inputfile;				// job dispatch file
	FILE * inputliststream;
	Pcb * inputqueue = NULL;     	// input queue 
	Pcb * realtimebuffer = NULL;
	Pcb * realtimequeue = NULL;	// real time job queue buffer
	Pcb * userjobqueue = NULL;	 	// user job queue 
	Pcb * fbqueue[PRIORITY_QUEUES];				// feedback queues 
	for (int i = 0; i < PRIORITY_QUEUES; ++i)
	{
		fbqueue[i] = NULL;
	}
	Pcb * currentprocess = NULL; // current process
	Pcb * process = NULL;        // working pcb pointer
	int timer = 0;                // dispatcher timer 

	//  0. Parse command line
	if (argc == 2) inputfile = argv[1];
	else PrintUsage (stderr, argv[0]);

	//  1. Initialize dispatcher queue;
	//     (already initialised in assignments above)

	//	2. Initialise memory and resource allocation structures
	//	Create root node of memory allocation block for buddy system, and allocate reserved RT memory
	//	the memAlloc for RT_MEMORy_SIZE is merely a 'dummy' allocate to reserve 64mb for real time processes
	Mab * memory = createUserMem();
	memAlloc(memory, RT_MEMORY_SIZE);
	Mab * rt_memory = createRTMem();
	Rsrc * resources = createRsrcs();
	
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
			|| currentprocess || realtimebuffer)
	{
		// i. Unload pending processes from the input queue
		// 		While (head-of-input-queue.arrival-time <= dispatch timer)
		// 		dequeue process from input queue and enqueue on user job queue
		while (inputqueue && inputqueue->arrivaltime <= timer)
		{
			Pcb * tmp = deqPcb(&inputqueue);

			//	a. Real-time queue or
			if (tmp->priority == 0)
			{
				realtimebuffer = enqPcb(realtimebuffer, tmp);
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

		while (realtimebuffer)
		{
			/* Dump real time processes that use invalid amounts of memory or request any resources */
			if (!checkRTMem(realtimebuffer) || !checkRTRsrcs(realtimebuffer))
			{
				deqPcb(&realtimebuffer);
				printf("Invalid amount of memory or resources requested for realtime job; rejected\n");
				continue;
			}

			Mab * allocated_mem = memAlloc(rt_memory, realtimebuffer->mbytes);
			if (allocated_mem)
			{
				Pcb * tmp = deqPcb(&realtimebuffer);
				tmp->mab_block = allocated_mem;
				realtimequeue = enqPcb(realtimequeue, tmp);
			}
			else
			{
				memFree(allocated_mem);
				break;
			}
		}

		while (!realtimequeue && userjobqueue)
		{
			/* Dump user processes that request invalid amounts of memory or request invalid resources */

			if (!checkUJMem(userjobqueue) || !checkUJRsrcs(userjobqueue))
			{
				printf("Invalid amount of memory or resources requested for user job; rejected\n");
				deqPcb(&userjobqueue);
				continue;
			}
			//	Try to allocate memory for the user process
			Mab * allocated_mem = memAlloc(memory, userjobqueue->mbytes);


			if (allocated_mem && rsrcChk(resources,userjobqueue->req))
			{
				//	a. dequeue process from user job queue
				int priority = userjobqueue->priority - 1;
				Pcb * tmp = deqPcb(&userjobqueue);

				//	b. allocate memory to the process
				tmp->mab_block = allocated_mem;

				//	c. allocate resources to the process
				Rsrc * allocated_Rsrc = rsrcAlloc(resources, tmp->req);

				//	d. enqueue on appropriate priority feedback queue
				fbqueue[priority] = enqPcb(fbqueue[priority], tmp);
			}
			else
			{
				//	Free memory in case it was allocated but resources failed
				memFree(allocated_mem);
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

				// B. Free memory and resources allocated to process (resources user only)
				rsrcFree(resources, currentprocess->req);
				// Automatically frees appropriately for RT/U memory as it recursively frees parents
				memFree(currentprocess->mab_block);

				// C. free up process structure memory
				free(currentprocess);
				currentprocess = NULL;
			}

			//	c. else if it is a user process and other processes are waiting in any of the queues
			else if (currentprocess->priority != 0 && (fbQueuesNotNull(fbqueue) || realtimequeue))
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

		//	iv. If no process currently running and real time queue and feedback queue are not all empty
		if (!currentprocess && (fbQueuesNotNull(fbqueue) || realtimequeue))
		{
			// a. Dequeue a process from the highest priority feedback queue that is not empty
			//	A. Real time queue takes priority 
			if (realtimequeue)
			{
				currentprocess = deqPcb(&realtimequeue);
			}
			//	B. Once real time queue is cleared, then give feedback queues service
			else if (!realtimebuffer && fbQueuesNotNull(fbqueue))
			{
				//	Find highest priority non-empty feedback queue
				int fb_index = -1;
				for (int i = 0; i < PRIORITY_QUEUES; ++i)
				{
					if (fbqueue[i] != NULL)
					{
						fb_index = i;
						break;
					}
				}
				if (fb_index != -1)
				{
					currentprocess = deqPcb(&fbqueue[fb_index]);	
				}
			}

			// b. If already started but suspended, resume process else start process
			// c. Set it as currently running process
			// Requires a check for when realtimebuffer hasn't been fully unloaded yet,
			// but they need to be first queued onto a currently empty realtimequeue
			if (currentprocess)
			{
				startPcb(currentprocess);
				currentprocess->status = PCB_RUNNING;
			}
		}

		// v. sleep for one second
		sleep(QUANTUM);

		// vi. increment dispatch timer
		timer++;
		// vii. go back to 4
	}

	free (memory);
	free (rt_memory);
	free (resources);

	//	5. Exit
	exit (0);
}

/****

  Checks whether any feedback queues contain queued processes

 ****/
bool fbQueuesNotNull (Pcb * *fbqueue)
{
	for (int i = 0; i < PRIORITY_QUEUES; ++i)
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


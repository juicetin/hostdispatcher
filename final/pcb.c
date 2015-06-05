/*
Name: Justin Ting
SID: 430203826
Tutor: Jeshua
Tutorial Time: Monday 10am
*/

/*******************************************************************

  OS Eercises - Project 2 - HOST dispatcher

pcb - process control block functions for HOST dispatcher

Pcb * startPcb(Pcb * process) - start (or restart) a process
returns:
Pcb * of process
NULL if start (restart) failed

Pcb * suspendPcb(Pcb * process) - suspend a process
returns:
Pcb * of process
NULL if suspend failed

Pcb * terminatePcb(Pcb * process) - terminate a process
returns:
Pcb * of process
NULL if terminate failed

Pcb * printPcb(Pcb * process, FILE * iostream)
- print process attributes on iostream
returns:
Pcb * of process

void printPcbHdr(FILE *) - print header for printPcb
returns:
void

Pcb * createnullPcb(void) - create inactive Pcb.
returns:
Pcb * of newly initialised Pcb
NULL if malloc failed

Pcb * enqPcb (Pcb * headofQ, Pcb * process)
- queue process (or join queues) at end of queue
- enqueues at "tail" of queue. 
returns head of queue

Pcb * deqPcb (Pcb * * headofQ);
- dequeue process - take Pcb from "head" of queue.
returns:
Pcb * if dequeued,
NULL if queue was empty
& sets new head of Q pointer in adrs at 1st arg

 ********************************************************************

version: 1.0 (exercise 7)
date:    December 2003
author:  Dr Ian G Graham, ian.graham@griffith.edu.au

 *******************************************************************/

#include "pcb.h"

/*******************************************************
 * Pcb * startPcb(Pcb * process) - start (or restart)
 *    a process
 * returns:
 *    Pcb * of process
 *    NULL if start (restart) failed
 ******************************************************/
Pcb * startPcb (Pcb * p) { 
	if (p->pid == 0) {                 // not yet started
		switch (p->pid = fork ()) {    //  so start it
			case -1: 
				perror ("startPcb");
				exit(1); 
			case 0:                             // child 
				p->pid = getpid();
				p->status = PCB_RUNNING;
				printPcbHdr(stdout);            // printout in child to
				printPcb(p, stdout);            //  sync with o/p
				fflush(stdout);
				execvp (p->args[0], p->args); 
				perror (p->args[0]);
				exit (2);
		}                                       // parent         

	}
	else { // already started & suspended so continue
		kill (p->pid, SIGCONT);
	}    
	p->status = PCB_RUNNING;
	return p; 
} 

/*******************************************************
 * Pcb * suspendPcb(Pcb * process) - suspend
 *    a process
 * returns:
 *    Pcb * of process
 *    NULL if suspend failed
 ******************************************************/
Pcb * suspendPcb(Pcb * p) {
	/* FILL IN THIS BLOCK WITH YOUR CODE */
	if (kill(p->pid, SIGTSTP) == 0)
	{
		waitpid(p->pid, NULL, WUNTRACED);
		return p;
	}

	return NULL;
}

Pcb * resumePcb(Pcb * p) {
	kill(p->pid, SIGCONT);
	return p;
}

/*******************************************************
 * Pcb * terminatePcb(Pcb * process) - terminate
 *    a process
 * returns:
 *    Pcb * of process
 *    NULL if terminate failed
 ******************************************************/
Pcb * terminatePcb(Pcb * p) {
	/* FILL IN THIS BLOCK WITH YOUR CODE */
	if (kill(p->pid, SIGINT) == 0)
	{
		waitpid(p->pid, NULL, WUNTRACED);
		return p;
	}

	return NULL;
}  

/*******************************************************
 * Pcb * printPcb(Pcb * process, FILE * iostream)
 *  - print process attributes on iostream
 *  returns:
 *    Pcb * of process
 ******************************************************/
Pcb * printPcb(Pcb * p, FILE * iostream) {
	/* FILL IN THIS BLOCK WITH YOUR CODE */
	//fprintf(iostream, "%-12d\t%-8d\t%-18d\t%-6d\t%-6d\t%-8d\t%-8d\t%-6d\t%-3d\n", 
	//		p->arrivaltime, p->priority, p->remainingcputime,
	//		p->mbytes, p->status, p->req.printers, p->req.scanners, p->req.modems, p->req.cds);
	fprintf(iostream, "%-12d\t", p->arrivaltime);
	fprintf(iostream, "%-8d\t", p->priority);
	fprintf(iostream, "%-18d\t", p->remainingcputime);
	fprintf(iostream, "%-6d\t", p->mbytes);
	fprintf(iostream, "%-6d\t", p->mab_block->offset);
	fprintf(iostream, "%-6d\t", p->status);
	fprintf(iostream, "%-8d\t", p->req.printers);
	fprintf(iostream, "%-8d\t", p->req.scanners);
	fprintf(iostream, "%-6d\t", p->req.modems);
	fprintf(iostream, "%-3d\t", p->req.cds);
	fprintf(iostream, "\n");
	return p;
}

/*******************************************************
 * void printPcbHdr(FILE *) - print header for printPcb
 *  returns:
 *    void
 ******************************************************/  
void printPcbHdr(FILE * iostream) {  
	//fprintf(iostream, "arrivaltime	priority	remainingcputime	mbytes	status	printers	scanners	modems	cds\n");
	fprintf(iostream, "arrivaltime\t");
	fprintf(iostream, "priority\t");
	fprintf(iostream, "remainingcputime\t");
	fprintf(iostream, "mbytes\t");
	fprintf(iostream, "offset\t");
	fprintf(iostream, "status\t");
	fprintf(iostream, "printers\t");
	fprintf(iostream, "scanners\t");
	fprintf(iostream, "modems\t");
	fprintf(iostream, "cds\t");
	fprintf(iostream, "\n");
	/* FILL IN THIS LINE WITH YOUR CODE */
}

/*******************************************************
 * Pcb * createnullPcb() - create inactive Pcb.
 *
 * returns:
 *    Pcb * of newly initialised Pcb
 *    NULL if malloc failed
 ******************************************************/
Pcb * createnullPcb() {
	Pcb * newprocessPtr;

	if ((newprocessPtr = (Pcb *) malloc (sizeof(Pcb)))) {
		newprocessPtr->pid = 0;
		newprocessPtr->args[0] = DEFAULT_PROCESS;
		newprocessPtr->args[1] = NULL;
		newprocessPtr->arrivaltime = 0;
		newprocessPtr->priority = HIGH_PRIORITY;
		newprocessPtr->remainingcputime = 0;
		newprocessPtr->mbytes = 0;
		//        newprocessPtr->memoryblock = NULL;
		newprocessPtr->req.printers = 0;
		newprocessPtr->req.scanners = 0;
		newprocessPtr->req.modems = 0;
		newprocessPtr->req.cds = 0;
		newprocessPtr->status = PCB_UNINITIALIZED;
		newprocessPtr->next = NULL;
		return newprocessPtr;
	}
	perror("allocating memory for new process");
	return NULL;
}   

/*******************************************************
 * Pcb * enqPcb (Pcb * headofQ, Pcb * process)
 *    - queue process (or join queues) at end of queue
 * 
 * returns head of queue
 ******************************************************/
Pcb * enqPcb(Pcb * q, Pcb * p) {
	Pcb * h = q;

	p->next = NULL; 
	if (q) {
		while (q->next) q = q->next;
		q->next = p;
		return h;
	}
	return p;
}
/*******************************************************
 * Pcb * deqPcb (Pcb ** headofQ);
 *    - dequeue process - take Pcb from head of queue.
 *
 * returns:
 *    Pcb * if dequeued,
 *    NULL if queue was empty
 *    & sets new head of Q pointer in adrs at 1st arg
 *******************************************************/
Pcb * deqPcb(Pcb ** hPtr) {
	Pcb * p;

	if (hPtr && (p = * hPtr)) {
		* hPtr = p->next;
		return p;
	}
	return NULL;
}

/*******************************************************
 *  bool checkRsrcs(Pcb *block)
 *  	- checks whether there are valid resources being
 *  	requested
 *
 *  returns:
 *  	true if realtime processes request no resources
 *  	true if user processes don't request more than
 *  		the max possible resources
 *******************************************************/
bool checkRsrcs(Pcb *block)
{
	//If the block belongs to a real time process
	if (block->priority == 0)
	{
		return block->req.printers == 0 && block->req.scanners == 0 && 
			block->req.modems == 0 && block->req.cds == 0;
	}
	//If the block belongs to a user process
	return block->req.printers >= 0 && block->req.printers <= PRINTERS &&
		block->req.scanners >= 0 && block->req.scanners <= SCANNERS &&
		block->req.modems >= 0 && block->req.modems <= MODEMS &&
		block->req.cds >= 0 && block->req.cds <= CDS;
}

/*******************************************************
 *  bool checkMem(Pcb *block)
 *  	- checks whether the memory requested is valid
 *  returns:
 *  	true if realtime jobs don't request more than 64mb
 *  	true if user jobs don't request more than TOTAL/2
 *  		(due to the Buddy system being in use)
 *******************************************************/
bool checkMem(Pcb *block)
{
	if (block->priority == 0)
	{
		return block->mbytes >= 0 && block->mbytes <= RT_MEMORY_SIZE;
	}
	return block->mbytes >= 0 && block->mbytes <= MEMORY_SIZE / 2;
}


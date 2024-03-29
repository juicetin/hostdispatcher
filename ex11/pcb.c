/*
Name: Justin Ting
SID: 430203826
Tutor: Jeshua
Tutorial Time: Monday 10am
*/

/*******************************************************************

  OS Eercises - Project 2 - HOST dispatcher

  pcb - process control block functions for HOST dispatcher

  PcbPtr startPcb(PcbPtr process) - start (or restart) a process
returns:
PcbPtr of process
NULL if start (restart) failed

PcbPtr suspendPcb(PcbPtr process) - suspend a process
returns:
PcbPtr of process
NULL if suspend failed

PcbPtr terminatePcb(PcbPtr process) - terminate a process
returns:
PcbPtr of process
NULL if terminate failed

PcbPtr printPcb(PcbPtr process, FILE * iostream)
- print process attributes on iostream
returns:
PcbPtr of process

void printPcbHdr(FILE *) - print header for printPcb
returns:
void

PcbPtr createnullPcb(void) - create inactive Pcb.
returns:
PcbPtr of newly initialised Pcb
NULL if malloc failed

PcbPtr enqPcb (PcbPtr headofQ, PcbPtr process)
- queue process (or join queues) at end of queue
- enqueues at "tail" of queue. 
returns head of queue

PcbPtr deqPcb (PcbPtr * headofQ);
- dequeue process - take Pcb from "head" of queue.
returns:
PcbPtr if dequeued,
NULL if queue was empty
& sets new head of Q pointer in adrs at 1st arg

 ********************************************************************

version: 1.0 (exercise 7)
date:    December 2003
author:  Dr Ian G Graham, ian.graham@griffith.edu.au

 *******************************************************************/

#include "pcb.h"

/*******************************************************
 * PcbPtr startPcb(PcbPtr process) - start (or restart)
 *    a process
 * returns:
 *    PcbPtr of process
 *    NULL if start (restart) failed
 ******************************************************/
PcbPtr startPcb (PcbPtr p) { 
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
 * PcbPtr suspendPcb(PcbPtr process) - suspend
 *    a process
 * returns:
 *    PcbPtr of process
 *    NULL if suspend failed
 ******************************************************/
PcbPtr suspendPcb(PcbPtr p) {
	/* FILL IN THIS BLOCK WITH YOUR CODE */
	if (kill(p->pid, SIGTSTP) == 0)
	{
		waitpid(p->pid, NULL, WUNTRACED);
		return p;
	}

	return NULL;
}

/*******************************************************
 * PcbPtr terminatePcb(PcbPtr process) - terminate
 *    a process
 * returns:
 *    PcbPtr of process
 *    NULL if terminate failed
 ******************************************************/
PcbPtr terminatePcb(PcbPtr p) {
	/* FILL IN THIS BLOCK WITH YOUR CODE */
	if (kill(p->pid, SIGINT) == 0)
	{
		waitpid(p->pid, NULL, WUNTRACED);
		return p;
	}

	return NULL;
}  

/*******************************************************
 * PcbPtr printPcb(PcbPtr process, FILE * iostream)
 *  - print process attributes on iostream
 *  returns:
 *    PcbPtr of process
 ******************************************************/
PcbPtr printPcb(PcbPtr p, FILE * iostream) {
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
 * PcbPtr createnullPcb() - create inactive Pcb.
 *
 * returns:
 *    PcbPtr of newly initialised Pcb
 *    NULL if malloc failed
 ******************************************************/
PcbPtr createnullPcb() {
	PcbPtr newprocessPtr;

	if ((newprocessPtr = (PcbPtr) malloc (sizeof(Pcb)))) {
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
 * PcbPtr enqPcb (PcbPtr headofQ, PcbPtr process)
 *    - queue process (or join queues) at end of queue
 * 
 * returns head of queue
 ******************************************************/
PcbPtr enqPcb(PcbPtr q, PcbPtr p) {
	PcbPtr h = q;

	p->next = NULL; 
	if (q) {
		while (q->next) q = q->next;
		q->next = p;
		return h;
	}
	return p;
}
/*******************************************************
 * PcbPtr deqPcb (PcbPtr * headofQ);
 *    - dequeue process - take Pcb from head of queue.
 *
 * returns:
 *    PcbPtr if dequeued,
 *    NULL if queue was empty
 *    & sets new head of Q pointer in adrs at 1st arg
 *******************************************************/
PcbPtr deqPcb(PcbPtr * hPtr) {
	PcbPtr p;

	if (hPtr && (p = * hPtr)) {
		* hPtr = p->next;
		return p;
	}
	return NULL;
}

bool checkRTRsrcs(PcbPtr block)
{
	return block->req.printers == 0 && block->req.scanners == 0 && 
		block->req.modems == 0 && block->req.cds == 0;
}

bool checkUJRsrcs(PcbPtr block)
{
	return block->req.printers >= 0 && block->req.printers <= PRINTERS &&
		   block->req.scanners >= 0 && block->req.scanners <= SCANNERS &&
		   block->req.modems >= 0 && block->req.modems <= MODEMS &&
		   block->req.cds >= 0 && block->req.cds <= CDS;
}

bool checkRTMem(PcbPtr block)
{
	return block->mbytes >= 0 && block->mbytes <= RT_MEMORY_SIZE;
}

bool checkUJMem(PcbPtr block)
{
	return block->mbytes >= 0 && block->mbytes <= MEMORY_SIZE / 2;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAXARGS 1024

struct pcb {
    pid_t pid;                // system process ID
    char *args[MAXARGS];      // program name and args
    int arrival_time;         
    int priority;
    int memory;
    int printers;
    int scanners;
    int modems;
    int cds;
    int remaining_cpu_time; 
    struct pcb *next;          // links for PCB handlers
};
typedef struct pcb Pcb;
typedef Pcb * PcbPtr;


PcbPtr startPcb(PcbPtr process);
PcbPtr terminatePcb(PcbPtr process);
PcbPtr createnullPcb(void);
PcbPtr enqPcb(PcbPtr headofQ, PcbPtr process);
PcbPtr deqPcb(PcbPtr *headofQ);

int main(int argc, char **argv)
{
    FILE * fp = fopen(argv[1], "r");
    char str[1024];

    Pcb processes[20];
    int proc_count = 0;

    while (fgets(str, 1024, fp) != NULL)
    {
        int items[8], count = 0;
        char *token = strtok(str, " ,\n");
        while (token != NULL)
        {
            items[count++] = atoi(token);
            token = strtok(NULL, " ,\n");
        }
        //<arrival time>, <priority>, <cputime>, <memory alloc>, <res1 alloc>, <res2>, <res3>, <res4>
        processes[proc_count].arrival_time = items[0];          
        processes[proc_count].priority = items[1];
        processes[proc_count].remaining_cpu_time = items[2];
        processes[proc_count].memory = items[3];
        processes[proc_count].printers = items[4];
        processes[proc_count].scanners = items[5];
        processes[proc_count].modems = items[6];
        processes[proc_count].cds = items[7];
        processes[proc_count].args[0] = "./process";
        processes[proc_count].args[1] = NULL;
        proc_count++;
    }

    for (int i = 0; i < proc_count; ++i)
    {
        int pid = fork();
        if (pid == 0)
        {
            execvp(processes[i].args[0], processes[i].args);
        }
        else
        {
            waitpid(pid, NULL, 0);
        }
    }

    return 0;
}

PcbPtr startPcb(PcbPtr process)
{

    int pid = fork();
    if (pid == 0)
    {
        PcbPtr newPcb = (PcbPtr) malloc(sizeof(Pcb));
        newPcb->args[0] = "process";
        newPcb->args[2] = "20";
        newPcb->args[3] = NULL;
        execvp(newPcb->args[0], newPcb->args);
    }
    return NULL;
}

PcbPtr terminatePcb(PcbPtr process)
{

    return NULL;
}

PcbPtr createnullPcb(void)
{

    return NULL;
}

PcbPtr enqPcb(PcbPtr headofQ, PcbPtr process)
{

    return NULL;
}

PcbPtr deqPcb(PcbPtr *headofQ)
{

    return NULL;
}

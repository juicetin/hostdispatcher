/*
Name: Justin Ting
SID: 430203826
Tutor: Jeshua
Tutorial Time: Monday 10am
*/

#ifndef rsrc_h
#define rsrc_h

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h> 

#define PRINTERS 2
#define SCANNERS 1
#define MODEMS 1
#define CDS 2

struct rsrc {
    int printers;
    int scanners;
    int modems;
    int cds;
};

typedef struct rsrc Rsrc;
typedef Rsrc * RsrcPtr;

/* resuorce management prototypes *****************************/
RsrcPtr rsrcChk(RsrcPtr, Rsrc);
RsrcPtr rsrcAlloc(RsrcPtr, Rsrc);
RsrcPtr rsrcFree(RsrcPtr, Rsrc);
RsrcPtr createRsrcs(void);

#endif
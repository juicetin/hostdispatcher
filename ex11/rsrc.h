//#ifdef RSRC_H
//#define RSRC_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h> 

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

//#endif

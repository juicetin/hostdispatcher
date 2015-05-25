/*
Name: Justin Ting
SID: 430203826
Tutor: Jeshua
Tutorial Time: Monday 10am
*/

#include "rsrc.h"

/*************************************************
 * RsrcPtr rsrcChk(RsrcPtr master, Rsrc r):
 * 		Given all resources and resources needed,
 * 		determine whether there is enough for a
 * 		process
 * returns:
 * 		The all-resources pointer if there are enough
 * 		NULL if there are insufficient resources
 * ***********************************************/
RsrcPtr rsrcChk(RsrcPtr master, Rsrc r)
{
	if (r.printers <= master->printers
			&& r.scanners <= master->scanners
			&& r.modems <= master->modems
			&& r.cds <= master->cds)
	{
		return master;
	}
	return NULL;
}

/*************************************************
 * RsrcPtr rsrcAlloc (rsrcPtr master, Rsrc r):
 * 		Allocates resources for a given process
 * returns:
 * 		Pointer to the modified resource block
 * ***********************************************/
RsrcPtr rsrcAlloc(RsrcPtr master, Rsrc r)
{
	master->printers -= r.printers;
	master->scanners -= r.scanners;
	master->modems -= r.modems;
	master->cds -= r.cds;
	return master;
}

/*************************************************
 * RsrcPtr rsrcFree(RsrcPtr master, rsrc r):
 * 		Frees resources for a given process
 * returns:
 * 		Pointer to the modified resource block
 * ***********************************************/
RsrcPtr rsrcFree(RsrcPtr master, Rsrc r)
{
	master->printers += r.printers;
	master->scanners += r.scanners;
	master->modems += r.modems;
	master->cds += r.cds;
	return master;
}

/*************************************************
 * RsrcPtr createRsrcs:
 * 		Creates all available system resources
 * returns:
 * 		Pointer to the full resource block
 * ***********************************************/
RsrcPtr createRsrcs(void)
{
	RsrcPtr rsrc = malloc(sizeof(Rsrc));
	rsrc->printers = PRINTERS;
	rsrc->scanners = SCANNERS;
	rsrc->modems = MODEMS;
	rsrc->cds = CDS;
	return rsrc;
}

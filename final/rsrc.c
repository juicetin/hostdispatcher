/*
Name: Justin Ting
SID: 430203826
Tutor: Jeshua
Tutorial Time: Monday 10am
*/

#include "rsrc.h"

/*************************************************
 * Rsrc * rsrcChk(Rsrc * master, Rsrc r):
 * 		Given all resources and resources needed,
 * 		determine whether there is enough for a
 * 		process
 * returns:
 * 		The all-resources pointer if there are enough
 * 		NULL if there are insufficient resources
 * ***********************************************/
Rsrc * rsrcChk(Rsrc * master, Rsrc r)
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
 * Rsrc * rsrcAlloc (Rsrc * master, Rsrc r):
 * 		Allocates resources for a given process
 * returns:
 * 		Pointer to the modified resource block
 * ***********************************************/
Rsrc * rsrcAlloc(Rsrc * master, Rsrc r)
{
	master->printers -= r.printers;
	master->scanners -= r.scanners;
	master->modems -= r.modems;
	master->cds -= r.cds;
	return master;
}

/*************************************************
 * Rsrc * rsrcFree(Rsrc * master, rsrc r):
 * 		Frees resources for a given process
 * returns:
 * 		Pointer to the modified resource block
 * ***********************************************/
Rsrc * rsrcFree(Rsrc * master, Rsrc r)
{
	master->printers += r.printers;
	master->scanners += r.scanners;
	master->modems += r.modems;
	master->cds += r.cds;
	return master;
}

/*************************************************
 * Rsrc * createRsrcs:
 * 		Creates all available system resources
 * returns:
 * 		Pointer to the full resource block
 * ***********************************************/
Rsrc * createRsrcs(void)
{
	Rsrc * rsrc = malloc(sizeof(Rsrc));
	rsrc->printers = PRINTERS;
	rsrc->scanners = SCANNERS;
	rsrc->modems = MODEMS;
	rsrc->cds = CDS;
	return rsrc;
}

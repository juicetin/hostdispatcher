#include "rsrc.h"

RsrcPtr rsrcChk(RsrcPtr master, Rsrc r)
{
	if (r.printers <= master->printers
			&& r.scanners <= master->scanners
			&& r.modems <= master->modems
			&& r.cds <= master->cds)
	{
		//master->printers -= r.printers;
		//master->scanners -= r.scanners;
		//master->modems -= r.modems;
		//master->cds -= r.cds;
		return master;
	}
	return NULL;
}

RsrcPtr rsrcAlloc(RsrcPtr master, Rsrc r)
{
	master->printers -= r.printers;
	master->scanners -= r.scanners;
	master->modems -= r.modems;
	master->cds -= r.cds;
	return master;
}

RsrcPtr rsrcFree(RsrcPtr master, Rsrc r)
{
	//printf("%d %d %d %d\n", r.printers, r.scanners, r.modems, r.cds);

	master->printers += r.printers;
	master->scanners += r.scanners;
	master->modems += r.modems;
	master->cds += r.cds;

	//printf("%d %d %d %d\n", master->printers, master->scanners, master->modems, master->cds);

	return master;
}


all: hostd.c pcb.c hostd.h pcb.h
	@gcc -Wall -std=gnu99 -pedantic hostd.c pcb.c -o hostd
	@./hostd rr.txt

all: hostd.c pcb.c hostd.h pcb.h
	@clang -Wall -pedantic hostd.c pcb.c -o hostd
	@./hostd feedback.txt

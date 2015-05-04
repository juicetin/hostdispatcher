# Makefile
# Justin Ting, 430203826 - Operating Systems Internals Assignment 2
# Monday 10am-12pm lab - Tutor: Jeshua

hostdis: main.c 
	@clang -Wall main.c -o hostd
	@./hostd fcfs.txt

process: sigtrap.c
	@clang -Wall sigtrap.c -o process
	@echo

clean:
	@rm -f *.o
	@rm hostd process

run:
	@./hostd

#Makefile
#Author Satya Mehta

CC= gcc
#CC = arm-linux-gcc

edf: edf_llf.o
	$(CC) -o edf edf_llf.o

edf_llf.o: edf_llf.c
	$(CC) -c edf_llf.c

clean: 
	rm -f edf_llf.o edf_llf

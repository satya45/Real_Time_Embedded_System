#Makefile
#Author Satya Mehta

#CC=arm-linux-gcc -g
CC=gcc -g
deadlock: deadlock.o
	$(CC) -o deadlock deadlock.o -lpthread -lrt

deadlock.o: deadlock.c
	$(CC) -c deadlock.c

clean: 
	rm -f deadlock deadlock.o

PROG = main seq
CFLAGS = -g -fPIC -m64 -Wall
LFLAGS= -fPIC -m64 -Wall
CC = mpicc

all: $(PROG)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

main : main.o
	$(CC) $(LFLAGS) main.o -o main

seq.o : seq.c
	gcc -c seq.c

seq : seq.o
	gcc seq.o -o seq


run:
	mpirun --hostfile hostfile -np 24 main

ps:
	ps -fu $$USER

clean:
	/bin/rm -f *~
	/bin/rm -f *.o

test-mpi:
	mpirun --hostfile hostfile -np 4 main

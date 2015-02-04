PROG = par
CFLAGS = -g -fPIC -m64 -Wall
LFLAGS= -fPIC -m64 -Wall
CC = mpicc

all: $(PROG)

par.o: par.c
	$(CC) $(CFLAGS) -c par.c

par : par.o
	$(CC) $(LFLAGS) par.o -o par

run:
	mpirun --hostfile hostfile.txt -np 8 ./par

ps:
	ps -fu $$USER

clean:
	/bin/rm -f par
	/bin/rm -f par.o
PROG = seq
CFLAGS = -g -fPIC -m64 -Wall -std=c99 -D_BSD_SOURCE -D_POSIX_C_SOURCE=200112
LFLAGS= -fPIC -m64 -Wall -Wl,--no-as-needed -lrt
CC = mpicc

all: $(PROG)

seq.o: seq.c
	$(CC) -c seq.c

seq : seq.o
	$(CC) seq.o -o ./seq

run:
	./seq

ps:
	ps -fu $$USER

clean:
	/bin/rm -f seq
	/bin/rm -f seq.o
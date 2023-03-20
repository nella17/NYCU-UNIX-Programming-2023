CC		= gcc
CFLAGS	= -Wall -g -D_GNU_SOURCE -fPIC
LDFLAGS = 
PROGS	= libsolver.so

all: $(PROGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

libsolver.so: libsolver.o
	$(CC) -o $@ -shared $^ $(LDFLAGS)

test:
	LD_LIBRARY_PATH=`pwd`/lab03_pub LD_PRELOAD=./libsolver.so ./lab03_pub/chals

cleanup:
	rm -f *.o

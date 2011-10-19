CC=/home/martin/Projects/Linuxembedded/arm-arm1176jzfs-linux-gnueabi/bin/arm-arm1176jzfs-linux-gnueabi-gcc
CFLAGS=-Wall -I include
LDFLAGS=-L lib -l freetype -l ts

all: interface

interface: interface.c
	$(CC) $(CFLAGS) interface.c -o interface.o -c
	$(CC) $(LDFLAGS) interface.o -o interface

clean:
	rm *.o interface

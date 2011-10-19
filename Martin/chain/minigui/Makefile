#CC=/home/martin/Projects/Linuxembedded/arm-arm1176jzfs-linux-gnueabi/bin/arm-arm1176jzfs-linux-gnueabi-gcc
#CFLAGS=-Wall -I include
#LDFLAGS=-L lib -l freetype -l ts

all: minigui

interface: minigui.c
	$(CC) $(CFLAGS) $(LDFLAGS) minigui.c -o minigui

clean:
	rm minigui

CC=6410gcc
CFLAGS=-W -Wall -Iinclude -lpthread
LDFLAGS=-L lib -lpthread -lfreetype -lts

all: bin/main

bin/main: obj/ts.o obj/font.o obj/ft.o obj/fb.o obj/gui.o obj/ip.o obj/main.o
#obj/ft.o
	$(CC) $(LDFLAGS) -o $@ $^

obj/%.o: src/%.c include/%.h
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm obj/*.o

mrproper: clean
	rm bin/*

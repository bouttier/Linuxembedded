#ifndef _FB_H
#define _FB_H

#include "gui.h"

#include <linux/fb.h> // Pour le framebuffer
#include <sys/mman.h> // Pour mmap (mapping du buffer sur le device)
#include <sys/ioctl.h> // Pour open()
#include <unistd.h> // Pour open/close le device
#include <stdio.h>
#include <sys/fcntl.h>


int FB_open(GUI* gui, char* device);
void FB_close(GUI* gui);
void FB_set(GUI* gui, int x, int y, int R, int G, int B);
int FB_loc(GUI* gui, int x, int y);
void FB_rect(GUI* gui, int x, int y, int width, int height, int R, int G, int B);
void FB_background(GUI* gui, int R, int G, int B);

#endif /* _FB_H */

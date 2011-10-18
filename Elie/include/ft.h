#ifndef _FT_H
#define _FT_H

#include "gui.h"

#include <stdio.h>

int FT_init(GUI* gui);
void FT_setColor(GUI* gui, int R, int G, int B);
int FT_setFont(GUI* gui, char* font);
int FT_setSize(GUI* gui, int size);
int FT_getSizeString(GUI* gui, char* str, int* size);
int FT_printString(GUI* gui, char* str, int X, int Y);

#endif /* _FT_H */

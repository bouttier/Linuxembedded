#ifndef _TS_H
#define _TS_H

#include "gui.h" // pour les structures (GUI, TS_button)

#include "tslib.h" // lib TSLIB externe

void TS_init(GUI* gui);
void* TS_thread(void* args);
TS_button* TS_addButton(GUI* gui, int x, int y, int width, int height, char* label, int size, int xoffset, int yoffset, void (*onEvent)(TS_button*, int, void*), void* args);
void TS_printButton(GUI* gui, TS_button* button);

#endif /* _TS_H */

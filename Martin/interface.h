/*
 * interface.h - Display the IP address on the framebuffer.
 *
 * Copyright (C) 2011 Martin d'Allens and Jérémy Rivière.
 *
 * This file is placed under the GPL. Please see the file
 * COPYING for more details.
 */


#ifndef INTERFACE_H
#define INTERFACE_H


// Dirty checks of return value.
#ifdef NDEBUG
  #define check_return(expr) expr
#else
  #define check_return(expr) assert(!(expr))
#endif

// Folder to search for a ttf file.
#define FACE_FOLDER "/sdcard/"

#define BUTTON_SIZE   40
#define BUTTON_MARGIN 5

#define BACKGROUND_RED  0xF800
#define BACKGROUND_GRAY 0xC618


int main(int argc, char **argv);

char* get_ip();

void open_fb();
void open_face();
void open_tslib();

int  get_text_width(char* text);

void draw_pixel(int x, int y, int r, int g, int b); // Unused.
void draw_red_background();
void draw_button(int state);

void draw_text(int offset_x, int offset_y, char* text, int background);
void draw_text_centered(char* text, int background);

void slide_in();
void wait_for_button();


#endif /* INTERFACE_H */

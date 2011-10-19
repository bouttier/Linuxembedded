/*
 * interface.c - Display the IP address on the framebuffer.
 * 
 * Copyright (C) 2011 Martin d'Allens and Jérémy Rivière.
 *
 * This file is placed under the GPL. Please see the file
 * COPYING for more details.
 */


#include <assert.h> 
#include <unistd.h>
#include <fcntl.h>
#include <ft2build.h>
#include <linux/fb.h>
#include <string.h>
#include <sys/dir.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <tslib.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>

#include FT_FREETYPE_H

#include "interface.h"

// Framebuffer
int    fb_handle;
int    fb_width;
int    fb_height;
unsigned short* fb_map;
// TODO: check the rotation for the real mini2440 screen.

// Freetype
FT_Library library;
FT_Face    face;
int        font_size = 0;

// Touchscreen
struct tsdev* tsdev;



/*
 * Display IP address for eth0, and switch to the credits with a button.
 */
int main(int argc, char **argv)
{
	open_fb();
	open_face();
	
	tsdev = ts_open("/dev/touchscreen", 0);
	assert(tsdev);
	check_return(ts_config(tsdev));
	
	while(1) {
		draw_red_background();
		draw_text_centered(get_ip(), BACKGROUND_RED);
		draw_button(1);
		
		wait_for_button(); // Wait before sliding in the credits.
		slide_in();
		
		draw_text_centered("Caer & Poupine", BACKGROUND_GRAY);
		draw_button(-1);
		
		wait_for_button(); // Wait before hiding the credits.
	}
	
	return 0;
}


/*
 * Finds the IP address for eth0 and convert it to dot-decimal notation.
 */
char* get_ip()
{
	// http://stackoverflow.com/questions/2283494/get-ip-address-of-an-interface-on-linux
	struct ifreq ifr;
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	assert(fd != -1);
	
	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);
	
	check_return(ioctl(fd, SIOCGIFADDR, &ifr));
	close(fd);
	
	return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}


/*
 * Initialize the framebuffer device.
 */
void open_fb()
{
	fb_handle = open("/dev/fb0", O_RDWR);
	assert(fb_handle);
	
	struct fb_var_screeninfo info;
	check_return(ioctl(fb_handle, FBIOGET_VSCREENINFO, &info));
	fb_width  = info.xres;
	fb_height = info.yres;
	
	fb_map = (unsigned short*) mmap(0,
	                       fb_width * fb_height * 2 /* 16bpp */,
	                       PROT_READ | PROT_WRITE,
	                       MAP_SHARED,
	                       fb_handle,
	                       0);
	assert(fb_map);
}


/*
 * Find the font in FACE_FOLDER and initialize the Freetype2 library.
 */
void open_face()
{	
	struct dirent *file;
	char path[sizeof(FACE_FOLDER) + sizeof("concours-999.tff") - 1] = FACE_FOLDER;
	DIR *dir = opendir(path);
	assert(dir != NULL);
	
	while ((file = readdir(dir)) != NULL) {
		if(sscanf(file->d_name, "concours-%i.ttf", &font_size)) {
			assert(0 < font_size && font_size <= 999);
			break;
		}
	}
	strcat(path, (const char*)file->d_name);
	closedir(dir);
	
	check_return(FT_Init_FreeType(&library));
	check_return(FT_New_Face(library,
	                    path,
	                    0,
	                    &face));
	
	FT_Set_Pixel_Sizes(face, 0, font_size);
}


/*
 * Computes the width with the current font, in pixels.
 */
int get_text_width(char* text)
{
	int w = 0;
	while (text[0]) {
		check_return(FT_Load_Char(face,
		                     text[0],
		                     FT_LOAD_DEFAULT));
		w += face->glyph->advance.x;
		text++;
	}
	return w >> 6; // Conversion to pixels.
}


/*
 * Set the color for the pixel at the given coordinates.
 */
void draw_pixel(int x, int y, int r, int g, int b)
{
	assert(0 <= x && x < fb_width);
	assert(0 <= y && y < fb_height);
	assert(0 <= r && r < 32);
	assert(0 <= g && g < 64);
	assert(0 <= b && b < 32);
	
	fb_map[x + y * fb_width] = r << 11 | g << 5 | b;
}


/*
 * Quickly fill the framebuffer with red.
 */
void draw_red_background()
{
	int* pixel = (int*) fb_map;
	int i = 0;
	
	for( ; i < fb_width * fb_height / 2; i++) {
		*pixel = 0xF800F800;
		pixel++;
	}
}


/*
 * Draw a button in the bottom right corner.
 * The state determines the direction of the arrow.
 */
void draw_button(int state)
{
	int x, y;
	
	unsigned short* pixel = fb_map - BUTTON_MARGIN - 1 + (fb_height - BUTTON_MARGIN) * fb_width;
	for(y = 0; y < BUTTON_SIZE; y++) {
		for(x = 0; x < BUTTON_SIZE; x++) {
			pixel[-x] = BACKGROUND_GRAY; // 3 << 14 | 3 << 9 | 3 << 3
		}
		pixel -= fb_width;
	}
	pixel += BUTTON_SIZE / 4 * (fb_width * (2 - state) - 2 - state);
	for(x = 0; x < BUTTON_SIZE / 2; x++) {
		pixel[state * x] = 0;
		pixel[state * x * (fb_width + 1)] = 0;
		pixel[state * x * fb_width] = 0;
	}
	return;
}


/*
 * Draw the given text at the given position, assuming a given 
 * background color.
 */
void draw_text(int offset_x, int offset_y, char* text, int background)
{
	assert(0 <= offset_x && offset_x < fb_width);
	assert(0 <= offset_y && offset_y < fb_height);
	
	while (text[0]) {
		check_return(FT_Load_Char(face,
		                     text[0],
		                     FT_LOAD_RENDER));
		FT_Bitmap bitmap = face->glyph->bitmap;
		
		unsigned short* offset = fb_map
			          + (offset_y - face->glyph->bitmap_top) * fb_width
			          + offset_x + face->glyph->bitmap_left;
		int x, y;
		for (y = 0; y < bitmap.rows; y++) {
			unsigned short* pixel = offset + y * fb_width;
			
			for (x = 0; x < bitmap.width; x++) {
				if(background == BACKGROUND_GRAY)
				{
					unsigned char c = ((unsigned short)~bitmap.buffer[y * bitmap.pitch + x]) * 0xc0 / 0xff;
					*pixel = ((c << 8) & 0xF800)
						   | ((c << 3) & 0x07E0)
						   | ((c >> 3) & 0x001F);
				}
				else if(background == BACKGROUND_RED)
					*pixel = (~bitmap.buffer[y * bitmap.pitch + x] & 0xF8) << 8;
				pixel++;
			}
		}
		offset_x += face->glyph->advance.x >> 6;
		text++;
	}
}


/*
 * Draw a text centered.
 */
void draw_text_centered(char* text, int background)
{
	draw_text((fb_width - get_text_width(text)) / 2,
	          (fb_height + font_size) / 2,
	          text, background);
}


/*
 * Animate a sliding panel for the button corner.
 */
void slide_in()
{
	int x, y, i;
	for(i = 0; i <= fb_width - 2 * BUTTON_MARGIN; i++) {
		int xx = fb_width  - BUTTON_MARGIN - i;
		int yy = fb_height - BUTTON_MARGIN - (fb_height - 2 * BUTTON_MARGIN) * i / (fb_width - 2 * BUTTON_MARGIN);
		for(x = xx; x < fb_width - BUTTON_MARGIN; x++) {
			fb_map[x + yy * fb_width] = BACKGROUND_GRAY;
		}
		for(y = yy; y < fb_height - BUTTON_MARGIN; y++) {
			fb_map[xx + y * fb_width] = BACKGROUND_GRAY;
		}
		if(xx % 20 == 0)
			usleep(5);
	}
}


/*
 * Wait for a click on the button surface (tolerance of BUTTON_MARGIN).
 */
void wait_for_button()
{
	struct ts_sample sample;
	int up = 1;
	
	while(1) {
		int ret = ts_read(tsdev, &sample, 1);
		assert(ret >= 0);
		
		if(ret == 0) { // No new event.
			usleep(5000);
		}
		else if(sample.x > fb_width - BUTTON_SIZE - 2 * BUTTON_MARGIN
				&& sample.y > fb_height - BUTTON_SIZE - 2 * BUTTON_MARGIN) {
			if(sample.pressure)
				up = 0;
			else if(!up)
				return;
		}
		else
			up = 0; 
	}
}

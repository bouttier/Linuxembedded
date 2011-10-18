#ifndef _GUI_H
#define _GUI_H

#include <linux/fb.h>

// Lib FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
// Lib TouchScreen
#include "tslib.h"
// pthread pour le touchscreen
#include <pthread.h>

typedef struct TS_button TS_button;
struct TS_button {
	
	int id;
	int x;
	int y;
	int height;
	int width;
	char* label;
	int size;
	int xoffset;
	int yoffset;
	unsigned int state;
	void (*onEvent)(TS_button* button, int event, void* args);
	void* args;
	TS_button* previous;
	//~ TS_button* next;
	
};

typedef struct GUI GUI;
struct GUI {
	
	/* Frame buffer */
	int fb_device; // Frame buffer device
	struct fb_var_screeninfo fb_var_info; // Paramètres variables de l'écran
	struct fb_fix_screeninfo fb_fix_info; // Paramètres statiques de l'écran
	long int fb_screensize; // Taille de l'écran en octet
	char* fb_buffer; // Buffer qui sera mappé sur le device
	
	/* Freetype */
	FT_Library ft_library;
	FT_Face ft_face;
	int ft_color_r;
	int ft_color_g;
	int ft_color_b;
	char* ft_font;
	int size;
	
	/* Touchscreen */
	pthread_t pth;
	char* ts_device;
	struct tsdev* ts_dev;
	struct ts_sample ts_samp;
	int ts_button_id;
	TS_button* ts_button;
	int run;
	
	/* IP */
	int ip_size;
	char* ip_font;
	
};


#include "fb.h"
#include "ft.h"
#include "ts.h"

int GUI_init(GUI* gui);

#endif /* _GUI_H */

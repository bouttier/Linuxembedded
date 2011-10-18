#include <linux/fb.h> // Pour le framebuffer

#ifndef _STRUCT_H
#define _STRUCT_H

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct FB FB;
struct FB {
	int fbd; // Frame buffer device
	struct fb_var_screeninfo var_info; // Paramètres variables de l'écran
	struct fb_fix_screeninfo fix_info; // Paramètres statiques de l'écran
	long int screensize; // Taille de l'écran en octet
	char *fbp; // Buffer qui sera mappé sur le device
	
};

typedef struct FT FT;
struct FT {
	
	FT_Library library;
	FT_Face face;
	int R;
	int G;
	int B;
	
};

typedef struct GUI GUI;
struct GUI {
	
	FB fb;
	FT ft;
	/*TS ts;*/
	
	
};

#endif /* _STRUCT_H */

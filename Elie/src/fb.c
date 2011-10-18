#include "fb.h"

//#define DEBUG // Contrôle la position du pixel avant de l'écrire dans le buffer

// Obtenir un handle sur le framebuffer
int FB_open(GUI* gui, char* device) {

	gui->fb_screensize = 0;
	gui->fb_device = 0;
	gui->fb_buffer = 0;
	
	// Ouverture du device
	gui->fb_device = open(device, O_RDWR);
	if (!gui->fb_device) {
		printf("Error : impossible d'ouvrir le framebuffer (%s)\n", device);
		return -1;
	}
	
	// Lecture des caratéristiques statics de l'écran
	if (ioctl(gui->fb_device, FBIOGET_FSCREENINFO, &gui->fb_fix_info)) {
		printf("Error : impossible de récupérer les informations statiques du framebuffer\n");
		return -1;
	}
	
	// Lecture des caratéristiques variables de l'écran
	if (ioctl(gui->fb_device, FBIOGET_VSCREENINFO, &gui->fb_var_info)) {
		printf("Error : impossible de récupérer les informations variables du framebuffer\n");
		return -1;
	}
	
	// Taille de l'écran en octet
	gui->fb_screensize = gui->fb_var_info.xres * gui->fb_var_info.yres * gui->fb_var_info.bits_per_pixel / 8;
	
	// Mapping du framebuffer sur le buffer fbp
	gui->fb_buffer = (char *)mmap(0, gui->fb_screensize, PROT_READ | PROT_WRITE, MAP_SHARED, gui->fb_device, 0);
	if (gui->fb_buffer == MAP_FAILED) {
		printf("Error : impossible de mapper le framebuffer sur le buffer\n");
		return -1;
	}
	
	return 0;
	
}


// Rendre le handle sur le framebuffer
void FB_close(GUI* gui) {
	
	munmap(gui->fb_buffer, gui->fb_screensize);
	close(gui->fb_device);
	
}

// Colorer le pixel de coordonné (x,y) en RGB
// R [0-31] G [0-63] B[0-31]
void FB_set(GUI* gui, int x, int y, int R, int G, int B) {
	unsigned short int colour = R<<11 | G << 5 | B;
	*((unsigned short int*)(gui->fb_buffer + FB_loc(gui, x, y))) = colour;
}

// Obtenir la position dans le buffer (en octet) à partir des coordonnés (x,y)
int FB_loc(GUI* gui, int x, int y) {
	return (x+gui->fb_var_info.xoffset) * (gui->fb_var_info.bits_per_pixel/8)
		+ (y+gui->fb_var_info.yoffset) * gui->fb_fix_info.line_length;
}

// Dessiner un rectangle dont l'angle supérieur gauche est positionné en (x,y)
void FB_rect(GUI* gui, int x, int y, int width, int height, int R, int G, int B) {
	unsigned short int colour = R<<11 | G << 5 | B;
	int a, b;
	for (b = y ; b < y+height ; b++) {
		for (a = x ; a < x+width ; a++) {
#ifdef DEBUG
			if ((__u32)a < gui->fb_var_info.xoffset
					|| (__u32)a >= gui->fb_var_info.xoffset+gui->fb_var_info.xres
					|| (__u32)b < gui->fb_var_info.yoffset
					|| (__u32)b >= gui->fb_var_info.yoffset+gui->fb_var_info.yres) {
				printf("Warning : point out of screen (x = %i, y = %i)\n", a, b);
			} else {
				*((unsigned short int*)(gui->fb_buffer + FB_loc(gui, x, y))) = colour;
			}
#else
			*((unsigned short int*)(gui->fb_buffer + FB_loc(gui, a, b))) = colour;
#endif
		}
	}
}

// Remplir l'écran d'une couleur uniforme
// Plus rapide qu'un rectangle remplissant tous l'écran
void FB_background(GUI* gui, int R, int G, int B) {
	// Si il y a des offsets, on utilise la méthode lente mais qui recalcule les coordonnées des pixels !
	if (gui->fb_var_info.xoffset != 0 || gui->fb_var_info.yoffset != 0) {
		FB_rect(gui, 0, 0, gui->fb_var_info.xres-1, gui->fb_var_info.yres-1, R, G, B);
	} else {
		unsigned short int colour = R<<11 | G << 5 | B;
		int location;
		for (location = 0 ; location < gui->fb_screensize ; location += 2) {
			*((unsigned short int*)(gui->fb_buffer + location)) = colour;
		}
	}
}




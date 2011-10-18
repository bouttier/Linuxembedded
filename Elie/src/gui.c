#include "gui.h"

int GUI_init(GUI* gui) {
	
	// Initialisation du framebuffer
	if (FB_open(gui, "/dev/fb0") == -1) return -1;
	printf("Résolution de l'écran : %dx%d, %dbpp\n", gui->fb_var_info.xres, gui->fb_var_info.yres, gui->fb_var_info.bits_per_pixel); 
	
	// Initialisation de la lib freetype
	FT_init(gui);
	
	// Initialisation du touchscreen
	TS_init(gui);
	
	
	return 0;
}

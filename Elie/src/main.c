#include "main.h"

void show(TS_button* button, int event, void* args);

int main(int args, char* argv[]) {
	
	GUI gui;
	if (GUI_init(&gui) == -1) exit(-1);
	
	FB_background(&gui, 0, 0, 0);
	
	char path[256];
	if (args > 1) {
		memcpy(path, argv[1], 256);
	} else {
		memcpy(path, "/sdcard/", 256);
	}
	int size = 0;
	if (getFont(path, &size) == -1) {
		printf("Impossible de trouver une police (dossier %s)\n", path);
		return -1;
	}
	printf("Font : %s, size : %i\n", path, size);
	gui.ip_size = size;
	gui.ip_font = path;
	
	FT_setFont(&gui, "/home/elie/FreeSans.ttf");
	TS_button* button = TS_addButton(&gui, 400, 222, 80, 50, "Retour", 16, -1, -1, show, (void*)&gui);
	show(button, 0, &gui);
	
	
	
	pthread_join(gui.pth, NULL);
	FB_close(&gui);
	return 0;
}

void show(TS_button* button, int event, void* args) {
	if (event == 0) {
		GUI* gui = args;
		if (button->label[0] == 'T') { // Go affichage ip
		
			FB_background(gui, 0, 0, 0);
			
			FT_setFont(gui, "/home/elie/FreeSans.ttf");
			FT_setSize(gui, 40);
			FT_setColor(gui, 31, 0, 0);
			FT_printString(gui, "Djanos - Lockidor", -1, 75);
			FT_setSize(gui, 35);
			FT_setColor(gui, 0, 0, 31);
			FT_printString(gui, "ENSEEIHT", -1, 150);
			FT_setSize(gui, 35);
			FT_setColor(gui, 0, 50, 20);
			FT_printString(gui, "Octobre 2011", -1, 225);
			
			button->label = "IP";
			TS_printButton(gui, button);
			
		} else { // Go affichage team
		
			FB_background(gui, 31, 0, 0);
			
			FT_setFont(gui, gui->ip_font);
			FT_setSize(gui, gui->ip_size);
			FT_setColor(gui, 0, 0, 0);
			FT_printString(gui, getIP("eth0"), -1, -1);
			
			button->label = "Team";
			FT_setFont(gui, "/home/elie/FreeSans.ttf");
			TS_printButton(gui, button);
			
		}
	}
}

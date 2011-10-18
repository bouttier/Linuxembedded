#include "ts.h"

void TS_init(GUI* gui) {
	
	if(getenv("TSLIB_TSDEVICE") == NULL) {
		gui->ts_device = "/dev/touchscreen";
	} else {
		gui->ts_device = getenv("TSLIB_TSDEVICE");
	}
	gui->ts_dev = ts_open(gui->ts_device, 0);
	if (!gui->ts_dev) {
		printf("Error : TS_init::ts_open");
		exit(1);
	}
	if (ts_config(gui->ts_dev)) {
		printf("Error : TS_init::ts_config");
		exit(1);
	}
	
	gui->ts_button = NULL;
	gui->run = 1;
	pthread_create(&gui->pth, NULL, TS_thread, (void*)gui);
	
}

void* TS_thread(void* args) {
	
	GUI* gui = args;
	int x = -1; int y = -1; unsigned int p = 2;
	int retour;
	TS_button* buttonEnabled = NULL;
	TS_button* button;
	int onButton = 0;
	
	while (gui->run == 1) {
	retour = ts_read(gui->ts_dev, &gui->ts_samp, 1);
		if (retour < 0) { printf("Error : ts_read fail\n"); exit(1); }
		// Ce n'est pas la même info que la dernière fois
		if (retour == 1 && ((gui->ts_samp.x != x) || (gui->ts_samp.y != y) || (p != gui->ts_samp.pressure))) {
			x = gui->ts_samp.x; y = gui->ts_samp.y; p = gui->ts_samp.pressure;
			if (p) { // Appuyer
				onButton = 0;
				button = gui->ts_button;
				while (button != NULL) {
					if (x >= button->x && x < button->x+button->width && y >= button->y && y < button->y+button->height) {
						if (!button->state) {
							if (!button->state) {
								if (buttonEnabled != NULL) {
									buttonEnabled->state = 0;
									TS_printButton(gui, buttonEnabled);
									(*buttonEnabled->onEvent)(buttonEnabled, -1, buttonEnabled->args);
								}
								button->state = 1;
								TS_printButton(gui, button);
								(*button->onEvent)(button, 1, button->args);
								buttonEnabled = button;
							}
						}
						onButton = 1;
						break;
					}
					button = button->previous;
				}
				if (!onButton) {
					if (buttonEnabled != NULL) {
						buttonEnabled->state = 0;
						TS_printButton(gui, buttonEnabled);
						(*buttonEnabled->onEvent)(buttonEnabled, -1, buttonEnabled->args);
						buttonEnabled = NULL;
					}
				}
			} else {
				if (buttonEnabled != NULL) {
					buttonEnabled->state = 0;
					TS_printButton(gui, buttonEnabled);
					(*buttonEnabled->onEvent)(buttonEnabled, 0, buttonEnabled->args);
					buttonEnabled = NULL;
				}
			}
		} else {
			usleep(1000);
		}
	}
	
	sleep(100);
	
	pthread_exit(0);
}

TS_button* TS_addButton(GUI* gui, int x, int y, int width, int height, char* label, int size, int xoffset, int yoffset, void (*onEvent)(TS_button*, int, void*), void* args) {
	
	TS_button* button = malloc(sizeof(TS_button));
	if (!button) { printf("TS_addButton::malloc fail\n"); exit(1); }
	button->id = gui->ts_button_id; gui->ts_button_id++;
	button->x = x;
	button->y = y;
	button->height = height;
	button->width = width;
	button->label = label;
	button->size = size;
	button->xoffset = xoffset;
	button->yoffset = yoffset;
	button->state = 0;
	button->onEvent = onEvent;
	button->args = args;
	button->previous = gui->ts_button;
	//~ button->next = NULL;
	gui->ts_button = button;
	TS_printButton(gui, button);
	return button;
	
}

void TS_printButton(GUI* gui, TS_button* button) {
	
	//~ printf("\tPrint button : position (%i,%i), size %i×%i, state : %i\n", button->x, button->y, button->width, button->height, button->state);
	int x; int y;
	FT_setColor(gui, 0, 0, 0);
	FT_setSize(gui, button->size);
	int xoffset = button->xoffset;
	int yoffset = button->yoffset;
	if (xoffset == -1 || yoffset == -1) {
		int size[2];
		FT_getSizeString(gui, button->label, size);
		if (xoffset == -1) {
			xoffset = (button->width - size[0])/2;
		}
		if (yoffset == -1) {
			yoffset = (button->height - size[1])/2;
		}
	}
	FB_rect(gui, button->x+1, button->y+1, button->width-1, button->height-1, 28, 57, 28);
	if (button->state) {
		for (x = button->x ; x < button->x+button->width ; x++) {
			FB_set(gui, x, button->y, 0, 0, 0);
			FB_set(gui, x, button->y+button->height-1, 31, 63, 31);
		}
		for (y = button->y ; y < button->y+button->height ; y++) {
			FB_set(gui, button->x, y, 0, 0, 0);
			FB_set(gui, button->x+button->width-1, y, 31, 63, 31);
		}
		FB_set(gui, button->x+button->width-1, button->y+button->height-1, 31, 63, 31);
		FT_printString(gui, button->label, button->x+xoffset+1, button->y+button->height-yoffset+1);
	} else {
		for (x = button->x ; x < button->x+button->width ; x++) {
			FB_set(gui, x, button->y, 31, 63, 31);
			FB_set(gui, x, button->y+button->height-1, 0, 0, 0);
		}
		for (y = button->y ; y < button->y+button->height ; y++) {
			FB_set(gui, button->x, y, 31, 63, 31);
			FB_set(gui, button->x+button->width-1, y, 0, 0, 0);
		}
		FB_set(gui, button->x+button->width-1, button->y+button->height-1, 0, 0, 0);
		FT_printString(gui, button->label, button->x+xoffset, button->y+button->height-yoffset);
	}
	
}

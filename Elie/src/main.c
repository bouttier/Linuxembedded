#include "main.h"

int main(int args, char* argv[]) {
	
	// initialisation composant graphique (cf gui.c)
	GUI gui;
	if (GUI_init(&gui) == -1) exit(-1);
	
	// Repeinte du fond d'écran
	FB_background(&gui, 0, 0, 0);
	
	// Recherche de la police
	char path[256]; // Dossier de recherche (doit contenir le « / » final !)
	if (args > 1) { // Si on a un arguments, on recherche dans ce dossier
		memcpy(path, argv[1], 256);
	} else { // Dossier par defaut si lancé sans arguments : /sdcard/
		memcpy(path, "/sdcard/", 256);
	}
	int size = 0; // Taille de la police
	if (getFont(path, &size) == -1) { // cf font.c
		printf("Impossible de trouver une police (dossier %s)\n", path);
		return -1;
	}
	printf("Font : %s, size : %i\n", path, size);
	// Enregistrement de la police et de la taille dans la structure gui pour utilisation lors de l'affichage
	gui.ip_size = size;
	gui.ip_font = path;
	
	// Police utilisé pour les labels de boutons, et l'affichage de la team
	FT_setFont(&gui, "/home/elie/FreeSans.ttf");
	// Ajout du bouton, avec un label utilisant la police précédement chargé
	// La fonction buttonEvent avec l'arguments &gui sera appelée sur les events (cf ts.c)
	TS_button* button = TS_addButton(&gui, 400, 222, 80, 50, "Retour", 16, -1, -1, buttonEvent, (void*)&gui);
	
	// Simulation d'un clic sur « Retour » pour afficher la page avec l'IP
	buttonEvent(button, 0, &gui);
	
	// Attente du thread d'interface graphique avant de quitter
	// (Dans les faits, le programme s'arrête là)
	pthread_join(gui.pth, NULL);
	FB_close(&gui);
	return 0;
}

// Fonction enregistré au près du bouton pour être executé sur les events
void buttonEvent(TS_button* button, int event, void* args) {
	if (event == 0) { // cf ts.c pour les codes d'event, en bref : 0 = clic
		GUI* gui = args;
		if (button->label[0] == 'T') { // Le label du bouton indiqué « Team » → affichage team
		
			FB_background(gui, 0, 0, 0); // Font d'écran noir
			
			// On charge la bonne police pour les crédits, on set la taille, la couleurs
			FT_setFont(gui, "/home/elie/FreeSans.ttf");
			FT_setSize(gui, 40);
			FT_setColor(gui, 31, 0, 0);
			FT_printString(gui, "Djanos - Lockidor", -1, 75); // -1 → centré suivant x (cf ft.c)
			FT_setSize(gui, 35);
			FT_setColor(gui, 0, 0, 31);
			FT_printString(gui, "ENSEEIHT", -1, 150);
			FT_setSize(gui, 35);
			FT_setColor(gui, 0, 50, 20);
			FT_printString(gui, "Octobre 2011", -1, 225);
			
			// On change le label du bouton et on le re-dessine
			button->label = "IP";
			TS_printButton(gui, button);
			
		} else { // Le label du bouton indiqué « Retour » → affichage IP
		
			FB_background(gui, 31, 0, 0); // Font d'écran rouge (cachier des charges)
			
			FT_setFont(gui, gui->ip_font); // Police trouvé sur la carte SD
			FT_setSize(gui, gui->ip_size); // Taille contenu dans le nom du fichier
			FT_setColor(gui, 0, 0, 0); // En noir (cahier des charges)
			FT_printString(gui, getIP("eth0"), -1, -1); // cf ip.c pour l'obtention de l'ip (ioctl)
			
			// On change le label du bouton et on le re-dessine
			button->label = "Team";
			FT_setFont(gui, "/home/elie/FreeSans.ttf"); // avec la police non aléatoire, c'est mieux
			TS_printButton(gui, button);
			
		}
	}
}

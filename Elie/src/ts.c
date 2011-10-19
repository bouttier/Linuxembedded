#include "ts.h"

void TS_init(GUI* gui) {
	
	// Obtention du device depuis la variable d'environnement TSLIB_TSDEVICE
	// (par defaut : /dev/touchscreen)
	if(getenv("TSLIB_TSDEVICE")) {
		gui->ts_device = getenv("TSLIB_TSDEVICE");
	} else {
		gui->ts_device = "/dev/touchscreen";
	}
	printf("Touchscreen device : %s\n", gui->ts_device);
	// Ouverture du TS avec la fonction de la tslib
	gui->ts_dev = ts_open(gui->ts_device, 0);
	if (!gui->ts_dev) {
		printf("Error : TS_init::ts_open");
		exit(1);
	}
	if (ts_config(gui->ts_dev)) {
		printf("Error : TS_init::ts_config");
		exit(1);
	}
	
	gui->ts_button = NULL; // Pas encore de bouton, le pointeur est nul
	gui->run = 1; // C'est parti, on arrête plus !
	// On lance le thread, il execute TS_thread, et obtient gui en argument
	pthread_create(&gui->pth, NULL, TS_thread, (void*)gui);
	
}

// Le thread de contrôle du touchscreen
void* TS_thread(void* args) {
	
	GUI* gui = args; // La struct GUI bien utile pour appelé les fonctions qui la demande en arguments
	int x = -1; int y = -1; unsigned int p = 2; // Pour sauvegarder les états précédent du touchscreen
	int retour; // pour enregistré les codes de retour des fonctions
	TS_button* buttonEnabled = NULL; // Pour savoir le boutons actuellement enfoncé
	TS_button* button; // Le bouton qui a le handle lors de la boucle sur les boutons
	int onButton = 0; // Pour savoir si on a appuyer sur un bouton
	
	while (gui->run == 1) {
		retour = ts_read(gui->ts_dev, &gui->ts_samp, 1); // Lecture du touchscreen
		if (retour < 0) { printf("Error : ts_read fail\n"); exit(1); }
		// On analyse les données seulement si il s'agit d'un nouvelle event du touchscreen différent du précédent
		if (retour == 1 && ((gui->ts_samp.x != x) || (gui->ts_samp.y != y) || (p != gui->ts_samp.pressure))) {
			x = gui->ts_samp.x; y = gui->ts_samp.y; p = gui->ts_samp.pressure; // Enregistrement de l'event pour les prochaines comparaisons
			if (p) { // pixel appuyé
				onButton = 0; // A priori, on n'a pas appuyé sur un bouton
				button = gui->ts_button; // On charge le pointeur vers le dernier bouton
				while (button != NULL) { // On parcour tous les bouton
					if (x >= button->x && x < button->x+button->width && y >= button->y && y < button->y+button->height) { // Sur le bouton ?
						if (!button->state) { // Si le bouton n'est pas enfoncé (si il l'est déjà, rien à faire)
							if (buttonEnabled != NULL) { // Si il y a déjà un bouton activé
								buttonEnabled->state = 0; // On le désactive
								TS_printButton(gui, buttonEnabled); // Et on le redessine
								 // On envoit l'event « relaché par glissage ailleurs sur l'écran »
								(*buttonEnabled->onEvent)(buttonEnabled, -1, buttonEnabled->args);
							}
							button->state = 1; // On active le nouveau
							TS_printButton(gui, button); // On le redessine
							(*button->onEvent)(button, 1, button->args); // On envoit l'event
							buttonEnabled = button; // On met le pointeur du bouton activé sur ce bouton
						}
						onButton = 1; // On a cliqué un bouton (pour skipper les actions en cas de clic hors bouton)
						break; // Pas besoin de checker les bouton suivant (chevauchaffe à la diligeance de l'utilisateur)
					}
					button = button->previous; // On passe au bouton suivant
				}
				if (!onButton) { // Si on a cliqué sur aucun bouton
					if (buttonEnabled != NULL) { // Et si il y avait un bouton enfoncé
						buttonEnabled->state = 0; // On le relâche
						TS_printButton(gui, buttonEnabled); // On le redessine
						// On envoit l'event « relaché par glissage ailleurs sur l'écran »
						(*buttonEnabled->onEvent)(buttonEnabled, -1, buttonEnabled->args);
						buttonEnabled = NULL;
					}
				}
			} else { // pixel relaché
				if (buttonEnabled != NULL) { // Si il y avait un bouton enfoncé, c'est qu'on vient de le relacher
					buttonEnabled->state = 0; // On le marque comme relaché
					TS_printButton(gui, buttonEnabled); // On le redessine
					(*buttonEnabled->onEvent)(buttonEnabled, 0, buttonEnabled->args); // On envoit l'event
					buttonEnabled = NULL; // Il n'y a plus de bouton enfoncé
				}
			}
		} else {
			usleep(1000); // Soulagement du processeur, réellement nécessaire !
		}
	}
	
	pthread_exit(0);
}

// Ajoute un bouton en (x,y) de dimension (width,heinght)
// La fonction onEvent sera appelé à chaque event, avec l'arguments args
// Pour les paramètres du label et plus de détaille, cf gui.h, struct TS_button
TS_button* TS_addButton(GUI* gui, int x, int y, int width, int height, char* label, int size, int xoffset, int yoffset, void (*onEvent)(TS_button*, int, void*), void* args) {
	
	TS_button* button = malloc(sizeof(TS_button));
	if (!button) { printf("TS_addButton::malloc fail\n"); exit(1); }
	//~ button->id = gui->ts_button_id; gui->ts_button_id++;
	// Enregistrement des paramètres du bouton
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
	button->previous = gui->ts_button; // Ce nouveau bouton enregistre le pointeur vers le précédent
	//~ button->next = NULL;
	gui->ts_button = button; // Le dernier bouton est maintenant celui là
	TS_printButton(gui, button); // On le dessine
	return button; // On retourne un pointeur vers le bouton ainsi créé
	
}

// Fonction d'affichage des boutons
void TS_printButton(GUI* gui, TS_button* button) {
	
	//~ printf("\tPrint button : position (%i,%i), size %i×%i, state : %i\n", button->x, button->y, button->width, button->height, button->state);
	int x; int y; // Pour le parcour de la zone pixel par pixel
	FT_setColor(gui, 0, 0, 0); // Label en noir (peut-être plus tard dépendera du bouton)
	FT_setSize(gui, button->size); // La taille du texte est spécifié pour chaque bouton
	// Récupération des offset (décallage du texte, voir struc TS_button dans gui.h)
	int xoffset = button->xoffset; 
	int yoffset = button->yoffset;
	if (xoffset == -1 || yoffset == -1) { // Si un offset a pour valeur -1, c'est une demande de centrage
		int size[2]; // Tableau de destination de la taille de la chaîne de caractère
		FT_getSizeString(gui, button->label, size); // Récupération de la taille (size[0] = longueur en pixel, size[1] = hauteur en pixel)
		if (xoffset == -1) { // Demande de centrage horizontal
			xoffset = (button->width - size[0])/2; // (Largeur du bouton - largeur de la chaîne de texte)/2
		}
		if (yoffset == -1) { // Demande de centrage vertical
			yoffset = (button->height - size[1])/2; // (Hauteur du bouton - hauteur de la chaîne de texte)/2
		}
	}
	// On dessine un rectangle gris pour l'intérieur du bouton (couleur paramétrable à l'avenir ?)
	FB_rect(gui, button->x+1, button->y+1, button->width-1, button->height-1, 25, 51, 25);
	if (button->state) { // Bouton enfoncé → haut/gauche noir, bas/droite blanc
		for (x = button->x ; x < button->x+button->width ; x++) { // Bordures horizontales
			FB_set(gui, x, button->y, 0, 0, 0); // Bordure haute noire
			FB_set(gui, x, button->y+button->height-1, 31, 63, 31); // Bordure basse blanche
		}
		for (y = button->y ; y < button->y+button->height ; y++) { // Bordures verticales
			FB_set(gui, button->x, y, 0, 0, 0); // Bordure gauche noire
			FB_set(gui, button->x+button->width-1, y, 31, 63, 31); // Bordure droite blanche
		}
		// Affichage du label, décallé d'un pixel en diagonal pour l'effet d'enfoncement
		FT_printString(gui, button->label, button->x+xoffset+1, button->y+button->height-yoffset+1);
	} else { // Bouton relacher
		for (x = button->x ; x < button->x+button->width ; x++) { // Bordures horizontales
			FB_set(gui, x, button->y, 31, 63, 31); // Bordure haute blanche
			FB_set(gui, x, button->y+button->height-1, 0, 0, 0); // Bordure basse noire
		}
		for (y = button->y ; y < button->y+button->height ; y++) { // Bordures verticales
			FB_set(gui, button->x, y, 31, 63, 31); // Bordure gauche blanche
			FB_set(gui, button->x+button->width-1, y, 0, 0, 0); // Bordure droite noire
		}
		// Affichage du label, offsets normaux
		FT_printString(gui, button->label, button->x+xoffset, button->y+button->height-yoffset);
	}
	
}

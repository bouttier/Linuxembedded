#ifndef _GUI_H
#define _GUI_H

// Lib du noyau pour le framebuffer
#include <linux/fb.h>
// Lib FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
// Lib TouchScreen
#include "tslib.h"
// pthread pour le touchscreen
#include <pthread.h>

// Structure de definition des boutons
typedef struct TS_button TS_button;
struct TS_button {
	
	//~ int id;
	int x; // Position x
	int y; // Position y
	int width; // Largeur
	int height; // Hauteur
	char* label; // Teste du bouton
	int size; // Taille de la police pour afficher le label
	int xoffset; // Décallage vers la droite du label par rapport au bord gauche du bouton (-1 centre horizontalement)
	int yoffset; // Décallage vers le **haut** (yoffset est soustrait à y) par rapport au bas du bouton (-1 centre horizontalement)
	unsigned int state; // État du bouton (0 = relaché, 1 = enfoncé)
	void (*onEvent)(TS_button* button, int event, void* args); // Pointeur vers la fonction a appeler lors d'un event
	void* args; // Arguments qui seront passés à la fonction appelé lors d'un event
	TS_button* previous; // Pointeur vers le bouton précédant (liste chaînée)
	//~ TS_button* next; // Deviendra utile lors de l'ajout de la fonction pour retirer des boutons (double liste chaînée)
	
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
	// Structure spécifique à la ftlib
	FT_Library ft_library;
	FT_Face ft_face;
	// Couleur en cour d'utilisation
	int ft_color_r;
	int ft_color_g;
	int ft_color_b;
	//~ char* ft_font; // Font actuellement chargé
	//~ int size; // Taille en cour d'utilisation
	
	/* Touchscreen */
	pthread_t pth; // Thread qui contrôle les évènements sur le touchscreen
	char* ts_device; // L'adresse du device (ex: /dev/touchscreen)
	struct tsdev* ts_dev; // Structure propre à tslib
	struct ts_sample ts_samp; // Idem
	//~ int ts_button_id;
	TS_button* ts_button; // Adresse vers le dernier bouton de la liste chaîné de bouton
	int run; // À mettre à 0 pour arrêter le thread, non utilisé en l'état des choses
	
	/* IP */
	// Couleur et taille de la police pour écrire l'ip
	int ip_size;
	char* ip_font;
	
};


#include "fb.h"
#include "ft.h"
#include "ts.h"

int GUI_init(GUI* gui);

#endif /* _GUI_H */

#include "font.h"

// Obtenir le path de la font à partir du path d'un dossier, renvoit la font et la taille contenue dans le nom du fichier
// Renvoit 0 pour succès, -1 pour font non trouvé
int getFont(char* path, int* size) {

	struct dirent *item; // Struct pour readdir
	DIR *dir = opendir(path); // Ouverture du dossier
	if (dir == NULL) {
		printf("Erreur : impossible d'ouvrir le dossier de la police\n");
		return -1;
	}
	
	int fontFound = 0; // On a pas encore trouvé la font
	while ((item = readdir(dir)) != NULL) { // Parcour du dossier
		if(sscanf(item->d_name, "concours-%i.ttf", size) && 0 < *size && *size < 256) { // Masque sur le nom des fichier
			fontFound = 1; // Font trouvé !
			break;
		}
	}
	if (!fontFound) return -1;
	
	strcat(path, (const char*)item->d_name); // Sauvegarde du path de la font
	closedir(dir); // Fermeture du dossier
	
	return 0;
	
}

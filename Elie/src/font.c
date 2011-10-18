#include "font.h"

int getFont(char* path, int* size) {

	struct dirent *item;
	DIR *dir = opendir(path);
	if (dir == NULL) {
		printf("Erreur : impossible d'ouvrir le dossier de la police\n");
		return -1;
	}
	
	int fontFound = 0;
	while ((item = readdir(dir)) != NULL) {
		if(sscanf(item->d_name, "concours-%i.ttf", size) && 0 < *size && *size < 256) {
			fontFound = 1;
			break;
		}
	}
	if (!fontFound) return -1;
	
	strcat(path, (const char*)item->d_name);
	closedir(dir);
	
	return 0;
	
}

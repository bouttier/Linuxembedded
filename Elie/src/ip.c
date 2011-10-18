#include "ip.h"

char* getIP(char* interface) {
	char* ip; // Chaîne de retour
	ip = (char*)malloc(16 * sizeof(char));
	int descSock;
	struct ifreq devea;
	struct sockaddr_in *sa;
	//récupération d'un pointeur sur toutes les cartes disponible;
	struct if_nameindex *nameindex = if_nameindex();
	if(nameindex == NULL){
		perror("if_nameindex ");
		exit(errno);
	}
	int i = 0;
	while(1){
		if(nameindex[i].if_name == NULL) break;//il n'y a plus de cates on sort
		//Copie du nom dans la structure ifreq et au passage on prépare l'index pour la prochaine carte
		strcpy(devea.ifr_name, nameindex[i++].if_name);
		//Affichage de celui-ci
		if (strcmp(devea.ifr_name, interface) == 0) {
			//on ouvre un socket pour chacune des cartes que l'on closera après
			descSock = socket(AF_INET, SOCK_DGRAM, 0);
			if (descSock < 0){
				perror("socket ");
				//On relache le pointeur dynamiquement alloue par if_nameindex
				if_freenameindex(nameindex);
				exit(errno);
			}
			//Récupération de l'adresse ip
			if (ioctl(descSock,SIOCGIFADDR, &devea) < 0) {
				printf("ERROR MAYBE NOT STARTED ?\n");
				exit(1);
			} else {
				//Récupère la structure sockaddr_in contenue dans ifreq
				sa = (struct sockaddr_in *)&devea.ifr_ifru.ifru_addr;
				//Convertion en ascii et copie dans la chaîne de retour
				strcpy(ip, inet_ntoa(sa->sin_addr));
			}
			//Ferme le socket pour passer au prochain
			close(descSock);
		}
	}
	//Relache le pointeur dynamiquement alloue par if_nameindex
	if_freenameindex(nameindex);
	return ip;
	
}

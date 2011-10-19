================ Équipe =================
Elie Bouttier (djanos)
Franklin Delehelle (Lockidor)
Équipe 7totomatix - ENSEEIHT, Toulouse
Concours Linuxembedded 2011
=========================================

=============== Fichiers ================

README.txt
	Ce fichier
buildroot-config
	Fichier de configuration de buildroot
home (home/elie/FreeSans.ttf)
	Police utilisé par le programme pour les textes autres que l'ip
package (package/Config.in, package/ipshow/Config.in, package/ipshow/ipshow.mk) :
	Fichier de config pour buildroot pour la compilation de notre programme (nommé ipshow)
S50ipshow
	Script de lancement d'ipshow
ipshow-1.0.tar.gz
	Les sources de notre programme

01-buildroot-download.sh
	Télécharge l'archive buildroot depuis le site officiel

02-buildroot-extract.sh
	Décompression de buildroot

03-buildroot-tune.sh : Modification de buildroot
	Copie de la congifuration de buildroot (buildroot-config)
	Copie des fichier de config de buildroot pour la création de notre archive
	Copie de l'archive des sources d'ipshow dans le répertoire de téléchargement de buildroot
	Copie du script de lancement d'ipshow dans le skeleton de buildroot
	Copie de la police FreeSans.ttf dans le package « customize »

04-host-required-packages.sh
	Installe les dépendances nécessaire à l'execution de buildroot
		build-essential
		bison
		flex
		gettext
		unzip

05-buildroot-make.sh
	Lance make dans le dossier de buildroot

06-images.sh
	Copie uImage et u-boot.bin depuis l'output de buildroot dans le répertoire courant
all.sh
	Éxecute toute les étapes
=========================================


========= Création de la NAND ===========


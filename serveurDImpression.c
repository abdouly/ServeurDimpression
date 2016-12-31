#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "../communication/communication.h"

//initialiser le serveur pour qu'il recoive les requetes
int init_serveur(char *serveur){
	int numServeur;
    unlink(serveur);
    if ((numServeur = initialiserServeur(serveur)) < 0)
  	{
		fprintf(stderr, "Erreur initialiserServeur: %s: %s\n",
			  serveur, messageErreur(numServeur));
	  	exit(1);
  	}
  	return numServeur;
}
//fonction du scheduler
void cups_scheduler(int numServeur){
	int numCommunication;
	for(;;){
		if((numCommunication = accepterCommunication(numServeur)) < 0){
		 	fprintf(stderr, "Erreur accepterCommunication: %s: %s\n",
				 serveur, messageErreur(numCommunication));
	 	 	exit(1);
	   	}
        // lire le nom du fichier à transferer
	}
}
//fonction d'un cups filter
void cups_filter(){
   
}
//fonction d'un imprimante locale
void imprimante_locale(){
 	for(;;){
 		
 	}
}

//fonctions qui demarre les threads des imprimantes locales
void demarrer_imprimantes_locales(tableau,int taille){
	int i;

}
//fonctions qui demarres les threads des cupsFilters
void demarrer_filters(int nb){

}
//fonction qui demarre le scheduler
void demarrer_scheduler(){

}

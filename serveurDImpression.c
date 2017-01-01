#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#include "communication/communication.h"

typedef struct imprimante{
  char *nom;
  int type;       //contient le type de l'imprimante. 0 : locale, 1 : distance
}Imprimante;

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
void * cups_scheduler(void *args){
	int numCommunication;
	for(;;){
		if((numCommunication = accepterCommunication(numServeur)) < 0){
		 	//fprintf(stderr, "Erreur accepterCommunication: %s: %s\n",serveur, messageErreur(numCommunication));
	 	 	exit(1);
	   	}
        // lire le nom du fichier à transferer
	}
}

//fonction d'un cups filter
void * cups_filter(void *args){
   for(;;){

   }
}
//fonction d'un imprimante locale
void * imprimante_locale(void *args){
 	for(;;){
 		
 	}
}

//fonctions qui demarre les threads des imprimantes locales
void demarrer_imprimantes_locales(int *tableau,int taille){
	int i;
}
//fonctions qui demarres les threads des cupsFilters
void demarrer_filters(int nb){

}
//fonction qui demarre le scheduler
void demarrer_scheduler(){

}

void lectureConfiguration(char *fileName, char *serverName, Imprimante *imprimantes) {
  FILE *file;
  char buffer[512];
  char nomImprimante[256];
  int taille = 0;
  int nbImprimantes = 0;
  Imprimante imp;
  imprimantes = (Imprimante *) malloc(sizeof(Imprimante));
  
  file = fopen(fileName, "r");
  if(file == NULL) {
    fprintf(stderr,"File %s not found.\n",fileName);
    exit(-1);
  }
  
  
  while (fgets(buffer, 512, file) != NULL){ 
    switch(buffer[0]) {
      case '#': //commentaire
	break;
      
      case 's':
	sscanf(&buffer[2],"%s",serverName);
	break;
      
      case 'l':
	sscanf(&buffer[2],"%s",nomImprimante);
	imp.nom = nomImprimante;
	imp.type = 0;
	if(nbImprimantes != 0)
	  imprimantes = (Imprimante *) realloc(imprimantes, sizeof(Imprimante)*(nbImprimantes+1));
	imprimantes[nbImprimantes++] = imp;
	break;
	
      case 'd':
	sscanf(&buffer[2],"%s",nomImprimante);
	imp.nom = nomImprimante;
	imp.type = 1;
	if(nbImprimantes != 0)
	 imprimantes = (Imprimante *) realloc(imprimantes, sizeof(Imprimante)*(nbImprimantes+1));
	imprimantes[nbImprimantes++] = imp;
	break;
    }
  } 
}

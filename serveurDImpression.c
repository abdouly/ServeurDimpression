#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>

#include "communication/communication.h"

typedef struct imprimante{
  char *nom;
  int type;       //contient le type de l'imprimante. 0 : locale, 1 : distance
}Imprimante;

typedef struct infos_serveur
{
	char nom[20];
	int numServeur;
}Infos_serveur;

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
	int numCommunication,numServeur;
	char serveur[20];
	Infos_serveur * infos = (Infos_serveur *) args;
	numServeur = infos->numServeur;
	strcpy(serveur,infos->nom);
	printf("[ Scheduler ] demarrage OK %d\n",numServeur);
	for(;;){
		if((numCommunication = accepterCommunication(numServeur)) < 0){
		 	fprintf(stderr, "Erreur accepterCommunication: %s: %s\n",serveur, messageErreur(numCommunication));
	 	 	exit(1);
	   	}
	}
}

//fonction d'un cups filter
void * cups_filter(void *args){
   printf("[ Filter ] demarrage OK\n");
   for(;;){

   }
}
//fonction d'un imprimante locale
void * imprimante_locale(void *args){
	char fichier_imprimante[20];
	char fichier;
 	printf("[ Imprimante locale ] demarrage OK\n");
 	for(;;){
 		
 	}
}

//fonctions qui demarre les threads des imprimantes locales
void demarrer_imprimantes_locales(Imprimante *imprimantes,int nb_imprimantes,pthread_t *idThdImp,int *nb_imprimantes_locale){
	int i,etat,j;
	j = 0;
	Imprimante p;
	printf("[ Main ] demarrage Imprimantes locales\n");
	for(i=0; i < nb_imprimantes; i++){
		p = imprimantes[i];
		if(p.type == 0){
			if((etat=pthread_create(&idThdImp[j++], NULL, imprimante_locale,NULL)) != 0){
				fprintf(stderr, "[ Main ] Echec demarrage Imprimante locale\n");
				exit(1);
			}
		}
	}
	*nb_imprimantes_locale = j;
	sleep(1);
}
//fonctions qui demarres les threads des cupsFilters
void demarrer_filters(int nb_filters,pthread_t *idThdFil){
	int i,etat;
	printf("[ Main ] demarrage Filters\n");
	for(i = 0; i < nb_filters; i++){
		if((etat=pthread_create(&idThdFil[i], NULL, cups_filter,NULL)) != 0){
			fprintf(stderr, "[ Main ] Echec demarrage Filters\n");
			exit(1);
		}
	}
	sleep(1);	
}
//fonction qui demarre le scheduler
void demarrer_scheduler(pthread_t *idScheduler,Infos_serveur infos){
 	int etat;
 	printf("[ Main ] demarrage Scheduler\n");
 	if((etat=pthread_create(idScheduler, NULL, cups_scheduler,&infos)) != 0){
		fprintf(stderr, "[ Main ] Echec demarrage scheduler\n");
		exit(1);
	}
	sleep(1);
}

void lectureConfiguration(char *fileName, char *serverName, Imprimante **imprimantes,int *nb_imprimantes) {
  FILE *file;
  char buffer[512];
  char nomImprimante[256];
  int nbImprimantes = 0;
  Imprimante imp;
  *imprimantes = (Imprimante *) malloc(sizeof(Imprimante));
  
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
	  *imprimantes = (Imprimante *) realloc(*imprimantes, sizeof(Imprimante)*(nbImprimantes+1));
	(*imprimantes)[nbImprimantes++] = imp;
	break;
	
      case 'd':
	sscanf(&buffer[2],"%s",nomImprimante);
	imp.nom = nomImprimante;
	imp.type = 1;
	if(nbImprimantes != 0)
	 *imprimantes = (Imprimante *) realloc(*imprimantes, sizeof(Imprimante)*(nbImprimantes+1));
	(*imprimantes)[nbImprimantes++] = imp;
	break;
    }
  }
  *nb_imprimantes = nbImprimantes; 
}

int main(int argc,char *argv[]){
	pthread_t idScheduler;
	pthread_t idThdImp[10];
	pthread_t idThdFil[10];
	int numServeur,etat;
	Infos_serveur infos;
	Imprimante * imprimantes = NULL;
	int nb_imprimantes,nbFilters,i,nb_imprimantes_locale;
	nbFilters = 4;
	char serveur[20];
	printf("[ Main ] Initialisation du serveur d'impression\n");
	lectureConfiguration(argv[1],serveur,&imprimantes,&nb_imprimantes);
	numServeur = init_serveur(serveur);
	infos.numServeur = numServeur;
	strcpy(infos.nom,serveur);
	demarrer_scheduler(&idScheduler,infos);
	demarrer_filters(nbFilters,idThdFil);
	demarrer_imprimantes_locales(imprimantes,nb_imprimantes,idThdImp,&nb_imprimantes_locale);
	printf("[ Main ] Initialisation serveur OK\n");
	if((etat = pthread_join(idScheduler,NULL)) != 0)
		fprintf(stderr, "[ Main ] Erreur fermeture Scheduler\n");
	for(i = 0; i < nbFilters; i++){
		if ((etat = pthread_join(idThdFil[i], NULL)) != 0)
			fprintf(stderr, "[ Main ] Erreur fermeture Filter\n");
	}
	for(i = 0; i < nb_imprimantes_locale; i++){
		if ((etat = pthread_join(idThdImp[i], NULL)) != 0)
			fprintf(stderr, "[ Main ] Erreur fermeture Imprimante locale\n");
	}
	exit(0);
}
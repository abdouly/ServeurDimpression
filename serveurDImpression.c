#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>

#include "communication/communication.h"

#define TAILLE 10
#define TAILLE_FILE_IMP 10

typedef struct imprimante{
  char *nom;
  int type;       //contient le type de l'imprimante. 0 : locale, 1 : distance
}Imprimante;



typedef struct job
{
	char * nom_imprimante;
	char * nom_fichier;
	int nb_copies;
	int type_impression; // 0 pour recto et 1 pour recto verso
}*Job;

typedef struct impression
{
	char *nom;
	char recto; //0 : recto , 1 : recto/verso
	int nb_copies;
}*Impression;

typedef struct infos
{
	Impression file_imprimante[TAILLE_FILE_IMP];
	int indice_depot;
	int indice_retrait;
	int nb_cases_remplies;
	char *nom_imprimante;
	pthread_cond_t place_disponible;
	pthread_cond_t fichier_disponible;
}*Infos;

Job job_files[TAILLE]; 


int indice_depot = 0, indice_retrait = 0;

void envoyer_document(char * nom_imprimante) {

}

void transformer_fichier_pdf(char *nom_fichier) {

}

void transformer_fichier_text(char *nom_fichier) {

}

void transformer_fichier_image(char *nom_fichier) {

}

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
	/*int numCommunication;
	for(;;){
		if((numCommunication = accepterCommunication(numServeur)) < 0){
		 	fprintf(stderr, "Erreur accepterCommunication: %s: %s\n",serveur, messageErreur(numCommunication));
	 	 	exit(1);
	   	}
         lire le nom du fichier à transferer
	}*/
	printf("Scheduler demarre\n");
	for(;;);
}

//fonction d'un cups filter
void * cups_filter(void){
   printf("Filter demarre\n");
   char extention[64];
   char nom_fichier[256];
   for(;;){
 	  	Job job = job_files[indice_retrait];
   		indice_retrait = (indice_retrait +1)%TAILLE;
   		strcpy(nom_fichier,job->nom_fichier);
   		strcpy(extention, strchr(nom_fichier, '.')+1); 
   		
   		if(strcmp(extention, "txt") == 0) {
   			transformer_fichier_pdf(nom_fichier);
   			envoyer_document(job->nom_imprimante);
   		} else if(strcmp(extention, "pdf") == 0) {
   			transformer_fichier_text(nom_fichier);
   			envoyer_document(job->nom_imprimante);
   		} else { //image JPEG
   			transformer_fichier_image(nom_fichier);
   			envoyer_document(job->nom_imprimante);
   		}

   }
}


//fonction d'un imprimante locale
void * imprimante_locale(void *args){
 	printf("Imprimante locale demarre\n");
 	for(;;){
 		
 	}
}

//fonctions qui demarre les threads des imprimantes locales
void demarrer_imprimantes_locales(Imprimante *imprimantes,int nb_imprimantes,pthread_t *idThdImp){
	int i,etat,j;
	j = 0;
	Imprimante p;
	for(i=0; i < nb_imprimantes; i++){
		p = imprimantes[i];
		if(p.type == 0){
			if((etat=pthread_create(&idThdImp[j++], NULL, imprimante_locale,NULL)) != 0){
				fprintf(stderr, "Echec demarrage imprimante\n");
				exit(1);
			}
		}
	}
	printf("Fin demarrage imprimantes locales\n");
}
//fonctions qui demarres les threads des cupsFilters
void demarrer_filters(int nb_filters,pthread_t *idThdFil){
	int i,etat;
	for(i = 0; i < nb_filters; i++){
		if((etat=pthread_create(&idThdFil[i], NULL, cups_filter,NULL)) != 0){
				fprintf(stderr, "Echec demarrage Filter\n");
				exit(1);
		}
	}
	printf("Fin demarrage filters\n");	
}
//fonction qui demarre le scheduler
void demarrer_scheduler(pthread_t idScheduler){
 	int etat;
 	if((etat=pthread_create(&idScheduler, NULL, cups_scheduler,NULL)) != 0){
		fprintf(stderr, "Echec demarrage scheduler\n");
		exit(1);
	}
	printf("Fin demarrage scheduler\n");
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
	Imprimante * imprimantes = NULL;
	int nb_imprimantes;
	char serveur[20];
	lectureConfiguration(argv[1],serveur,&imprimantes,&nb_imprimantes);
	demarrer_scheduler(idScheduler);
	demarrer_filters(4,idThdFil);
	if(imprimantes == NULL)
		printf("Oulalalalalalal\n");
	demarrer_imprimantes_locales(imprimantes,nb_imprimantes,idThdImp);
	for(;;);
}
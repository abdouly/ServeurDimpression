#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>

#include "communication/communication.h"
#include "impression.h"

Job jobs[TAILLE_FILE_SCH];
pthread_cond_t place_disponible;
pthread_cond_t job_disponible;
pthread_mutex_t mutex;
Infos * file_imprimantes = NULL;//tableau des files d'attentes des imprimantes 
Imprimante * imprimantes = NULL;//tableau des imprimantes
char ** machines = NULL;//tableau des machines sites
char nomServeur[30];//nom du serveur
pthread_t idThdImp[NB_MAX_FILTER];
pthread_t idThdFil[NB_MAX_PRINTERS];

int nbFilters,nbImprimantes,nbMachines;
int indice_depot = 0, indice_retrait = 0;

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

//retourne 1 si machine authentifié 0 sinon
int authentifier_machine(char *nomMachine){
	int i;
	for(i = 0; i < nbMachines; i++){
		if(strcmp(machines[i],nomMachine) == 0)
			break;
	}
	return i < nbMachines;
}

void traiter_impression(Demande requete,int numCommunication){

}
void etat_imprimante(void ){

}
void etat_impression(void){

}
void annuler_impression(void){

}

void traiter_requete(Demande requete,int numCommunication){
	if(authentifier_machine(requete.machine) == 0)
		return;
	switch(requete.type){
		case IMPRESSION:
			traiter_impression(requete,numCommunication);
			break;
		case ETAT_IMPRIMANTE:
			etat_imprimante();
			break;
		case ETAT_IMPRESSION:
			etat_impression();
			break;
		case ANNULER:
			annuler_impression();
			break;
	}
}
//fonction du scheduler
void * cups_scheduler(void *args){
	int numCommunication,numServeur;
	Demande requete;
	Infos_serveur * infos = (Infos_serveur *) args;
	numServeur = infos->numServeur;
	printf("[ Scheduler ] demarrage OK \n");
	for(;;){
		if((numCommunication = accepterCommunication(numServeur)) < 0){
		 	fprintf(stderr, "Erreur accepterCommunication: %s: %s\n",infos->nom, messageErreur(numCommunication));
	 	 	exit(1);
	   	}
	   	recevoirOctets(numCommunication,&requete,sizeof(Demande));
	   	traiter_requete(requete,numCommunication);
	   	cloreCommunication(numCommunication);
	}
}

//fonction d'un cups filter
void * cups_filter(void *args){
   printf("[ Filter ] demarrage OK\n");
   //Job job = job_files[indice_retrait];
   //indice_retrait = (indice_retrait +1)%TAILLE;
   for(;;){
 	  	/*Job job = job_files[indice_retrait];
   		indice_retrait = (indice_retrait +1)%TAILLE;
   		strcpy(nom_fichier,job->nom_fichier);
   		strcpy(extention, strchr(nom_fichier, '.')+1); */
   }
}

//fonction d'un imprimante locale
void * imprimante_locale(void *args){
	char fichier_imprimante[20];
	char fichier[20];
	char line[256];
	int taille,numero_file;
	int inputFile,outputFile;
	numero_file = *(int *) args;
 	printf("[ Imprimante locale ] demarrage OK\n");
 	for(;;){
 		inputFile = open(fichier,O_RDONLY);
 		outputFile = open(fichier_imprimante,O_WRONLY|O_CREAT|O_APPEND,S_IRWXU);
 		while((taille = read(inputFile,line,256)) > 0 )
 			write(outputFile,line,taille);
 		close(inputFile);
 		close(outputFile);
 	}
}

//fonction backend d'une imprimante distante
void * backend(void *args){

	printf("[ Backend ] demarrage OK\n");
	for(;;);
}

//fonctions qui demarre les threads des imprimantes locales et les backends des distantes
void demarrer_imprimantes(void){
	int i,etat;
	Imprimante p;
	printf("[ Main ] demarrage Imprimantes locales et Backends\n");
	for(i=0; i < nbImprimantes; i++){
		p = imprimantes[i];
		if(p.type == IMPRIMANTE_LOCALE){
			if((etat=pthread_create(&idThdImp[i], NULL, imprimante_locale,NULL)) != 0){
				fprintf(stderr, "[ Main ] Echec demarrage Imprimante locale\n");
				exit(1);
			}
		}else {
			if((etat=pthread_create(&idThdImp[i], NULL, backend,NULL)) != 0){
				fprintf(stderr, "[ Main ] Echec demarrage backend\n");
				exit(1);
			}
		}
	}
	sleep(1);
}
//fonctions qui demarres les threads des cupsFilters
void demarrer_filters(void){
	int i,etat;
	printf("[ Main ] demarrage Filters\n");
	for(i = 0; i < nbFilters; i++){
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
//
void init_file_attente(void){
	int i;
	printf("[ Main ] demarrage des verrous\n");
	file_imprimantes = (Infos *) malloc(sizeof(Infos) * nbImprimantes);
	for(i = 0; i < nbImprimantes; i++){
		file_imprimantes[i].indice_depot = 0;
		file_imprimantes[i].indice_retrait = 0;
		file_imprimantes[i].nb_cases_remplies = 0;
		pthread_cond_init(&file_imprimantes[i].place_disponible,NULL);
		pthread_cond_init(&file_imprimantes[i].fichier_disponible,NULL);
		pthread_mutex_init(&file_imprimantes[i].mutex,NULL);
	}
	pthread_cond_init(&place_disponible,NULL);
	pthread_cond_init(&job_disponible,NULL);
	pthread_mutex_init(&mutex,NULL);
	printf("[ Main ] demarrage verrous OK\n");
}

void lectureConfiguration(char *fileName) {
  FILE *file;
  char buffer[512];
  char nomImprimante[256];
  char nomMachine[30];
  Imprimante imp;
  nbImprimantes = 0;
  nbMachines = 0;
  printf("[ Main ] lecture du fichier de configuration\n");
  machines = (char **) malloc(sizeof(char *));
  imprimantes = (Imprimante *) malloc(sizeof(Imprimante));
  file = fopen(fileName, "r");
  if(file == NULL) {
    fprintf(stderr,"[ Main ] impossible d'ouvrir le fichier de configuration %s \n",fileName);
    exit(-1);
  }
  while (fgets(buffer, 512, file) != NULL){ 
  	switch(buffer[0]) {
      case '#': //commentaire
		break;  
    case 's':
		sscanf(&buffer[2],"%s",nomServeur);
		break;
    case 'l':
		sscanf(&buffer[2],"%s",nomImprimante);
		imp.nom = (char *) malloc(sizeof(char)*(strlen(nomImprimante)+1));
		strcpy(imp.nom,nomImprimante);
		imp.type = IMPRIMANTE_LOCALE;
		if(nbImprimantes != 0)
	  		imprimantes = (Imprimante *) realloc(imprimantes, sizeof(Imprimante)*(nbImprimantes+1));
		imprimantes[nbImprimantes++] = imp;
		break;
    case 'd':
		sscanf(&buffer[2],"%s",nomImprimante);
		imp.nom = (char *) malloc(sizeof(char)*(strlen(nomImprimante)+1));
		strcpy(imp.nom,nomImprimante);
		imp.type = IMPRIMANTE_DISTANTE;
		if(nbImprimantes != 0)
	 		imprimantes = (Imprimante *) realloc(imprimantes, sizeof(Imprimante)*(nbImprimantes+1));
		imprimantes[nbImprimantes++] = imp;
		break;
	case 'f':
		sscanf(&buffer[2],"%d",&nbFilters);
		break;
	case 'm':
		sscanf(&buffer[2],"%s",nomMachine);
		if(nbMachines != 0)
			machines = (char **) realloc(machines,sizeof(char *)*(nbMachines+1));
		machines[nbMachines] = (char *) malloc(sizeof(char)*(strlen(nomMachine)+1));
		strcpy(machines[nbMachines++],nomMachine);
		break;
    }
  } 
  printf("[ Main ] lecture du fichier de configuration OK\n");
}

int main(int argc,char *argv[]){
	pthread_t idScheduler;
	int numServeur,etat;
	Infos_serveur infos;
	int i;
	printf("[ Main ] Initialisation du serveur d'impression\n");
	lectureConfiguration(argv[1]);
	numServeur = init_serveur(nomServeur);
	infos.numServeur = numServeur;
	strcpy(infos.nom,nomServeur);
	init_file_attente();
	demarrer_scheduler(&idScheduler,infos);
	demarrer_filters();
	demarrer_imprimantes();
	printf("[ Main ] Initialisation serveur OK\n");
	if((etat = pthread_join(idScheduler,NULL)) != 0)
		fprintf(stderr, "[ Main ] Erreur fermeture Scheduler\n");
	for(i = 0; i < nbFilters; i++){
		if ((etat = pthread_join(idThdFil[i], NULL)) != 0)
			fprintf(stderr, "[ Main ] Erreur fermeture Filters\n");
	}
	for(i = 0; i < nbImprimantes; i++){
		if ((etat = pthread_join(idThdImp[i], NULL)) != 0)
			fprintf(stderr, "[ Main ] Erreur fermeture Imprimantes\n");
	}
	exit(0);
}
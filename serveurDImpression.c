#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>

#include "communication/communication.h"
#include "impression.h"
#include "cups_filters.h"

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
int nb_jobs_disponibles;
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
int authentifier_machine(char nomMachine[]){
	int i;
	for(i = 0; i < nbMachines; i++){
		if(strcmp(machines[i],nomMachine) == 0)
			break;
	}
	return i < nbMachines;
}
//retourne le numero de la file d'attente de l'imprimante dont le nom est en parametre
int get_numero_imprimante(char * nom){
	int i;
	for(i = 0; i < nbImprimantes; i++){
		if(strcmp(imprimantes[i].nom,nom) == 0)
			break;
	}
	if(i < nbImprimantes)
		return i;
	return -1;
}
//place le job j dans la file d'attente des jobs, pour etre traite par un filter
void placer_job(Job j){
	pthread_mutex_lock(&mutex);
	while(nb_jobs_disponibles == TAILLE_FILE_SCH)
		pthread_cond_wait(&place_disponible,&mutex);
	jobs[indice_depot] = j;
	indice_depot = (indice_depot+1)%TAILLE_FILE_SCH;
	nb_jobs_disponibles++;
	if(nb_jobs_disponibles == 1)
		pthread_cond_signal(&job_disponible);
	pthread_mutex_unlock(&mutex);
}
//fonction qui permet a un filter de recuperer un job dans la file d'attente
Job recuperer_job(){
	Job j;
	pthread_mutex_lock(&mutex);
	while(nb_jobs_disponibles == 0)
		pthread_cond_wait(&job_disponible,&mutex);
	j = jobs[indice_retrait];
	indice_retrait = (indice_retrait+1)%TAILLE_FILE_SCH;
	nb_jobs_disponibles--;
	if(nb_jobs_disponibles == TAILLE_FILE_SCH-1)
		pthread_cond_signal(&place_disponible);
	pthread_mutex_unlock(&mutex);
	return j;
}
//fonction qui permet a un filter de deposer un job filtre sur la file d'attente d'une imprimante
void placer_job_after_filter(Job j,int numero_file){
	int i;
	pthread_mutex_lock(&file_imprimantes[numero_file].mutex);
	while(file_imprimantes[numero_file].nb_cases_remplies == TAILLE_FILE_IMP)
    	pthread_cond_wait(&file_imprimantes[numero_file].place_disponible,
      					&file_imprimantes[numero_file].mutex);
    i = file_imprimantes[numero_file].indice_depot;
    file_imprimantes[numero_file].file[i] = j;
    i = (i+1)%TAILLE_FILE_IMP;
    file_imprimantes[numero_file].indice_depot = i;
    file_imprimantes[numero_file].nb_cases_remplies++;
    if(file_imprimantes[numero_file].nb_cases_remplies == 1)
    	pthread_cond_signal(&file_imprimantes[numero_file].fichier_disponible);
    pthread_mutex_unlock(&file_imprimantes[numero_file].mutex);
}
//fonction qui permet a une imprimante de recuperer un travail d'impression sur sa file d'attente
Job recuperer_job_after_filter(int numero_file){
	Job j;
	int i;
	pthread_mutex_lock(&file_imprimantes[numero_file].mutex);
	while(file_imprimantes[numero_file].nb_cases_remplies == 0)
    	pthread_cond_wait(&file_imprimantes[numero_file].fichier_disponible,
      					&file_imprimantes[numero_file].mutex);
    i = file_imprimantes[numero_file].indice_retrait;
    j = file_imprimantes[numero_file].file[i];
    i = (i+1)%TAILLE_FILE_IMP;
    file_imprimantes[numero_file].indice_retrait = i;
    file_imprimantes[numero_file].nb_cases_remplies--;
    if(file_imprimantes[numero_file].nb_cases_remplies == TAILLE_FILE_IMP-1)
    	pthread_cond_signal(&file_imprimantes[numero_file].place_disponible);
    pthread_mutex_unlock(&file_imprimantes[numero_file].mutex);
    return j;
}
//fonction qu'utilise le scheduler apres reception d'une demande d'impression
void traiter_impression(Demande requete,int numCommunication){
	Infos_demande infos;
	Job job;
	infos = requete.infos;
	job.nb_copies = infos.nb_copies;
	job.type_impression = infos.type_impression;
	job.id_demande = requete.id_demande;
	strcpy(job.nom_imprimante,requete.nom_imprimante);
	strcpy(job.nom_fichier,infos.nom_fichier);
	placer_job(job);
}
//fonction qu'utilise le scheduler apres reception d'une requete sur l'etat d'une imprimante
void etat_imprimante(Demande requete,int numCommunication){
	int numero,nb_elements;
	numero = get_numero_imprimante(requete.nom_imprimante);
	nb_elements = file_imprimantes[numero].nb_cases_remplies;
	//ecriture de la reponse
}

//fonction qu'utilise le scheduler apres reception d'une requete sur l'etat d'une impression
void etat_impression(void){

}
//fonction qu'utilise le scheduler apres reception d'une demande d'annulation d'une impression
void annuler_impression(void){

}
//fonction de traitement des requetes d'un client
void traiter_requete(Demande requete,int numCommunication){
	if(authentifier_machine(requete.machine) == 0)
		return;
	switch(requete.type){
		case IMPRESSION:
			traiter_impression(requete,numCommunication);
			break;
		case ETAT_IMPRIMANTE:
			etat_imprimante(requete,numCommunication);
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
	   	printf("[ Scheduler ] nouvelle demande \n");
	   	traiter_requete(requete,numCommunication);
	   	cloreCommunication(numCommunication);
	}
	pthread_exit(NULL);
}
//fonction qu'utilise un filter pour convertir un fichier 
void traiter_job(Job *job,int numero_file){
	char ext[5];
	char nom_sortie[50];
	sprintf(nom_sortie,"%d%s%d",job->id_demande,job->nom_fichier,numero_file);
    strcpy(ext,extension(job->nom_fichier));
	if(strcmp(ext,"pdf") == 0)
		transformer_fichier_pdf(job->nom_fichier,nom_sortie);
	else if(strcmp(ext,"txt") == 0)
		transformer_fichier_text(job->nom_fichier,nom_sortie);
	else transformer_fichier_image(job->nom_fichier,nom_sortie);
	strcpy(job->nom_fichier,nom_sortie);
}

//fonction d'un cups filter
void * cups_filter(void *args){
	Job j;
	int numero_file,num_filter;
	num_filter = *(int *)args;
   	printf("[ Filter %d ] demarrage OK\n",num_filter);
   	for(;;){
   		j = recuperer_job();
   		printf("[ Filter %d ] traitement Job\n",num_filter);
   		numero_file = get_numero_imprimante(j.nom_imprimante);
   		if(numero_file != -1){
   			traiter_job(&j,numero_file);
   			placer_job_after_filter(j,numero_file);
   		}
   	}
   	pthread_exit(NULL);
}

//fonction d'un imprimante locale
void * imprimante_locale(void *args){
	char fichier_imprimante[50];
	char buffer[256];
	int taille,numero_file;
	int inputFile,outputFile;
	Job j;
	numero_file = *(int *) args;
	sprintf(fichier_imprimante,"file_locale_printer%d",numero_file);
 	printf("[ Imprimante locale %s ] demarrage OK\n",imprimantes[numero_file].nom);
 	outputFile = open(fichier_imprimante,O_WRONLY|O_CREAT|O_APPEND,S_IRWXU);
 	for(;;){
 		j = recuperer_job_after_filter(numero_file);
 		printf("[ Imprimante locale %s ] nouvelle impression\n",imprimantes[numero_file].nom);
 		file_imprimantes[numero_file].encours=1;
 		file_imprimantes[numero_file].id_demande = j.id_demande;
 		inputFile = open(j.nom_fichier,O_RDONLY);
 		while((taille = read(inputFile,buffer,256)) > 0 && file_imprimantes[numero_file].encours==1)
 			write(outputFile,buffer,taille);
 		close(inputFile);
 		//suppression du fichier intermediaire
 		unlink(j.nom_fichier);
 	}
 	close(outputFile);
 	pthread_exit(NULL);
}

//fonction backend d'une imprimante distante
void * backend(void *args){
	char buffer[256];
	int taille,numero_file;
	int inputFile,numCommunication;
	Job j;
	numero_file = *(int *) args;
	printf("[ Backend ] demarrage OK\n");
	for(;;){
		j = recuperer_job_after_filter(numero_file);
		printf("[ Backend ] nouvelle impression\n");
 		file_imprimantes[numero_file].encours=1;
 		file_imprimantes[numero_file].id_demande = j.id_demande;
 		inputFile = open(j.nom_fichier,O_RDONLY);
 		numCommunication = demanderCommunication(j.nom_imprimante);
 		while((taille = read(inputFile,buffer,256)) > 0 && file_imprimantes[numero_file].encours==1)
 			envoyerOctets(numCommunication,buffer, taille);
 		close(inputFile);
 		cloreCommunication(numCommunication);
	}
	pthread_exit(NULL);
}

//fonctions qui demarre les threads des imprimantes locales et les backends des distantes
void demarrer_imprimantes(void){
	int i,etat;
	int * tab;
	Imprimante p;
	printf("[ Main ] demarrage Imprimantes locales et Backends\n");
	tab = malloc(sizeof(int) * nbImprimantes);
	for(i=0; i < nbImprimantes; i++){
		p = imprimantes[i];
		tab[i] = i;
		if(p.type == IMPRIMANTE_LOCALE){
			if((etat=pthread_create(&idThdImp[i], NULL, imprimante_locale,&tab[i])) != 0){
				fprintf(stderr, "[ Main ] Echec demarrage Imprimante locale\n");
				exit(1);
			}
		}else {
			if((etat=pthread_create(&idThdImp[i], NULL, backend,&tab[i])) != 0){
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
	int *tab;
	printf("[ Main ] demarrage Filters\n");
	tab = malloc(sizeof(int) * nbFilters);
	for(i = 0; i < nbFilters; i++){
		tab[i] = i;
		if((etat=pthread_create(&idThdFil[i], NULL, cups_filter,&tab[i])) != 0){
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
	if(argc != 2){
		fprintf(stderr, "Usage %s fichier_configuration\n",argv[0]);
		exit(1);
	}
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
	arreterServeur(numServeur);
	exit(0);
}
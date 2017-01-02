#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>

#include "communication/communication.h"
#include "impression.h"

#define SERVEUR "server"
#define NOM_IMP "imp1"
#define ID 1
#define NOM_MACHINE "ma1"
#define TYPE 1
#define NOM_FICHIER "file.txt"

char *imprimantes_dispo[] = {"imp1", "imp2", "imp3", "imp5", "imp6"};
int nb_imprimante = 6;

int machine_site(Demande demande, char * nom_serveur) 
{
	int numCommunication;
	int lgChemin;
	// se connecter au serveur
	if ((numCommunication = demanderCommunication(nom_serveur)) < 0)
		{
		fprintf(stderr, "Machine site : erreur %s\n", messageErreur((RetourCommunication)numCommunication));
		return 1;
		}

	// envoyer le path du fichier demandé
	lgChemin = sizeof(demande);
    if (envoyerOctets(numCommunication, &demande, lgChemin) != lgChemin)
	{
		fprintf(stderr, "Machine site: erreur envoi path %s\n", nom_serveur);
		return 2;
	}
    // clore la communication
	cloreCommunication(numCommunication);

	return 0;
}

void  demande_impression(char *nom_machine, char *nom_fichier, char *nom_imprimante, int identifiant ) {
  int i;
  srand(time(NULL));
  Demande demande;
  Infos_demande infos_demande;
  
  infos_demande.nb_copies = rand() %4 +1;
  strcpy(infos_demande.nom_fichier, nom_fichier);
  infos_demande.type_impression = RECTO;
  demande.type = IMPRESSION;
  demande.infos = infos_demande;
  strcpy(demande.machine, nom_machine);
  strcpy(demande.nom_imprimante, nom_imprimante);
  demande.id_demande = identifiant;
  demande.type = IMPRESSION;
  demande.infos = infos_demande;
  for(i=0; i< 10; i++)
  	machine_site(demande, SERVEUR);
}


void annuler_impression(char * nom_imprimante, int identifiant) {
  Demande demande;
  
  demande.id_demande = identifiant;
  strcpy(demande.nom_imprimante,nom_imprimante);
  demande.type = ANNULER;
  
  machine_site(demande, SERVEUR);
}

void etat_imprimante(char * nom_imprimante, int identifiant) {
  Demande demande; 
  demande.id_demande = identifiant;
  strcpy(demande.nom_imprimante,nom_imprimante);
  demande.type = ETAT_IMPRIMANTE;
  machine_site(demande, SERVEUR);
}

void etat_impression( char * nom_imprimante, int identifiant) {
  Demande demande; 
  
  demande.id_demande = identifiant;
  strcpy(demande.nom_imprimante,nom_imprimante);
  demande.type = ETAT_IMPRESSION;
  
  machine_site(demande, SERVEUR);
}


void * envoyer_demande(void *args) {
  srand(time(NULL));
  int i;
  for(i = 0; i< 20; i++ ) {
    int numero_imp = rand() % 6;
    demande_impression(imprimantes_dispo[numero_imp], NOM_FICHIER, NOM_IMP, ID);
  }
  
 /* switch(TYPE) {
    case IMPRESSION:
      demande_impression(NOM_MACHINE, NOM_FICHIER, NOM_IMP, ID);
      break;
      
    case ANNULER:
      annuler_impression(NOM_IMP, ID);
      break;
      
    case ETAT_IMPRIMANTE:
      etat_imprimante(NOM_IMP, ID);
      break;
      
    case ETAT_IMPRESSION:
      etat_impression(NOM_IMP, ID);
      break;
  }*/
  pthread_exit(NULL);
}
 
int main(int argc, char *argv[])
{
  pthread_t th;
  if(pthread_create(&th, NULL, envoyer_demande, NULL) != 0) {
    fprintf(stderr, "[ Client ] Demande non envoyee\n");
    exit(1);
  }

  if ((pthread_join(th, NULL)) != 0)
			fprintf(stderr, "[ Main ] Erreur fermeture Imprimantes\n");
  return 0;
}





















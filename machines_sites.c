#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "communication/communication.h"
#include "impression.h"

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

int main(int argc, char *argv[])
{
  int ret;
  Demande demande;
  Infos_demande infos_demande;
  
  infos_demande.nb_copies = 1;
  strcpy(infos_demande.nom_fichier,"file.txt");
  infos_demande.type_impression = RECTO;
  strcpy(demande.machine,"ma1");
  strcpy(demande.nom_imprimante,"imp1");
  demande.id_demande = 1;
  demande.type = IMPRESSION;
  demande.infos = infos_demande;

  /*if (argc != 3)
  	  {
	  fprintf(stderr, "usage: %s nom_serveur chemin_fichier", argv[0]);
	  exit(2);
  	  }*/
  ret = machine_site(demande, "server");
  exit(ret);
}

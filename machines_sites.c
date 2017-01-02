#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "communication/communication.h"
#include "impression.h"

int machine_site(Demande demande, Infos_serveur infos_serveur) 
{
	int numCommunication;
	char tampon[BUFSIZ];
	int lgChemin;
	int nbEnvoyes;

	// se connecter au serveur
	if ((numCommunication = demanderCommunication(infos_serveur.nom)) < 0)
		{
		fprintf(stderr, "Machine site : erreur %s\n", messageErreur((RetourCommunication)numCommunication));
		return 1;
		}

	// envoyer le path du fichier demandé
	lgChemin = strlen(demande.inf_demande.nom_fichier)+1;
    if (envoyerOctets(numCommunication, demande.inf_demande.nom_fichier, lgChemin) != lgChemin)
	{
		fprintf(stderr, "Machine site: erreur envoi path %s\n", demande.inf_demande.nom_fichier);
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
  Infos_serveur infos_serveur;
  if (argc != 3)
  	  {
	  fprintf(stderr, "usage: %s nom_serveur chemin_fichier", argv[0]);
	  exit(2);
  	  }
 // ret = machine_site(demande, infos_serveur);
  exit(ret);
}

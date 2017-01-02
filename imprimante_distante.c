#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "./communication/communication.h"

void 
void imprimante_distante(const char *imp_dist, char *nom_fichier)
{
  int numServeur, numCommunication;
  char tampon[BUFSIZ];
  int nbRecus;
  int fd;
  

  unlink(imp_dist);

  // initialiser le serveur pour qu'il recoive les requetes
   if ((numServeur = initialiserServeur(imp_dist)) < 0)
  	  {
	  fprintf(stderr, "Erreur initialiserServeur: %s: %s\n",
			  imp_dist, messageErreur(numServeur));
	  exit(1);
  	  }
  	  
    if(open(nom_fichier, O_WRONLY|O_CREAT,S_IRWXU) == -1);
      fprintf(stderr, "Erreur ouverture du fichier %s\n", nom_fichier);
      exit((2));
    }
  
  for (;;)
	 {
	 // accepter une connection
	 if ((numCommunication = accepterCommunication(numServeur)) < 0)
	 {
		 fprintf(stderr, "Erreur accepterCommunication: %s: %s\n",
				 imp_dist, messageErreur(numCommunication));
	 	 exit(1);
	   	 }

	 // lire le nom du fichier à transferer
	 if ((nbRecus = recevoirOctets(numCommunication, tampon, BUFSIZ)) > 0)
		 {
		 int inputFile;
		 if ((inputFile = open(tampon, O_RDONLY)) != -1)
			{
			// envoyer le cntenu du fichier demande
			int nbLus;
		 	while ((nbLus = read(inputFile, tampon, BUFSIZ)) > 0)
		 	{
				
		 		if(write(fd, tampon, nbLus) != nbLus)
				  break;
		 	}
		 	close(inputFile);
			}
		 }

	 // clore la communication
	 cloreCommunication(numCommunication);
	 }
	 close(fd);
}

int main(int argc, char *argv[]) {
   int ret;
  if (argc != 2)
  	  {
	  fprintf(stderr, "usage: %s nom_serveur\n", argv[0]);
	  exit(2);
  	  }
  imprimante_distante(argv[1], argv[2]);
  exit(ret);
  return 0;
}
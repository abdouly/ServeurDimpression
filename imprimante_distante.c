#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "communication/communication.h"
 
void imprimante_distante(const char *imp_dist)
{
	int numServeur, numCommunication;
  	char tampon[BUFSIZ];
  	char nom_fichier[50];
  	int nbRecus;
  	int fd;
  	printf("[ Imprimante distante %s ] demarrage \n",imp_dist);
  	unlink(imp_dist);
  	sprintf(nom_fichier,"file_remote_printer_%s",imp_dist);
   	if ((numServeur = initialiserServeur(imp_dist)) < 0){
		fprintf(stderr, "Erreur initialiserServeur: %s: %s\n",
			  imp_dist, messageErreur(numServeur));
	  	exit(1);
   	} 	  
   	if((fd=open(nom_fichier, O_WRONLY|O_CREAT|O_APPEND,S_IRWXU)) == -1){
    	fprintf(stderr, "Erreur ouverture du fichier %s\n", nom_fichier);
      	exit((2));
   	}
   	printf("[ Imprimante distante %s ] demarrage OK \n",imp_dist);
    for(;;){
	 	if ((numCommunication = accepterCommunication(numServeur)) < 0){
		 	fprintf(stderr, "Erreur accepterCommunication: %s: %s\n",
				 imp_dist, messageErreur(numCommunication));
	 	 	exit(1);
	   	}
	   	printf("[ Imprimante distante %s ] nouvelle impression\n",imp_dist);
	   	while ((nbRecus = recevoirOctets(numCommunication, tampon, BUFSIZ)) > 0)
			write(fd, tampon, nbRecus);
		cloreCommunication(numCommunication);
	}
	close(fd);
	arreterServeur(numServeur);
}

int main(int argc, char *argv[]) {
	if(argc != 2){
		fprintf(stderr, "Usage %s nom_imprimante\n",argv[0]);
		exit(1);
	}
  	imprimante_distante(argv[1]);
  	return 0;
}
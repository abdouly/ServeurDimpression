#include <stdio.h>
#include <string.h>


typedef struct imprimante{
  char *nom;
  int type;       //contient le type de l'imprimante. 0 : locale, 1 : distance
}Imprimante;

void lectureConfiguration(char *fileName, char *serverName, Imprimante *imprimantes) {
  FILE *file;
  char buffer[512];
  char nomImprimante[256];
  int taille = 0;
  int nbImprimantes = 0;
  Imprimante imp;
  imprimantes = (Imprimante *) malloc(sizeof(Imprimante));
  
  file = open(fileName, "r");
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
	  imprimantes = (Imprimante *) realloc(imprimantes, sizeof(Imprimante)*nbImprimantes);
	imprimantes[nbImprimantes++] = imp;
	break;
	
      case 'd':
	sscanf(&buffer[2],"%s",nomImprimante);
	imp.nom = nomImprimante;
	imp.type = 1;
	if(nbImprimantes != 0)
	 imprimantes = (Imprimante *) realloc(imprimantes, sizeof(Imprimante)*nbImprimantes);
	imprimantes[nbImprimantes++] = imp;
	break;
    }
  }
  
}
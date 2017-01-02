#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

char *convertir_manj_min(char *ch) {
  char * chaine = (char *) malloc(sizeof(char)*strlen(ch));
  for(int i = 0; i < strlen(ch); i++) {
    chaine[i] = tolower(ch[i]);
  }
  return chaine;
}


void transformer_fichier_text(char *file_in, char *file_out) {
  int fd_entree, fd_sortie;
  char buffer[256];  
  fd_entree = open(file_in, O_RDONLY);
  if(fd_entree == -1) {
    fprintf(stderr,"File %s not found.\n",file_in);
    exit(-1);
  }
  
  fd_sortie = open(file_out, O_WRONLY|O_CREAT,S_IRWXU);
  if(fd_sortie == -1) {
    close(fd_entree);
    fprintf(stderr,"File %s not found.\n",file_out);
    exit(-1);
  }
  
  int t;
  while((t=read(fd_entree, buffer, 256)) > 0) {
    write(fd_sortie, convertir_manj_min(buffer), t);
  }
  
  close(fd_entree);
  close(fd_sortie);
}

void transformer_fichier_pdf(char *file_in, char *file_out) {
  int fd_entree, fd_sortie;
  char *buffer = (char *) malloc(sizeof(char)*256);  
  char *tmp = (char *) malloc(sizeof(char)*256);  
  fd_entree = open(file_in, O_RDONLY);
    int t;
  if(fd_entree == -1) {
    fprintf(stderr,"File %s not found.\n",file_in);
    exit(-1);
  }
  
  fd_sortie = open(file_out, O_WRONLY|O_CREAT,S_IRWXU);
  if(fd_sortie == -1) {
    close(fd_entree);
    fprintf(stderr,"File %s not found.\n",file_out);
    exit(-1);
  }
  
  while((t=read(fd_entree, buffer, 256)) > 0) {
    write(fd_sortie, buffer, t);
  }
  
  close(fd_entree);
  close(fd_sortie);
}  

void transformer_fichier_image(char *file_in, char *file_out) {
  int fd_entree, fd_sortie;
  char buffer[256];  
  fd_entree = open(file_in, O_RDONLY);
  if(fd_entree == -1) {
    fprintf(stderr,"File %s not found.\n",file_in);
    exit(-1);
  }
  
  fd_sortie = open(file_out, O_WRONLY|O_CREAT,S_IRWXU);
  if(fd_sortie == -1) {
    close(fd_entree);
    fprintf(stderr,"File %s not found.\n",file_out);
    exit(-1);
  }
  
  int t;
  while((t=read(fd_entree, buffer, 256)) > 0) {
    write(fd_sortie, buffer, t);
  }
  
  close(fd_entree);
  close(fd_sortie);
}


int main() {
 transformer_fichier_text("file.txt","sortie");
 transformer_fichier_pdf("tp.pdf", "sortie.pdf");
 transformer_fichier_image("linux.png","img.png");
 transformer_fichier_image("capitole.jpg", "cap.jpg");
}











#ifndef CUPS_FILTER_S
#define CUPS_FILTER_S
/*
 * transformation d'un fichier texte à un fichier
 * qui sera reconnu par l'imprimante
 */
void transformer_fichier_text(char *file_in, char *file_out);

/*
 * transformation d'un fichier pdf à un fichier
 * qui sera reconnu par l'imprimante
 */
void transformer_fichier_pdf(char *file_in, char *file_out);

/*
  transformation d'un fichier image à un fichier
 * qui sera reconnu par l'imprimante
 */
void transformer_fichier_image(char *file_in, char *file_out);
#endif
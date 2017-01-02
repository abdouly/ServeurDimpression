// type imprimante
#define IMPRIMANTE_LOCALE 0
#define IMPRIMANTE_DISTANTE 1
//type de demande
#define IMPRESSION 1
#define ETAT_IMPRIMANTE 2
#define ETAT_IMPRESSION 3
#define ANNULER 4
//recto verso
#define RECTO 1
#define RECTO_VERSO 2
//taille des files d'attentes
#define TAILLE_FILE_IMP 5
#define TAILLE_FILE_SCH 10 

#define NB_MAX_FILTER 10
#define NB_MAX_PRINTERS 10


typedef struct imprimante{
  char *nom;
  int type;       
}Imprimante;

typedef struct infos_serveur
{
	char nom[20];
	int numServeur;
}Infos_serveur;

typedef struct job
{	
	int id_demande;
	char * nom_imprimante;
	char * nom_fichier;
	int nb_copies;
	int type_impression;
}Job;

typedef struct infos_demande
{
	char * nom_fichier;
	int nb_copies;
	int type_impression;
}Infos_demande;

typedef struct demande
{
	char * machine;
	int id_demande;
	int type;
	char * nom_imprimante;
	Infos_demande infos;
}Demande;

typedef struct infos
{
	Job file_imprimante[TAILLE_FILE_IMP];
	int indice_depot;
	int indice_retrait;
	int nb_cases_remplies;
	pthread_cond_t place_disponible;
	pthread_cond_t fichier_disponible;
	pthread_mutex_t mutex;
	int encours;
	int id_demande;
}Infos;

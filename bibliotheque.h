#ifndef BIBLIOTHEQUE_H  /* Permet d'éviter les inclusions multiples d’un header.*/
#define BIBLIOTHEQUE_H

#define _GNU_SOURCE   // pour strcasestr

/* Ce fichier contient les définitions de types, les constantes et les prototypes de fonctions
 Pour pouvoir les inclure dans plusieurs fichiers .c sans redéfinir les variables globales,
on utilise les lignes ci-dessous*/

#include <stdio.h> // permet d'utiliser printf, scanf, fgets, etc.
#include <stdlib.h> // permet d'utiliser exit, atoi, etc.
#include <string.h> // permet d'utiliser strlen, strcpy, strtok, etc.
#include <ctype.h> // permet d'utiliser isalpha, isdigit, etc.
#include <time.h> // permet d'utiliser time, localtime, etc.

/* --- Couleurs défini dans le programme selon les normes de l'ANSI ----------------------------------------- */ 
#define RESET  "\033[0m"
#define BOLD   "\033[1m"
#define ROUGE  "\033[31m"
#define VERT   "\033[32m"
#define JAUNE  "\033[33m"
#define BLEU   "\033[34m"
#define CYAN   "\033[36m"
#define GRIS   "\033[90m"

/* Macro-instructions pour messages formatés */ 
#define OK(msg)   printf(VERT  "  ✔  " RESET msg "\n")
#define ERR(msg)  printf(ROUGE "  ✘  " RESET msg "\n")
#define WARN(msg) printf(JAUNE "  ⚠  " RESET msg "\n")
#define INFO(msg) printf(CYAN  "  ℹ  " RESET msg "\n")

/* Limites de ressources */
#define MAX_LIVRES    100
#define MAX_MEMBRES    50
#define MAX_EMPRUNTS  200
#define MAX_STR       100

/* Fichiers contenant les données */
#define FICHIER_LIVRES   "livres.csv"
#define FICHIER_MEMBRES  "membres.csv"
#define FICHIER_EMPRUNTS "emprunts.csv"

/* STRUCTURES */

typedef struct // Structure représentant un livre dans la bibliothèque
{
    int  id;
    char titre[MAX_STR];
    char auteur[MAX_STR];
    int  annee;
    int  disponible;   /* 1 = disponible, 0 = emprunté */
} 
Livre;
typedef struct // Structure représentant un membre de la bibliothèque
{
    int  id;
    char nom[MAX_STR];
    char prenom[MAX_STR];
    char email[MAX_STR];
} 
Membre;
typedef struct // Structure représentant un emprunt à la bibliothèque
{
    int  id;
    int  id_livre;
    int  id_membre;
    char date_retour[12];  /* JJ/MM/AAAA */
    int  rendu;            /* 1 = rendu */
} 
Emprunt;

/*  Définition des variables globales ( Initialisées dans main.c)  */
extern Livre   livres[MAX_LIVRES]; 
extern Membre  membres[MAX_MEMBRES];
extern Emprunt emprunts[MAX_EMPRUNTS];
extern int nb_livres, nb_membres, nb_emprunts;


/*  UTILITAIRES  */

// Utilitaires d'affichage 
void separate(void); // pour une ligne de séparation
void titre(const char *t); // pour un titre encadré de lignes de séparation
// Utilitaires de gestion des dates 
void date_aujourdhui(char *buf, int taille);  // remplit buf avec la date du jour au format JJ/MM/AAAA
int  date_vers_jours(const char *date); // convertit une date JJ/MM/AAAA en nombre de jours depuis une date de référence (ex: 01/01/0000) pour faciliter les comparaisons
void lire_date(char *buf, int taille); // lit une date au format JJ/MM/AAAA
// Utilitaires de saisie
int  est_en_retard(const Emprunt *e); // retourne 1 si l'emprunt est en retard, 0 sinon
void lire_chaine(char *buf, int taille);   // vide le buffer puis lit une ligne
// Utilitaires de recherches 
int  lire_ligne_csv(char *ligne, char *champs[], int nb_champs); // permet de lire une ligne d'un fichier CSV
int  trouver_livre_par_id(int id); // retourne l'indice du livre avec l'ID donné, ou -1 si non trouvé
int  trouver_membre_par_id(int id); // retourne l'indice du membre avec l'ID donné, ou -1 si non trouvé

/* PROTOTYPES */

//  Fichiers : les fonctions de gestion des fichiers CSV 
void sauvegarder(void); 
void charger(void); 
//Livres : les fonctions de gestion des livres 
void ajouter_livre(void);
void afficher_livres(void);
void rechercher_livre(void);
void supprimer_livre(void);
//   Membres : les fonctions de gestion des membres de la bibliothèque 
void ajouter_membre(void);
void afficher_membres(void);
void supprimer_membre(void);
// Emprunts : les fonctions de gestion des emprunts de la bibliothèque  
void emprunter_livre(void);
void retourner_livre(void);
void afficher_emprunts(void);
void afficher_retards(void);
// Statistiques : la fonction de visualisation des statisties de la bibliothèque 
void afficher_stats(void);

#endif

// main.c — Gestion de Bibliothèque

/* DÉBUT DU PROGRAMME */
#include "bibliotheque.h"

//  Variables globales 
Livre   livres[MAX_LIVRES];
Membre  membres[MAX_MEMBRES];
Emprunt emprunts[MAX_EMPRUNTS];
// Initialisation des variables globales à 0 au premier lancement du programme
int nb_livres = 0, nb_membres = 0, nb_emprunts = 0; // Initialisées à 0 au lancement du programme

/* UTILITAIRES*/


// Retourne la date du jour au format JJ/MM/AAAA dans buf
void date_aujourdhui(char *buf, int taille) {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    snprintf(buf, taille, "%02d/%02d/%04d",
             tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900);
}

// Convertit une date JJ/MM/AAAA en nombre de jours depuis une date de référence pour faciliter les comparaisons
int date_vers_jours(const char *date) {
    int j, m, a;
    if (sscanf(date, "%d/%d/%d", &j, &m, &a) != 3) return 0;
    if (m < 3) { m += 12; a--; }
    return 365*a + a/4 - a/100 + a/400 + (153*m + 8)/5 + j;
}

// Retourne 1 si l'emprunt est en retard, 0 sinon
int est_en_retard(const Emprunt *e) {
    if (e->rendu) return 0;
    char auj[12];
    date_aujourdhui(auj, sizeof(auj));
    return date_vers_jours(auj) > date_vers_jours(e->date_retour);
}

// Lit une ligne depuis stdin dans buf (vide la mémoire tampon au préalable)
void lire_chaine(char *buf, int taille) {
    // vider ce qui reste dans le buffer clavier (ex: le \n du scanf précédent)
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    if (fgets(buf, taille, stdin) == NULL) buf[0] = '\0';
    buf[strcspn(buf, "\n")] = '\0'; // supprimer le \n final
}

// Lit la date au format JJ/MM/AAAA
void lire_date(char *buf, int taille) {
    int j, m, a;
    do {
        if (fgets(buf, taille, stdin) == NULL) break;
        buf[strcspn(buf, "\n")] = '\0';
        if (sscanf(buf, "%d/%d/%d", &j, &m, &a) != 3)
            printf("Format invalide, reessayez (JJ/MM/AAAA) : ");
    } while (sscanf(buf, "%d/%d/%d", &j, &m, &a) != 3);
}

// Affiche une ligne de séparation entre les sections
void sep(void) {
    printf(GRIS "  ================================================\n" RESET);
}

// Affiche un titre encadré de lignes de séparation
void titre(const char *t) {
    sep();
    printf(BOLD BLEU "   %s\n" RESET, t);
    sep();
}

// Trouve un livre à partir de son id, retourne son indice dans le tableau ou -1 si non trouvé
int trouver_livre_par_id(int id) {
    for (int i = 0; i < nb_livres; i++)
        if (livres[i].id == id) return i;
    return -1;
}

// Trouve un membre à partir de son id, retourne son indice dans le tableau ou -1 si non trouvé
int trouver_membre_par_id(int id) {
    for (int i = 0; i < nb_membres; i++)
        if (membres[i].id == id) return i;
    return -1;
}


/*FICHIERS CSV */


// Sauvegarde les données des livres, membres et emprunts dans des fichiers CSV
void sauvegarder(void) {
    FILE *f;

    f = fopen(FICHIER_LIVRES, "w");
    for (int i = 0; i < nb_livres; i++)
        fprintf(f, "%d;%s;%s;%d;%d\n",
                livres[i].id, livres[i].titre, livres[i].auteur,
                livres[i].annee, livres[i].disponible);
    fclose(f);

    f = fopen(FICHIER_MEMBRES, "w");
    for (int i = 0; i < nb_membres; i++)
        fprintf(f, "%d;%s;%s;%s\n",
                membres[i].id, membres[i].nom,
                membres[i].prenom, membres[i].email);
    fclose(f);

    f = fopen(FICHIER_EMPRUNTS, "w");
    for (int i = 0; i < nb_emprunts; i++)
        fprintf(f, "%d;%d;%d;%s;%d\n",
                emprunts[i].id, emprunts[i].id_livre,
                emprunts[i].id_membre, emprunts[i].date_retour,
                emprunts[i].rendu);
    fclose(f);

    printf(GRIS "  Donnees sauvegardees.\n" RESET);
}

// Lit une ligne d'un fichier CSV et remplit le tableau champs avec les pointeurs vers les champs de la ligne, retourne le nombre de champs lus
int lire_ligne_csv(char *ligne, char *champs[], int nb_champs) {
    int n = 0;
    char *tok = strtok(ligne, ";");
    while (tok && n < nb_champs) {
        champs[n++] = tok;
        tok = strtok(NULL, ";");
    }
    return n;
}

// Charge les données des livres, membres et emprunts depuis les fichiers CSV
void charger(void) {
    FILE *f;
    char ligne[256];
    char *c[6]; // tableau de pointeurs vers les champs de la ligne

    f = fopen(FICHIER_LIVRES, "r");
    if (f) {
        while (nb_livres < MAX_LIVRES && fgets(ligne, sizeof(ligne), f)) {
            if (lire_ligne_csv(ligne, c, 5) == 5) {
                livres[nb_livres].id         = atoi(c[0]);
                strncpy(livres[nb_livres].titre,  c[1], MAX_STR);
                strncpy(livres[nb_livres].auteur, c[2], MAX_STR);
                livres[nb_livres].annee      = atoi(c[3]);
                livres[nb_livres].disponible = atoi(c[4]);
                nb_livres++;
            }
        }
        fclose(f);
    }

    f = fopen(FICHIER_MEMBRES, "r");
    if (f) {
        while (nb_membres < MAX_MEMBRES && fgets(ligne, sizeof(ligne), f)) {
            if (lire_ligne_csv(ligne, c, 4) == 4) {
                membres[nb_membres].id = atoi(c[0]);
                strncpy(membres[nb_membres].nom,    c[1], MAX_STR);
                strncpy(membres[nb_membres].prenom, c[2], MAX_STR);
                strncpy(membres[nb_membres].email,  c[3], MAX_STR);
                nb_membres++;
            }
        }
        fclose(f);
    }

    f = fopen(FICHIER_EMPRUNTS, "r");
    if (f) {
        while (nb_emprunts < MAX_EMPRUNTS && fgets(ligne, sizeof(ligne), f)) {
            if (lire_ligne_csv(ligne, c, 5) == 5) {
                emprunts[nb_emprunts].id         = atoi(c[0]);
                emprunts[nb_emprunts].id_livre   = atoi(c[1]);
                emprunts[nb_emprunts].id_membre  = atoi(c[2]);
                strncpy(emprunts[nb_emprunts].date_retour, c[3], 12);
                emprunts[nb_emprunts].rendu      = atoi(c[4]);
                nb_emprunts++;
            }
        }
        fclose(f);
    }

    printf(GRIS "  Chargement : " BOLD "%d" RESET GRIS " livre(s), "
                BOLD "%d" RESET GRIS " membre(s), "
                BOLD "%d" RESET GRIS " emprunt(s).\n" RESET,
           nb_livres, nb_membres, nb_emprunts);
}

/* LIVRES */


// Ajoute un livre à la bibliothèque en demandant les informations nécessaires à l'utilisateur
void ajouter_livre(void) {
    if (nb_livres >= MAX_LIVRES) { ERR("Capacite maximale atteinte."); return; }

    Livre l;
    l.id         = nb_livres + 1;
    l.disponible = 1;

    titre("AJOUT D'UN LIVRE");
    printf("  Titre  : "); lire_chaine(l.titre,  MAX_STR);
    printf("  Auteur : "); lire_chaine(l.auteur, MAX_STR);
    printf("  Annee  : "); scanf("%d", &l.annee);

    livres[nb_livres++] = l;
    printf(VERT "  ✔  Livre ajoute ! " BOLD "(ID = %d)\n" RESET, l.id);
}

// Affiche la liste de tous les livres de la bibliothèque avec leurs informations et leur disponibilité
void afficher_livres(void) {
    if (nb_livres == 0) { INFO("Aucun livre enregistre."); return; }
    printf("\n  " BOLD "%-5s %-30s %-20s %-6s %s\n" RESET,
           "ID", "Titre", "Auteur", "Annee", "Dispo");
    sep();
    for (int i = 0; i < nb_livres; i++)
        printf("  %-5d %-30s %-20s %-6d %s\n",
               livres[i].id, livres[i].titre, livres[i].auteur,
               livres[i].annee,
               livres[i].disponible ? VERT "Oui" RESET : ROUGE "Non" RESET);
    printf(GRIS "\n  %d livre(s) au total.\n" RESET, nb_livres);
}

// Permet de rechercher un livre par son titre ou son auteur et indique leur disponibilité
void rechercher_livre(void) {
    char mot[MAX_STR];
    printf(CYAN "\n  Recherche (titre ou auteur) : " RESET);
    lire_chaine(mot, MAX_STR);

    int trouve = 0;
    for (int i = 0; i < nb_livres; i++) {
        if (strcasestr(livres[i].titre,  mot) ||
            strcasestr(livres[i].auteur, mot)) {
            printf("  " BOLD "ID=%-4d" RESET " | %-30s | %-20s | %s\n",
                   livres[i].id, livres[i].titre, livres[i].auteur,
                   livres[i].disponible ? VERT "Dispo" RESET : ROUGE "Emprunte" RESET);
            trouve++;
        }
    }
    if (!trouve) printf(JAUNE "  Aucun resultat pour \"%s\".\n" RESET, mot);
}

// Supprime un livre de la bibliothèque à partir de son ID, après confirmation de l'utilisateur. 
void supprimer_livre(void) {
    int id;
    printf("\n  ID du livre : "); scanf("%d", &id);
    int idx = trouver_livre_par_id(id);
    if (idx == -1)               { ERR("Livre introuvable.");                        return; }
    if (!livres[idx].disponible) { ERR("Livre emprunte, suppression impossible.");   return; }

    printf(CYAN "  Livre : \"%s\" de %s\n" RESET, livres[idx].titre, livres[idx].auteur);
    printf(JAUNE "  Confirmer la suppression ? (o/n) : " RESET);
    char rep[4]; lire_chaine(rep, sizeof(rep));
    if (rep[0] != 'o' && rep[0] != 'O') { INFO("Suppression annulee."); return; }

    for (int i = idx; i < nb_livres - 1; i++)
        livres[i] = livres[i + 1];
    nb_livres--;
    OK("Livre supprime.");
}
//NB : Un livre emprunté ne peut pas être supprimé.

/* MEMBRES */


// Ajoute un membre à la bibliothèque en demandant les informations nécessaires à l'utilisateur
void ajouter_membre(void) {
    if (nb_membres >= MAX_MEMBRES) { ERR("Capacite maximale atteinte."); return; }

    Membre m;
    m.id = nb_membres + 1;

    titre("AJOUT D'UN MEMBRE");
    printf("  Nom    : "); lire_chaine(m.nom,    MAX_STR);
    printf("  Prenom : "); lire_chaine(m.prenom, MAX_STR);
    printf("  Email  : "); lire_chaine(m.email,  MAX_STR);

    membres[nb_membres++] = m;
    printf(VERT "  ✔  Membre ajoute ! " BOLD "(ID = %d)\n" RESET, m.id);
}

// Affiche la liste de tous les membres de la bibliothèque avec leurs informations
void afficher_membres(void) {
    if (nb_membres == 0) { INFO("Aucun membre enregistre."); return; }
    printf("\n  " BOLD "%-5s %-20s %-20s %s\n" RESET, "ID", "Nom", "Prenom", "Email");
    sep();
    for (int i = 0; i < nb_membres; i++)
        printf("  %-5d %-20s %-20s %s\n",
               membres[i].id, membres[i].nom,
               membres[i].prenom, membres[i].email);
    printf(GRIS "\n  %d membre(s) au total.\n" RESET, nb_membres);
}

// Supprime un membre de la bibliothèque à partir de son ID, après confirmation de l'utilisateur.
void supprimer_membre(void) {
    int id;
    printf("\n  ID du membre : "); scanf("%d", &id);

    for (int i = 0; i < nb_emprunts; i++)
        if (emprunts[i].id_membre == id && !emprunts[i].rendu) {
            ERR("Ce membre a des emprunts en cours."); return;
        }

    int idx = trouver_membre_par_id(id);
    if (idx == -1) { ERR("Membre introuvable."); return; }

    printf(CYAN "  Membre : %s %s\n" RESET, membres[idx].nom, membres[idx].prenom);
    printf(JAUNE "  Confirmer la suppression ? (o/n) : " RESET);
    char rep[4]; lire_chaine(rep, sizeof(rep));
    if (rep[0] != 'o' && rep[0] != 'O') { INFO("Suppression annulee."); return; }

    for (int i = idx; i < nb_membres - 1; i++)
        membres[i] = membres[i + 1];
    nb_membres--;
    OK("Membre supprime.");
}
// NB:  Un membre ne peut pas être supprimé s'il a des emprunts en cours.

/* EMPRUNTS */


// Enregistre un emprunt de livre en demandant les informations nécessaires à l'utilisateur et en vérifiant la disponibilité du livre et l'existence du membre
void emprunter_livre(void) {
    if (nb_emprunts >= MAX_EMPRUNTS) { ERR("Capacite maximale atteinte."); return; }

    int id_livre, id_membre;
    titre("NOUVEL EMPRUNT");
    printf("  ID du livre  : "); scanf("%d", &id_livre);
    printf("  ID du membre : "); scanf("%d", &id_membre);

    int il = trouver_livre_par_id(id_livre);
    int im = trouver_membre_par_id(id_membre);
    if (il == -1)               { ERR("Livre introuvable.");  return; }
    if (im == -1)               { ERR("Membre introuvable."); return; }
    if (!livres[il].disponible) { ERR("Livre deja emprunte."); return; }

    Emprunt e;
    e.id        = nb_emprunts + 1;
    e.id_livre  = id_livre;
    e.id_membre = id_membre;
    e.rendu     = 0;

    printf("  Date de retour prevue (JJ/MM/AAAA) : ");
    lire_date(e.date_retour, sizeof(e.date_retour));

    livres[il].disponible = 0;
    emprunts[nb_emprunts++] = e;
    printf(VERT "  ✔  Emprunt enregistre ! " BOLD "(ID = %d)\n" RESET, e.id);
    printf(GRIS "      \"%s\" -> %s %s\n" RESET,
           livres[il].titre, membres[im].nom, membres[im].prenom);
}

// Enregistre le retour d'un livre en demandant l'ID de l'emprunt, vérifie s'il est en retard
void retourner_livre(void) {
    int id;
    printf("\n  ID de l'emprunt : "); scanf("%d", &id);

    for (int i = 0; i < nb_emprunts; i++) {
        if (emprunts[i].id != id) continue;
        if (emprunts[i].rendu) { INFO("Cet emprunt a deja ete cloture."); return; }

        if (est_en_retard(&emprunts[i]))
            printf(ROUGE "  ⚠  Retour en retard !\n" RESET);

        emprunts[i].rendu = 1;
        int il = trouver_livre_par_id(emprunts[i].id_livre);
        if (il != -1) {
            livres[il].disponible = 1;
            printf(VERT "  ✔  Retour enregistre. \"%s\" a nouveau disponible.\n" RESET,
                   livres[il].titre);
        } else {
            OK("Retour enregistre.");
        }
        return;
    }
    ERR("Emprunt introuvable.");
}

// Affiche la liste de tous les emprunts de la bibliothèque avec leurs informations
void afficher_emprunts(void) {
    if (nb_emprunts == 0) { INFO("Aucun emprunt enregistre."); return; }
    printf("\n  " BOLD "%-5s %-30s %-20s %-12s %-5s\n" RESET,
           "ID", "Titre", "Membre", "Retour", "Rendu");
    sep();
    for (int i = 0; i < nb_emprunts; i++) {
        int il = trouver_livre_par_id(emprunts[i].id_livre);
        int im = trouver_membre_par_id(emprunts[i].id_membre);
        const char *col = est_en_retard(&emprunts[i]) ? ROUGE : RESET;
        printf("  %s%-5d %-30s %-20s %-12s %-5s %s%s\n" RESET,
               col,
               emprunts[i].id,
               il != -1 ? livres[il].titre  : "Supprime",
               im != -1 ? membres[im].nom   : "Supprime",
               emprunts[i].date_retour,
               emprunts[i].rendu ? "Oui" : "Non",
               est_en_retard(&emprunts[i]) ? "[RETARD]" : "",
               RESET);
    }
    printf(GRIS "\n  %d emprunt(s) au total.\n" RESET, nb_emprunts);
}

// Affiche la liste de tous les emprunts en retard
void afficher_retards(void) {
    char auj[12]; date_aujourdhui(auj, sizeof(auj));
    printf(ROUGE BOLD "\n  EMPRUNTS EN RETARD (aujourd'hui : %s)\n" RESET, auj);
    sep();
    int n = 0;
    for (int i = 0; i < nb_emprunts; i++) {
        if (!est_en_retard(&emprunts[i])) continue;
        int il = trouver_livre_par_id(emprunts[i].id_livre);
        int im = trouver_membre_par_id(emprunts[i].id_membre);
        printf(ROUGE "  #%-4d" RESET " | %-30s | %-20s | retour prevu : %s\n",
               emprunts[i].id,
               il != -1 ? livres[il].titre : "?",
               im != -1 ? membres[im].nom  : "?",
               emprunts[i].date_retour);
        n++;
    }
    if (n == 0) OK("Aucun emprunt en retard !");
    else printf(ROUGE "\n  %d emprunt(s) en retard.\n" RESET, n);
}


/* STATISTIQUES */


// Affiche les statisties 
void afficher_stats(void) {
    titre("TABLEAU DE BORD");
    int nb_dispo = 0;
    for (int i = 0; i < nb_livres; i++)
        if (livres[i].disponible) nb_dispo++;

    int nb_en_cours = 0, nb_retards = 0;
    for (int i = 0; i < nb_emprunts; i++) {
        if (!emprunts[i].rendu)          nb_en_cours++;
        if (est_en_retard(&emprunts[i])) nb_retards++;
    }

    printf(BOLD BLEU "  -- Livres --\n" RESET);
    printf("  Total       : " BOLD "%d\n" RESET, nb_livres);
    printf("  Disponibles : " VERT BOLD "%d\n" RESET, nb_dispo);
    printf("  Empruntes   : " JAUNE BOLD "%d\n\n" RESET, nb_livres - nb_dispo);

    printf(BOLD BLEU "  -- Membres --\n" RESET);
    printf("  Total       : " BOLD "%d\n\n" RESET, nb_membres);

    printf(BOLD BLEU "  -- Emprunts --\n" RESET);
    printf("  Total       : " BOLD "%d\n" RESET, nb_emprunts);
    printf("  En cours    : " JAUNE BOLD "%d\n" RESET, nb_en_cours);
    if (nb_retards > 0)
        printf("  En retard   : " ROUGE BOLD "%d  !\n" RESET, nb_retards);
    else
        printf("  En retard   : " VERT BOLD "0  OK\n" RESET);
    printf("\n");
    sep();
}


//  MENUS


// Menu pour la gestion des livres
void menu_livres(void) {
    int c;
    do {
        titre("GESTION DES LIVRES");
        printf("  " BOLD "1." RESET " Ajouter un livre\n");
        printf("  " BOLD "2." RESET " Afficher tous les livres\n");
        printf("  " BOLD "3." RESET " Rechercher un livre\n");
        printf("  " BOLD "4." RESET " Supprimer un livre\n");
        printf("  " BOLD "0." RESET " Retour\n");
        sep();
        printf("  Votre choix : "); scanf("%d", &c);
        switch (c) {
            case 1: ajouter_livre();    break;
            case 2: afficher_livres();  break;
            case 3: rechercher_livre(); break;
            case 4: supprimer_livre();  break;
            case 0: break;
            default: WARN("Choix invalide.");
        }
    } while (c != 0);
}

//Menu pour la gestion des membres
void menu_membres(void) {
    int c;
    do {
        titre("GESTION DES MEMBRES");
        printf("  " BOLD "1." RESET " Ajouter un membre\n");
        printf("  " BOLD "2." RESET " Afficher tous les membres\n");
        printf("  " BOLD "3." RESET " Supprimer un membre\n");
        printf("  " BOLD "0." RESET " Retour\n");
        sep();
        printf("  Votre choix : "); scanf("%d", &c);
        switch (c) {
            case 1: ajouter_membre();   break;
            case 2: afficher_membres(); break;
            case 3: supprimer_membre(); break;
            case 0: break;
            default: WARN("Choix invalide.");
        }
    } while (c != 0);
}

// Menu pour la gestion des emprunts
void menu_emprunts(void) {
    int c;
    do {
        titre("GESTION DES EMPRUNTS");
        printf("  " BOLD "1." RESET " Enregistrer un emprunt\n");
        printf("  " BOLD "2." RESET " Enregistrer un retour\n");
        printf("  " BOLD "3." RESET " Afficher les emprunts\n");
        printf("  " BOLD "4." RESET " Emprunts en retard\n");
        printf("  " BOLD "0." RESET " Retour\n");
        sep();
        printf("  Votre choix : "); scanf("%d", &c);
        switch (c) {
            case 1: emprunter_livre();   break;
            case 2: retourner_livre();   break;
            case 3: afficher_emprunts(); break;
            case 4: afficher_retards();  break;
            case 0: break;
            default: WARN("Choix invalide.");
        }
    } while (c != 0);
}

// Menu principal du programme
int main(void) {
    printf(BOLD BLEU
        "\n  ╔══════════════════════════════════════════╗\n"
        "  ║   Gestion de Bibliotheque  v2.1.         ║\n"
        "  ╚══════════════════════════════════════════╝\n\n"
        RESET);

    charger();

    int choix;
    do {
        titre("MENU PRINCIPAL");
        printf("  " BOLD "1." RESET " Gestion des livres\n");
        printf("  " BOLD "2." RESET " Gestion des membres\n");
        printf("  " BOLD "3." RESET " Gestion des emprunts\n");
        printf("  " BOLD "4." RESET " Tableau de bord\n");
        printf("  " BOLD "5." RESET " Sauvegarder\n");
        printf("  " BOLD "0." RESET " Quitter\n");
        sep();
        printf("  Votre choix : "); scanf("%d", &choix);
        switch (choix) {
            case 1: menu_livres();    break;
            case 2: menu_membres();   break;
            case 3: menu_emprunts();  break;
            case 4: afficher_stats(); break;
            case 5: sauvegarder();    break;
            case 0: sauvegarder(); printf(VERT "\n  Au revoir !\n\n" RESET); break;
            default: WARN("Choix invalide.");
        }
    } while (choix != 0);

    return 0;
}

/* FIN DU PROGRAMME */
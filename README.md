#  G-lib

> il s'agit d'un gestinnaire de bibiotheque qui va servir  enregistrer des membres , des livres , des emprunts mais également des retard .

---

## Organisation du programme

### Découpage en fonctions

Le programme est divisé en cinq  fonctions, regroupées dans `main.c` et déclarées dans `bibliotheque.h`.

**Fonctions utilitaires** — elles assurent les opérations transversales utilisées partout dans le programme : récupérer la date du jour, convertir une date en nombre de jours pour comparer deux dates, détecter si un livre n'a pas été rendu , lire une saisie clavier de façon sûre, et afficher les séparateurs visuels des menus.

**Fonctions de gestion des fichiers** — elles gèrent la persistance des données. Une fonction découpe chaque ligne CSV en champs exploitables. Une fonction de chargement lit les trois fichiers au démarrage et remplit les tableaux en mémoire. Une fonction de sauvegarde écrit l'état courant de ces tableaux dans les fichiers CSV à chaque fermeture ou à la demande.

**Fonctions de gestion des livres** — elles permettent d'ajouter un livre, d'afficher la liste complète, de rechercher par titre ou auteur, et de supprimer un ouvrage.

**Fonctions de gestion des membres** — elles permettent d'inscrire un membre, d'afficher la liste des membres, et de supprimer un membre.

**Fonctions de gestion des emprunts** — elles permettent d'enregistrer un emprunt, de valider un retour, d'afficher tous les emprunts, et d'isoler les emprunts en retard.

### Rôle des fonctions principales

| Fonction | Rôle |
|---|---|
| `date_aujourdhui` | Récupère la date du jour |
| `date_vers_jours` | Convertit une date en nombre de jours pour permettre la comparaison |
| `est_en_retard` | Retourne vrai si un emprunt est non rendu et sa date dépassée |
| `lire_chaine` | Lit une saisie clavier en vidant le buffer au préalable |
| `charger` | Lit les trois fichiers CSV et initialise les tableaux en mémoire |
| `sauvegarder` | Écrit l'état courant des tableaux dans les fichiers CSV |
| `ajouter_livre` | Ajoute un nouveau livre avec un ID auto-incrémenté |
| `rechercher_livre` | Recherche un livre par titre ou auteur, insensible à la casse |
| `supprimer_livre` | Supprime un livre, sous réserve qu'il ne soit pas emprunté |
| `ajouter_membre` | Inscrit un nouveau membre avec un ID auto-incrémenté |
| `supprimer_membre` | Supprime un membre, s'il n'a n'a pas d'emprunts en cours |
| `emprunter_livre` | Enregistre un emprunt et marque le livre comme indisponible |
| `retourner_livre` | Valide un retour et remet le livre disponible |
| `afficher_retards` | Liste uniquement les emprunts dont la date de retour est dépassée |
| `afficher_stats` | Affiche le tableau de bord général de la bibliothèque |

### Fonctionnement général

Au lancement, le programme lit automatiquement les fichiers CSV et reconstruit l'état de la bibliothèque en mémoire. Toutes les données sont stockées dans trois tableaux globaux : un pour les livres, un pour les membres, un pour les emprunts. Chaque tableau a une capacité maximale définie dans `bibliotheque.h`.

La navigation se fait à l'aide des menus imbriqués : un menu principal dirige  vers trois sous-menus (livres, membres, emprunts) ainsi que vers le tableau de bord et la sauvegarde manuelle. À la fermeture, la sauvegarde est déclenchée automatiquement avant de quitter.

Les suppressions sont protégées : il est impossible de supprimer un livre actuellement emprunté, ni un membre ayant des livres  non rendus. La détection des retards repose sur une comparaison de dates convertie en nombre de jours, ce qui permet une soustraction simple sans bibliothèque externe.

---

## Mode d'emploi

### Lancement

Une fois le programme compilé et exécuté, il charge automatiquement les données existantes et affiche le menu principal. La navigation se fait en choisissant le numéro correspondant à l'option souhaitée.

### Gestion des livres

Depuis le sous-menu livres, il est possible d'ajouter un nouveau livre en renseignant son titre, son auteur et son année de publication. La liste complète des livres s'affiche avec leur disponibilité. La recherche accepte un mot-clé partiel sur le titre ou l'auteur. La suppression demande une confirmation d'être effective.

### Gestion des membres

Depuis le sous-menu membres, on peut inscrire un nouveau membre en renseignant son nom, prénom et email. La liste des membres inscrits est consultable à tout moment. La suppression est soumise à confirmation et bloquée si le membre n'a pas rendu des livres .

### Gestion des emprunts

Pour enregistrer un emprunt, il faut saisir l'identifiant du livre et celui du membre, puis indiquer la date de retour prévue . Pour enregistrer un retour, il suffit de saisir l'identifiant de l'emprunt. Le programme signale automatiquement si le retour est effectué en retard. L'option retard liste uniquement les emprunts dont la date est dépassée.

### Sauvegarde

La sauvegarde est automatique à la fermeture du programme. Elle peut être déclenchée manuellement depuis le menu principal via l'option dédiée.

---

## Bilan et difficultés rencontrées

### Ce qui a été réalisé

L'ensemble des fonctionnalités prévues a été implémenté : gestion complète des livres, des membres et des emprunts, avec persistance des données entre les sessions, détection des retards et tableau de bord récapitulatif. L'interface en ligne de commande est colorée et structurée pour faciliter la lecture.

### Difficultés rencontrées

L'une des difficultés que nous avons rencontré est la  **comparaison des dates**. Le format JJ/MM/AAAA ne permet pas une comparaison directe par ordre lexicographique. La solution que nous avons adopté a été de convertir chaque date en un nombre de jours via l'algorithme de Zeller, ce qui permet ensuite  de faire une simple soustraction pour savoir si un emprunt est en retard.

La **gestion du buffer clavier** a également posé problème : les appels successifs à `scanf` et `fgets` peuvent laisser des caractères résiduels dans le buffer, provoquant des saisies ignorées ou des comportements inattendus. Une fonction dédiée (`lire_chaine`) a été créée pour vider systématiquement le buffer avant chaque lecture de chaîne.

Enfin, la **cohérence des données entre les tableaux**a été compliqué à gérer car  lors d'une suppression, il faut vérifier les dépendances dans les autres tableaux avant d'agir, et décaler manuellement les éléments suivants dans le tableau pour combler le vide laissé.

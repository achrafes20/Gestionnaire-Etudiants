#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>

#define MAX_NOM 100
#define MAX_NOTES 8
#define MAX_MATIERES 8
#define TAILLE_HASH 100
#define FICHIER "etudiants.txt"
#define FICHIER_MATIERES "matieres.bin"

typedef struct {
    char libelle[50];
    int coeff;
    float valeur;
} Note;

typedef struct Etudiant {
    char id[10];
    char nom[MAX_NOM];
    char prenom[MAX_NOM];
    int age;
    Note notes[MAX_NOTES];
    struct Etudiant* suivant; 
} Etudiant;

typedef struct {
    char libelle[50];
    int coeff;
} Matiere;

Matiere matieres[MAX_MATIERES];
int nbr_matieres = 0;


Etudiant* tableHash[TAILLE_HASH] = { NULL };

unsigned int hash(char* id) {
    unsigned int hash = 0;
    while (*id) {
        hash = (hash * 31 + *id++) % TAILLE_HASH;
    }
    return hash;
}

void ajouterDansHash(Etudiant* etu) {
    unsigned int index = hash(etu->id);
    etu->suivant = tableHash[index];
    tableHash[index] = etu;
}

void supprimerDeHash(char* id) {
    unsigned int index = hash(id);
    Etudiant* courant = tableHash[index];
    Etudiant* precedent = NULL;

    while (courant) {
        if (strcmp(courant->id, id) == 0) {
            if (precedent == NULL) {
                tableHash[index] = courant->suivant;
            } else {
                precedent->suivant = courant->suivant;
            }

            free(courant);
            printf("L'etudiant avec l'ID %s a ete supprime avec succes.\n", id);
            return;
        }
        precedent = courant;
        courant = courant->suivant;
    }

    printf("Aucun etudiant avec l'ID %s trouve dans la table de hachage.\n", id);
}

Etudiant* rechercherDansHash(char* id) {
    unsigned int index = hash(id);
    Etudiant* courant = tableHash[index];
    while (courant) {
        if (strcmp(courant->id, id) == 0) {
            return courant;
        }
        courant = courant->suivant;
    }
    return NULL;
}

void definirMatieres() {
    do{
    printf("Combien de matieres voulez-vous definir ? (max %d) : ", MAX_MATIERES);
    scanf("%d", &nbr_matieres);
    if (nbr_matieres > MAX_MATIERES || nbr_matieres <= 0) 
        printf("Erreur : le nombre de matieres doit être entre 1 et %d.\n", MAX_MATIERES);
        
    }while(nbr_matieres <= 0 || nbr_matieres >8);
    for (int i = 0; i < nbr_matieres; i++) {
        printf("Entrez le nom de la matiere %d : ", i + 1);
        scanf("%s", matieres[i].libelle);
        printf("Entrez son coefficient : ");
        scanf("%d", &matieres[i].coeff);
    }
    printf("Matieres definies avec succes !\n");
    
}

void sauvegarderMatieres() {
    FILE* fichier = fopen(FICHIER_MATIERES, "wb");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur lors de la sauvegarde des matieres.\n");
        return;
    }
    fwrite(&nbr_matieres, sizeof(int), 1, fichier);
    fwrite(matieres, sizeof(Matiere), nbr_matieres, fichier);
    fclose(fichier);
}

void chargerMatieres() {
    FILE* fichier = fopen(FICHIER_MATIERES, "rb");
    if (fichier == NULL) {
        printf("Aucune matiere sauvegardee.\n");
        nbr_matieres = 0;
        return;
    }
    fread(&nbr_matieres, sizeof(int), 1, fichier);
    fread(matieres, sizeof(Matiere), nbr_matieres, fichier);
    fclose(fichier);
}

void creerNouvelleListe() {
    printf("\n=== Creation d'une nouvelle liste ===\n");
    definirMatieres();
    sauvegarderMatieres();
    FILE* fichier = fopen(FICHIER, "wt"); 
    if (fichier == NULL) {
        fprintf(stderr, "Erreur lors de la creation de la nouvelle liste.\n");
        return;
    }
    fclose(fichier);
    for (int i = 0; i < TAILLE_HASH; i++) {
        Etudiant* courant = tableHash[i];
        while (courant) {
            Etudiant* tmp = courant;
            courant = courant->suivant;
            free(tmp);
        }
        tableHash[i] = NULL;
    }
    printf("Nouvelle liste creee avec succes. Vous pouvez maintenant ajouter des etudiants.\n");
}

void convertirMajuscules(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

void echanger(Etudiant* etudiants[], int i, int j) {
    Etudiant* temp = etudiants[i];
    etudiants[i] = etudiants[j];
    etudiants[j] = temp;
}

int partitionner(Etudiant* etudiants[], int debut, int fin) {
    Etudiant* pivot = etudiants[fin]; 
    int i = debut - 1; 
    for (int j = debut; j < fin; j++) {
        if (strcmp(etudiants[j]->nom, pivot->nom) < 0 || 
            (strcmp(etudiants[j]->nom, pivot->nom) == 0 && strcmp(etudiants[j]->prenom, pivot->prenom) < 0)) {
            i++; 
            echanger(etudiants, i, j);
        }
    }
    echanger(etudiants, i + 1, fin);
    return i + 1; 
}

void trierEtudiants(Etudiant* etudiants[], int debut, int fin) {
    if (debut < fin) {
        int pivot = partitionner(etudiants, debut, fin);
        trierEtudiants(etudiants, debut, pivot - 1); 
        trierEtudiants(etudiants, pivot + 1, fin); 
    }
}

void sauvegarderEtudiants() {
    FILE* fichier = fopen(FICHIER, "w");
    if (fichier==0) {
        fprintf(stderr, "Erreur lors de la sauvegarde des etudiants.\n");
        return;
    }
    Etudiant* etudiants[TAILLE_HASH * 10];  
    int count = 0;
    for (int i = 0; i < TAILLE_HASH; i++) {
        Etudiant* courant = tableHash[i];
        while (courant) {
            etudiants[count++] = courant;
            courant = courant->suivant;
        }
    }
    trierEtudiants(etudiants,0, count-1);
    for (int i = 0; i < count; i++) {
        Etudiant* etu = etudiants[i];
        convertirMajuscules(etu->id);
        convertirMajuscules(etu->nom);
        convertirMajuscules(etu->prenom);
        fprintf(fichier, "%s\t\"%s\"\t\"%s\"\t%d", etu->id, etu->nom, etu->prenom, etu->age);
        for (int j = 0; j < nbr_matieres; j++) {
            fprintf(fichier, "\t%.2f", etu->notes[j].valeur);  
        }
        fprintf(fichier, "\n");
    }

    fclose(fichier);
}

void chargerEtudiants() {
    FILE* fichier = fopen(FICHIER, "r");
    if (fichier==0) {
        printf("Aucune donnee trouvee dans le fichier.\n");
        return;
    }
    while (1) {
        Etudiant* etu = malloc(sizeof(Etudiant));
        if (!etu) {
            fprintf(stderr, "Erreur de memoire.\n");
            fclose(fichier);
            return;
        }
        if (fscanf(fichier, "%s\t\"%[^\"]\"\t\"%[^\"]\"\t%d", etu->id, etu->nom, etu->prenom, &etu->age) != 4) {
            free(etu); 
            break; 
        }
        for (int i = 0; i < nbr_matieres; i++) {
            fscanf(fichier, "\t%f", &etu->notes[i].valeur);
        }
        ajouterDansHash(etu);  
    }
    fclose(fichier);
}

void ajouterEtudiants() {
    if (nbr_matieres == 0) {
        printf("Aucune matiere n'est definie. Veuillez d'abord definir les matieres.\n");
        return;
    }
    int nombreEtudiants;
    printf("Combien d'etudiants voulez-vous ajouter ? ");
    scanf("%d", &nombreEtudiants);
    getchar(); 
    for (int i = 0; i < nombreEtudiants; i++) {
        printf("\nAjout de l'etudiant %d/%d\n", i + 1, nombreEtudiants);
        Etudiant* etu = malloc(sizeof(Etudiant));
        if (etu == NULL) {
            printf("Erreur d'allocation memoire pour l'etudiant.\n");
            return;
        }
        printf("Entrez l'ID de l'etudiant : ");
        scanf("%s", etu->id); 
        convertirMajuscules(etu->id);  
        if (rechercherDansHash(etu->id) != NULL) {
            printf("Erreur : Un etudiant avec cet ID existe deja. Veuillez entrer un autre ID.\n");
            free(etu); 
            i--;
            continue;
        }
        getchar(); 
        printf("Entrez le nom de l'etudiant : ");
        fgets(etu->nom, MAX_NOM, stdin);
        etu->nom[strcspn(etu->nom, "\n")] = '\0'; 
        convertirMajuscules(etu->nom);  
        printf("Entrez le prenom de l'etudiant : ");
        fgets(etu->prenom, MAX_NOM, stdin);
        etu->prenom[strcspn(etu->prenom, "\n")] = '\0';
        convertirMajuscules(etu->prenom);  
        printf("Entrez l'age de l'etudiant : ");
        scanf("%d", &etu->age);
        for (int j = 0; j < nbr_matieres; j++) {
            strcpy(etu->notes[j].libelle, matieres[j].libelle);
            etu->notes[j].coeff = matieres[j].coeff;
            do{
            printf("Entrez la note pour %s (0-20): ", matieres[j].libelle);
            scanf("%f", &etu->notes[j].valeur);
            } while (etu->notes[j].valeur < 0 || etu->notes[j].valeur > 20);
        }
        ajouterDansHash(etu);
        printf("etudiant %d ajoute avec succes.\n", i + 1);
    }
}

void rechercherEtudiant() {
    char critere[100];
    int choix;
    int ageRecherche;
    printf("Selectionnez le critere de recherche :\n");
    printf("1. Par ID\n");
    printf("2. Par nom\n");
    printf("3. Par prenom\n");
    printf("4. Par age\n");
    printf("Choix : ");
    scanf("%d", &choix);
    getchar(); 
    if (choix == 4) {
        printf("Entrez l'age a rechercher : ");
        scanf("%d", &ageRecherche);
    } else {
    printf("Entrez le critere de recherche : ");
    fgets(critere, sizeof(critere), stdin);
    critere[strcspn(critere, "\n")] = '\0'; 
    convertirMajuscules(critere);  
    }
    int count = 0;
    Etudiant* etudiantsTrouves[TAILLE_HASH * 10];  
    for (int i = 0; i < TAILLE_HASH; i++) {
        Etudiant* courant = tableHash[i];
        while (courant) {
            int match = 0;
            switch (choix) {
                case 1:  
                    if (strcmp(courant->id, critere) == 0) {
                        match = 1;
                    }
                    break;
                case 2: 
                    if (strcmp(courant->nom, critere) == 0) {
                        match = 1;
                    }
                    break;
                case 3:  
                    if (strcmp(courant->prenom, critere) == 0) {
                        match = 1;
                    }
                    break;
                case 4: 
                    if (courant->age == ageRecherche) {
                        match = 1;
                    }
                    break;
                default:
                    printf("Critere invalide.\n");
                    return;
            }
            if (match) {
                etudiantsTrouves[count++] = courant;
            }
            courant = courant->suivant;
        }
    }
    if (count > 0) {
        printf("Nombre d'etudiants trouves : %d\n", count);
        for (int i = 0; i < count; i++) {
            Etudiant* etu = etudiantsTrouves[i];
            printf("\netudiant %d:\n", i + 1);
            printf("ID: %s\n", etu->id);
            printf("Nom: %s\n", etu->nom);
            printf("Prenom: %s\n", etu->prenom);
            printf("age: %d\n", etu->age);
            for (int j = 0; j < nbr_matieres; j++) {
                printf("Note en %s: %.2f\n", matieres[j].libelle, etu->notes[j].valeur);
            }
        }
    } else {
        printf("Aucun etudiant trouve pour ce critere.\n");
    }
}

void supprimerEtudiant() {
    char critere[100];
    int choix;
    printf("Selectionnez le critere de recherche pour suppression :\n");
    printf("1. Par ID\n");
    printf("2. Par nom\n");
    printf("3. Par prenom\n");
    printf("Choix : ");
    scanf("%d", &choix);
    getchar();
    printf("Entrez le critere de recherche : ");
    fgets(critere, sizeof(critere), stdin);
    critere[strcspn(critere, "\n")] = '\0'; 
    convertirMajuscules(critere); 
    Etudiant* candidats[TAILLE_HASH * 10];
    int nb_candidats = 0;
    for (int i = 0; i < TAILLE_HASH; i++) {
        Etudiant* courant = tableHash[i];
        while (courant) {
            int match = 0;
            switch (choix) {
                case 1:  
                    if (strcmp(courant->id, critere) == 0) {
                        match = 1;
                    }
                    break;
                case 2: 
                    if (strcmp(courant->nom, critere) == 0) {
                        match = 1;
                    }
                    break;
                case 3:  
                    if (strcmp(courant->prenom, critere) == 0) {
                        match = 1;
                    }
                    break;
                default:
                    printf("Critere invalide.\n");
                    return;
            }
            if (match) {
                candidats[nb_candidats++] = courant;
            }
            courant = courant->suivant;
        }
    }
    if (nb_candidats == 0) {
        printf("Aucun etudiant trouve pour le critere : %s\n", critere);
        return;
    }
    if (nb_candidats == 1) {
        printf("Un seul etudiant trouve :\n");
        printf("ID: %s, Nom: %s, Prenom: %s, age: %d, Moyenne: %.2f\n",
               candidats[0]->id, candidats[0]->nom, candidats[0]->prenom, candidats[0]->age);
        printf("Confirmez-vous la suppression ? (O/N) : ");
        char confirmation;
        scanf(" %c", &confirmation);
        if (confirmation != 'O' && confirmation != 'o') {
            printf("Suppression annulee.\n");
            return;
        }
        supprimerDeHash(candidats[0]->id);
    } else {
        printf("Plusieurs etudiants correspondent au critere. Veuillez choisir par ID :\n");
        for (int i = 0; i < nb_candidats; i++) {
            printf("[%d] ID: %s, Nom: %s, Prenom: %s, age: %d \n",i + 1, candidats[i]->id, candidats[i]->nom, candidats[i]->prenom, candidats[i]->age);
        }
        char id_selectionne[10];
        printf("Entrez l'ID de l'etudiant a supprimer : ");
        fgets(id_selectionne, sizeof(id_selectionne), stdin);
        id_selectionne[strcspn(id_selectionne, "\n")] = '\0'; 
        convertirMajuscules(id_selectionne);
        supprimerDeHash(id_selectionne);
    }
}

void modifierEtudiant() {
    char critere[100];
    int choix;
    printf("Selectionnez le critere de recherche pour modification :\n");
    printf("1. Par ID\n");
    printf("2. Par nom\n");
    printf("3. Par prenom\n");
    printf("Choix : ");
    scanf("%d", &choix);
    getchar(); 
    printf("Entrez le critere de recherche : ");
    fgets(critere, sizeof(critere), stdin);
    critere[strcspn(critere, "\n")] = '\0'; 
    convertirMajuscules(critere); 
    Etudiant* candidats[TAILLE_HASH * 10];
    int nb_candidats = 0;
    for (int i = 0; i < TAILLE_HASH; i++) {
        Etudiant* courant = tableHash[i];
        while (courant) {
            int match = 0;
            switch (choix) {
                case 1: 
                    if (strcmp(courant->id, critere) == 0) {
                        match = 1;
                    }
                    break;
                case 2:  
                    if (strcmp(courant->nom, critere) == 0) {
                        match = 1;
                    }
                    break;
                case 3:  
                    if (strcmp(courant->prenom, critere) == 0) {
                        match = 1;
                    }
                    break;
                default:
                    printf("Critere invalide.\n");
                    return;
            }
            if (match) {
                candidats[nb_candidats++] = courant;
            }
            courant = courant->suivant;
        }
    }
    if (nb_candidats == 0) {
        printf("Aucun etudiant trouve pour le critere : %s\n", critere);
        return;
    }
    Etudiant* etu = NULL;
    if (nb_candidats == 1) {
        etu = candidats[0];
    } else {
        printf("Plusieurs etudiants correspondent au critere. Veuillez choisir par ID :\n");
        for (int i = 0; i < nb_candidats; i++) {
            printf("[%d] ID: %s, Nom: %s, Prenom: %s, age: %d\n",
                   i + 1, candidats[i]->id, candidats[i]->nom, candidats[i]->prenom, candidats[i]->age);
        }
        char id_selectionne[10];
        printf("Entrez l'ID de l'etudiant a modifier : ");
        fgets(id_selectionne, sizeof(id_selectionne), stdin);
        id_selectionne[strcspn(id_selectionne, "\n")] = '\0'; 
        convertirMajuscules(id_selectionne);
        for (int i = 0; i < nb_candidats; i++) {
            if (strcmp(candidats[i]->id, id_selectionne) == 0) {
                etu = candidats[i];
                break;
            }
        }
        if (etu == NULL) {
            printf("Aucun etudiant trouve avec l'ID selectionne.\n");
            return;
        }
    }
    printf("etudiant trouve :\n");
    printf("ID: %s\n", etu->id);
    printf("Nom: %s\n", etu->nom);
    printf("Prenom: %s\n", etu->prenom);
    printf("age: %d\n", etu->age);
    printf("Notes de l'etudiant :\n");
    for (int i = 0; i < nbr_matieres; i++) {
        printf("%s (coeff %d) : %.2f\n", matieres[i].libelle, matieres[i].coeff, etu->notes[i].valeur);
    }
    printf("Que voulez-vous modifier ?\n");
    printf("1. Nom\n");
    printf("2. Prenom\n");
    printf("3. age\n");
    printf("4. Notes\n");
    printf("5. Tout\n");
    printf("Choix : ");
    scanf("%d", &choix);
    getchar(); 
    switch (choix) {
        case 1: 
            printf("Entrez le nouveau nom : ");
            fgets(etu->nom, MAX_NOM, stdin);
            etu->nom[strcspn(etu->nom, "\n")] = '\0';
            convertirMajuscules(etu->nom);
            break;
        case 2: 
            printf("Entrez le nouveau prenom : ");
            fgets(etu->prenom, MAX_NOM, stdin);
            etu->prenom[strcspn(etu->prenom, "\n")] = '\0';
            convertirMajuscules(etu->prenom);
            break;
        case 3: 
            printf("Entrez le nouvel age : ");
            scanf("%d", &etu->age);
            break;
        case 4: 
    printf("Liste des matieres disponibles :\n");
    for (int i = 0; i < nbr_matieres; i++) {
        printf("%d. %s (coeff %d)\n", i + 1, matieres[i].libelle, matieres[i].coeff);
    }
    printf("Entrez le numero de la matiere a modifier ou tapez 't' pour modifier toutes les notes : ");
    char choix_notes[10];
    fgets(choix_notes, sizeof(choix_notes), stdin);
    choix_notes[strcspn(choix_notes, "\n")] = '\0'; 
    if (choix_notes[0] == 't' || choix_notes[0] == 'T') {
        for (int i = 0; i < nbr_matieres; i++) {
            do {
                printf("Entrez la nouvelle note pour %s (coeff %d) (0-20) : ", matieres[i].libelle, matieres[i].coeff);
                scanf("%f", &etu->notes[i].valeur);
            } while (etu->notes[i].valeur < 0 || etu->notes[i].valeur > 20);
        }
    } else {
        int matiere_index = atoi(choix_notes); 
        if (matiere_index < 1 || matiere_index > nbr_matieres) {
            printf("Numero de matiere invalide.\n");
            return;
        }
        do {
            printf("Entrez la nouvelle note pour %s (coeff %d) (0-20) : ", matieres[matiere_index - 1].libelle, matieres[matiere_index - 1].coeff);
            scanf("%f", &etu->notes[matiere_index - 1].valeur);
        } while (etu->notes[matiere_index - 1].valeur < 0 || etu->notes[matiere_index - 1].valeur > 20);
    }
    break;
        case 5: // Modifier tout
            printf("Entrez le nouveau nom : ");
            fgets(etu->nom, MAX_NOM, stdin);
            etu->nom[strcspn(etu->nom, "\n")] = '\0';
            convertirMajuscules(etu->nom);
            printf("Entrez le nouveau prenom : ");
            fgets(etu->prenom, MAX_NOM, stdin);
            etu->prenom[strcspn(etu->prenom, "\n")] = '\0';
            convertirMajuscules(etu->prenom);
            printf("Entrez le nouvel age : ");
            scanf("%d", &etu->age);
            for (int i = 0; i < nbr_matieres; i++) {
                do{
                printf("Entrez la nouvelle note pour %s (coeff %d) (0-20): ", matieres[i].libelle, matieres[i].coeff);
                scanf("%f", &etu->notes[i].valeur);
                } while (etu->notes[i].valeur < 0 || etu->notes[i].valeur > 20);
            }
            break;
        default:
            printf("Choix invalide.\n");
            return;
    }
    printf("Les informations de l'etudiant ont ete mises a jour avec succes.\n");
}

void genererRapport() {
    char critere[100];
    int choix;
    printf("Selectionnez le critere de recherche pour generer le rapport :\n");
    printf("1. Par ID\n");
    printf("2. Par nom\n");
    printf("3. Par prenom\n");
    printf("Choix : ");
    scanf("%d", &choix);
    getchar(); 
    printf("Entrez le critere de recherche : ");
    fgets(critere, sizeof(critere), stdin);
    critere[strcspn(critere, "\n")] = '\0'; 
    convertirMajuscules(critere); 
    Etudiant* candidats[TAILLE_HASH * 10];
    int nb_candidats = 0;
    for (int i = 0; i < TAILLE_HASH; i++) {
        Etudiant* courant = tableHash[i];
        while (courant) {
            int match = 0;
            switch (choix) {
                case 1:  
                    if (strcmp(courant->id, critere) == 0) {
                        match = 1;
                    }
                    break;
                case 2:  
                    if (strcmp(courant->nom, critere) == 0) {
                        match = 1;
                    }
                    break;
                case 3:  
                    if (strcmp(courant->prenom, critere) == 0) {
                        match = 1;
                    }
                    break;
                default:
                    printf("Critere invalide.\n");
                    return;
            }
            if (match) {
                candidats[nb_candidats++] = courant;
            }
            courant = courant->suivant;
        }
    }
    if (nb_candidats == 0) {
        printf("Aucun etudiant trouve pour le critere : %s\n", critere);
        return;
    }
    Etudiant* etu = NULL;
    if (nb_candidats == 1) {
        etu = candidats[0];
    } else {
        printf("Plusieurs etudiants correspondent au critere. Veuillez choisir par ID :\n");
        for (int i = 0; i < nb_candidats; i++) {
            printf("[%d] ID: %s, Nom: %s, Prenom: %s, age: %d\n",
                   i + 1, candidats[i]->id, candidats[i]->nom, candidats[i]->prenom, candidats[i]->age);
        }
        char id_selectionne[10];
        printf("Entrez l'ID de l'etudiant pour generer le rapport : ");
        fgets(id_selectionne, sizeof(id_selectionne), stdin);
        id_selectionne[strcspn(id_selectionne, "\n")] = '\0'; 
        convertirMajuscules(id_selectionne);
        for (int i = 0; i < nb_candidats; i++) {
            if (strcmp(candidats[i]->id, id_selectionne) == 0) {
                etu = candidats[i];
                break;
            }
        }
        if (etu == NULL) {
            printf("Aucun etudiant trouve avec l'ID selectionne.\n");
            return;
        }
    }
    printf("\nRapport de l'etudiant :\n");
    printf("ID: %s\n", etu->id);
    printf("Nom: %s\n", etu->nom);
    printf("Prenom: %s\n", etu->prenom);
    printf("age: %d\n", etu->age);
    printf("\nNotes de l'etudiant :\n");
    float somme_notes = 0;
    int total_coeff = 0;
    for (int i = 0; i < nbr_matieres; i++) {
        printf("%s (coeff %d) : %.2f\n", matieres[i].libelle, matieres[i].coeff, etu->notes[i].valeur);
        somme_notes += etu->notes[i].valeur * matieres[i].coeff;
        total_coeff += matieres[i].coeff;
    }
    float moyenne = somme_notes / total_coeff;
    printf("\nMoyenne generale : %.2f\n", moyenne);
    char mention[20];
    if (moyenne >= 16) {
        strcpy(mention, "Tres Bien");
    } else if (moyenne >= 14) {
        strcpy(mention, "Bien");
    } else if (moyenne >= 12) {
        strcpy(mention, "Assez Bien");
    } else if (moyenne >= 10) {
        strcpy(mention, "Passable");
    } else {
        strcpy(mention, "echec");
    }
    printf("Mention : %s\n", mention);
    if (moyenne >= 10) {
        printf("L'etudiant est admis.\n");
    } else {
        printf("L'etudiant n'est pas admis.\n");
    }
}


void enregistrerResumeAcademique() {
    FILE *fichier = fopen("resume_academique.txt", "w");
    if (fichier == NULL) {
        printf("Erreur d'ouverture du fichier!\n");
        return;
    }

    float somme_moyennes = 0;
    int nb_admis = 0;
    int nb_non_admis = 0;
    float min_moyenne = 20.0, max_moyenne = 0.0;  
    Etudiant* etu_min = NULL;  
    Etudiant* etu_max = NULL;  
    int nb_etudiants = 0;  
    Etudiant* etudiants[TAILLE_HASH * 10];

    for (int i = 0; i < TAILLE_HASH; i++) {
        Etudiant* courant = tableHash[i];
        while (courant) {
            etudiants[nb_etudiants++] = courant;
            courant = courant->suivant;
        }
    }

    trierEtudiants(etudiants, 0, nb_etudiants - 1);

    fprintf(fichier, "\nResume academique de la classe :\n");
    fprintf(fichier, "------------------------------------------------------------------------------\n");
    fprintf(fichier, "| %-10s | %-20s | %-20s | %-5s | %-6s |\n", "ID", "Nom", "Prenom", "Age", "Moyenne");
    fprintf(fichier, "------------------------------------------------------------------------------\n");

    for (int i = 0; i < nb_etudiants; i++) {
        Etudiant* etu = etudiants[i];
        float somme_notes = 0;
        int total_coeff = 0;
        for (int j = 0; j < nbr_matieres; j++) {
            somme_notes += etu->notes[j].valeur * matieres[j].coeff;
            total_coeff += matieres[j].coeff;
        }
        float moyenne = somme_notes / total_coeff;
        somme_moyennes += moyenne;
        if (moyenne >= 10) nb_admis++;
        else nb_non_admis++;
        if (moyenne < min_moyenne) {
            min_moyenne = moyenne;
            etu_min = etu;  
        }
        if (moyenne > max_moyenne) {
            max_moyenne = moyenne;
            etu_max = etu;  
        }
        fprintf(fichier, "| %-10s | %-20s | %-20s | %-5d | %-6.2f  |\n", etu->id, etu->nom, etu->prenom, etu->age, moyenne);
    }

    float moyenne_generale_classe = somme_moyennes / nb_etudiants;
    fprintf(fichier, "------------------------------------------------------------------------------\n");
    fprintf(fichier, "Moyenne generale de la classe : %.2f\n", moyenne_generale_classe);
    fprintf(fichier, "Moyenne la plus elevee : %.2f (etudiant : %s %s, ID: %s)\n", max_moyenne, etu_max->nom, etu_max->prenom, etu_max->id);
    fprintf(fichier, "Moyenne la plus basse : %.2f (etudiant : %s %s, ID: %s)\n", min_moyenne, etu_min->nom, etu_min->prenom, etu_min->id);
    fprintf(fichier, "Nombre d'etudiants admis : %d\n", nb_admis);
    fprintf(fichier, "Nombre d'etudiants non admis : %d\n", nb_non_admis);

    fclose(fichier);
    printf("Resume academique enregistre dans 'resume_academique.txt'.\n");
}



void afficherResumeAcademique() {
    float somme_moyennes = 0;
    int nb_admis = 0;
    int nb_non_admis = 0;
    float min_moyenne = 20.0, max_moyenne = 0.0;  
    Etudiant* etu_min = NULL;  
    Etudiant* etu_max = NULL;  
    int nb_etudiants = 0;  
    Etudiant* etudiants[TAILLE_HASH * 10];
    for (int i = 0; i < TAILLE_HASH; i++) {
        Etudiant* courant = tableHash[i];
        while (courant) {
            etudiants[nb_etudiants++] = courant;
            courant = courant->suivant;
        }
    }
    trierEtudiants(etudiants,0, nb_etudiants-1);
    printf("\nResume academique de la classe :\n");
    printf("------------------------------------------------------------------------------\n");
    printf("| %-10s | %-20s | %-20s | %-5s | %-6s |\n", "ID", "Nom", "Prenom", "Age", "Moyenne");
    printf("------------------------------------------------------------------------------\n");
    for (int i = 0; i < nb_etudiants; i++) {
        Etudiant* etu = etudiants[i];
        float somme_notes = 0;
        int total_coeff = 0;
        for (int j = 0; j < nbr_matieres; j++) {
            somme_notes += etu->notes[j].valeur * matieres[j].coeff;
            total_coeff += matieres[j].coeff;
        }
        float moyenne = somme_notes / total_coeff;
        somme_moyennes += moyenne;
        if (moyenne >= 10) nb_admis++;
        else nb_non_admis++;
        if (moyenne < min_moyenne) {
            min_moyenne = moyenne;
            etu_min = etu;  
        }
        if (moyenne > max_moyenne) {
            max_moyenne = moyenne;
            etu_max = etu;  
        }
        printf("| %-10s | %-20s | %-20s | %-5d | %-6.2f  |\n", etu->id, etu->nom, etu->prenom, etu->age, moyenne);
    }
    float moyenne_generale_classe = somme_moyennes / nb_etudiants;
    printf("------------------------------------------------------------------------------\n");
    printf("Moyenne generale de la classe : %.2f\n", moyenne_generale_classe);
    printf("Moyenne la plus elevee : %.2f (etudiant : %s %s, ID: %s)\n", max_moyenne, etu_max->nom, etu_max->prenom, etu_max->id);
    printf("Moyenne la plus basse : %.2f (etudiant : %s %s, ID: %s)\n", min_moyenne, etu_min->nom, etu_min->prenom, etu_min->id);
    printf("Nombre d'etudiants admis : %d\n", nb_admis);
    printf("Nombre d'etudiants non admis : %d\n", nb_non_admis);
    printf("Voulez-vous enregistrer le resume academique dans un fichier texte ? (o/n) : ");
    char choix;
    scanf(" %c", &choix);
    if (choix == 'o' || choix == 'O') {
    enregistrerResumeAcademique();
}

}

int main() {

    int choix;
    chargerMatieres();  
    chargerEtudiants(); 

    do {
        system("cls");
        printf("\n1. Creer une nouvelle liste d'etudiants(l'ancienne liste va etre supprimer !)\n");
        printf("2. Ajouter un etudiant\n");
        printf("3. supprimer un etudiant\n");
        printf("4. modifier un etudiant\n");
        printf("5. Rechercher un etudiant\n");
        printf("6. Generer un rapport pour un etudiant\n");
        printf("7. Afficher un resume academique\n");
        printf("0. Quitter\n");
        
        printf("Choix : ");
        scanf("%d", &choix);

        switch (choix) {
        case 1:
            creerNouvelleListe(); 
            break;
        case 2:
            ajouterEtudiants();
            sauvegarderEtudiants(); 
            break;
        case 3:
            supprimerEtudiant();
            sauvegarderEtudiants();
            break;
        case 4:
            modifierEtudiant();
            sauvegarderEtudiants();
            break;
        case 5:
            rechercherEtudiant(); 
            break;
        case 6:
            genererRapport(); 
            break;
        case 7:
            afficherResumeAcademique(); 
            break;
        case 0:
            sauvegarderEtudiants(); 
            printf("Au revoir !\n");
            break;
        
        default:
            printf("Choix invalide. Essayez de nouveau.\n");
        }
        printf("\ncliquer sur entrer pour continuer\n");
        getch();
    } while (choix != 0); 

    return 0;
}
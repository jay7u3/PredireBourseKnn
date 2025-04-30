#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Arbre.h"

EspaceLatent* lirePointsDepuisCSV(const char* chemin, int* dim_out) {
    FILE* fichier = fopen(chemin, "r");
    if (!fichier) {
        perror("Erreur lors de l'ouverture du fichier CSV");
        return NULL;
    }

    char ligne[10000];
    int nb_lignes = 0;
    int dimension = -1;

    // Compter les lignes et dimensions
    while (fgets(ligne, sizeof(ligne), fichier)) {
        if (dimension == -1) {
            dimension = 1;
            for (char* p = ligne; *p; p++) {
                if (*p == ',') dimension++;
            }
        }
        nb_lignes++;
    }

    rewind(fichier);

    int m = dimension - 1;
    *dim_out = m;
    EspaceLatent* espace = creerEspaceLatent(nb_lignes, m);

    int i = 0;
    while (fgets(ligne, sizeof(ligne), fichier)) {
        float* coords = malloc(m * sizeof(float));
        char* token = strtok(ligne, ",");
        for (int j = 0; j < m; j++) {
            if (!token) {
                fprintf(stderr, "Erreur: ligne %d incomplète (manque coordonnée %d)\n", i, j);
                free(coords);
                goto skip;
            }
            coords[j] = atof(token);
            token = strtok(NULL, ",");
        }

        if (!token) {
            fprintf(stderr, "Erreur: ligne %d incomplète (classe manquante)\n", i);
            free(coords);
            goto skip;
        }

        int classe = atoi(token);
        ajouterDonnees(espace, coords, classe, i);

        free(coords);
        i++;

    skip:
        continue;
    }

    fclose(fichier);
    return espace;
}

int predireClasseKNN(NoeudKD* arbre, float* point, int m, int k) {
    FilePriorite file;
    file.k = k;
    file.taille = 0;
    file.m = m;
    file.voisins = malloc(k * sizeof(Voisin));

    rechercheKNN(arbre, point, &file, m);

    int votes[10] = {0};
    for (int i = 0; i < file.taille; i++) {
        printf("Voisin %d : classe = %d, distance = %.5f\n",
           i, file.voisins[i].classe, file.voisins[i].distance);
        votes[file.voisins[i].classe]++;
    }

    int max_vote = 0, classe_predite = -1;
    for (int i = 0; i < 10; i++) {
        if (votes[i] > max_vote) {
            max_vote = votes[i];
            classe_predite = i;
        }
    }

    free(file.voisins);
    return classe_predite;
}

int main() {
    int dimension = 0;
    EspaceLatent* espace = lirePointsDepuisCSV("points_classes.csv", &dimension);
    if (!espace) return 1;

    NoeudKD* arbre = creerArbreDepuisEspaceLatent(espace);

    float* point = malloc(dimension * sizeof(float));
    char buffer[100000];
    fgets(buffer, sizeof(buffer), stdin);
    char* token = strtok(buffer, ",");
    for (int i = 0; i < dimension && token; i++) {
        point[i] = atof(token);
        token = strtok(NULL, ",");
    }

    int classe = predireClasseKNN(arbre, point, dimension, 5);
    printf("\nClasse prédite : %s\n", classe == 1 ? "Hausse" : "Baisse");

    free(point);
    libererKDTree(arbre);
    libererEspaceLatent(espace);
    return 0;
}
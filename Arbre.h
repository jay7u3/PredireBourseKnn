#ifndef ARBRE_H
#define ARBRE_H

#include <stdio.h>

// ===== Structures de donnees =====
typedef struct EspaceLatent {
    float** donnees;
    int* classes;
    int n;
    int m;
} EspaceLatent;

typedef struct NoeudKD {
    float* point;
    int classe;
    struct NoeudKD* gauche;
    struct NoeudKD* droit;
    int dimension;
} NoeudKD;

typedef struct Voisin {
    float* point;
    int classe;
    double distance;
} Voisin;

typedef struct FilePriorite {
    Voisin* voisins;
    int k;
    int taille;
    int m;
} FilePriorite;

// ===== Fonctions de l'espace latent =====
EspaceLatent* creerEspaceLatent(int n, int m);
void ajouterDonnees(EspaceLatent* espace, float* donnees, int classe, int index);
void afficherEspaceLatent(EspaceLatent* espace);
void libererEspaceLatent(EspaceLatent* espace);

// ===== Fonctions de lecture CSV =====
EspaceLatent* lirePointsDepuisCSV(const char* chemin, int* dim_out);

// ===== Fonctions de l'arbre k-d =====
NoeudKD* creerArbreDepuisEspaceLatent(EspaceLatent* espace);
void afficherKDTreeFormeArbre(NoeudKD* noeud, int m, int profondeur, char* prefixe);
void libererKDTree(NoeudKD* noeud);

// ===== Fonctions KNN =====
double distanceEuclidienne(float* a, float* b, int m);
void ajouterVoisin(FilePriorite* file, float* point, int classe, double distance);
void rechercheKNN(NoeudKD* noeud, float* point, FilePriorite* file, int m);

// ===== Debug utils =====
void printPoint(float* point, int m);

#endif // ARBRE_H

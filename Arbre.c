#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ===== STRUCTURE DE L'ESPACE LATENT =====
typedef struct EspaceLatent {
    float** donnees;
    int* classes;
    int n;
    int m;
} EspaceLatent;

EspaceLatent* creerEspaceLatent(int n, int m) {
    EspaceLatent* espace = malloc(sizeof(EspaceLatent));
    espace->n = n;
    espace->m = m;
    espace->donnees = malloc(n * sizeof(float*));
    espace->classes = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        espace->donnees[i] = malloc(m * sizeof(float));
    }
    return espace;
}

void ajouterDonnees(EspaceLatent* espace, float* donnees, int classe, int index) {
    for (int j = 0; j < espace->m; j++) {
        espace->donnees[index][j] = donnees[j];
    }
    espace->classes[index] = classe;
}

void afficherEspaceLatent(EspaceLatent* espace) {
    for (int i = 0; i < espace->n; i++) {
        for (int j = 0; j < espace->m; j++) {
            printf("%.2f ", espace->donnees[i][j]);
        }
        printf("| Classe: %d\n", espace->classes[i]);
    }
}

void libererEspaceLatent(EspaceLatent* espace) {
    for (int i = 0; i < espace->n; i++) {
        free(espace->donnees[i]);
    }
    free(espace->donnees);
    free(espace->classes);
    free(espace);
}

// ===== STRUCTURE DE L'ARBRE K-D =====
typedef struct NoeudKD {
    float* point;
    int classe;
    struct NoeudKD* gauche;
    struct NoeudKD* droit;
    int dimension;
} NoeudKD;

int dimension_globale = 0;

typedef struct {
    float* point;
    int classe;
} PointAvecClasse;

int comparer_point_classe(const void* a, const void* b) {
    const PointAvecClasse* pa = (const PointAvecClasse*)a;
    const PointAvecClasse* pb = (const PointAvecClasse*)b;
    if (pa->point[dimension_globale] < pb->point[dimension_globale]) return -1;
    if (pa->point[dimension_globale] > pb->point[dimension_globale]) return 1;
    return 0;
}

NoeudKD* construireKDTreeAvecClasseRecurse(PointAvecClasse* data, int n, int m, int profondeur) {
    if (n <= 0) return NULL;

    int dim = profondeur % m;
    dimension_globale = dim;
    qsort(data, n, sizeof(PointAvecClasse), comparer_point_classe);

    int mediane = n / 2;

    NoeudKD* noeud = malloc(sizeof(NoeudKD));
    noeud->point = malloc(m * sizeof(float));
    for (int i = 0; i < m; i++) {
        noeud->point[i] = data[mediane].point[i];
    }
    noeud->classe = data[mediane].classe;
    noeud->dimension = dim;

    noeud->gauche = construireKDTreeAvecClasseRecurse(data, mediane, m, profondeur + 1);
    noeud->droit = construireKDTreeAvecClasseRecurse(data + mediane + 1, n - mediane - 1, m, profondeur + 1);

    return noeud;
}

NoeudKD* construireKDTreeAvecClasse(float** donnees, int* classes, int n, int m, int profondeur) {
    PointAvecClasse* data = malloc(n * sizeof(PointAvecClasse));
    for (int i = 0; i < n; i++) {
        data[i].point = donnees[i];
        data[i].classe = classes[i];
    }
    NoeudKD* racine = construireKDTreeAvecClasseRecurse(data, n, m, profondeur);
    free(data);
    return racine;
}

void afficherKDTreeFormeArbre(NoeudKD* noeud, int m, int profondeur, char* prefixe) {
    if (noeud == NULL) {
        printf("%s[NULL]\n", prefixe);
        return;
    }

    printf("%s(", prefixe);
    for (int i = 0; i < m; i++) {
        printf("%.2f", noeud->point[i]);
        if (i < m - 1) printf(", ");
    }
    printf(") [dim=%d, classe=%d]\n", noeud->dimension, noeud->classe);

    char prefixe_gauche[256];
    char prefixe_droit[256];
    snprintf(prefixe_gauche, sizeof(prefixe_gauche), "%s├── ", prefixe);
    snprintf(prefixe_droit, sizeof(prefixe_droit), "%s└── ", prefixe);

    afficherKDTreeFormeArbre(noeud->gauche, m, profondeur + 1, prefixe_gauche);
    afficherKDTreeFormeArbre(noeud->droit, m, profondeur + 1, prefixe_droit);
}

void libererKDTree(NoeudKD* noeud) {
    if (noeud == NULL) return;
    free(noeud->point);
    libererKDTree(noeud->gauche);
    libererKDTree(noeud->droit);
    free(noeud);
}

NoeudKD* creerArbreDepuisEspaceLatent(EspaceLatent* espace) {
    return construireKDTreeAvecClasse(espace->donnees, espace->classes, espace->n, espace->m, 0);
}

// ===== STRUCTURES POUR KNN =====
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

double distanceEuclidienne(float* a, float* b, int m) {
    double somme = 0.0;
    for (int i = 0; i < m; i++) {
        double diff = a[i] - b[i];
        somme += diff * diff;
    }
    return sqrt(somme);
}

void ajouterVoisin(FilePriorite* file, float* point, int classe, double distance) {
    if (file->taille < file->k || distance < file->voisins[file->taille - 1].distance) {
        int i = file->taille - 1;
        while (i >= 0 && file->voisins[i].distance > distance) {
            if (i + 1 < file->k) file->voisins[i + 1] = file->voisins[i];
            i--;
        }
        if (i + 1 < file->k) {
            file->voisins[i + 1].point = point;
            file->voisins[i + 1].classe = classe;
            file->voisins[i + 1].distance = distance;
        }
        if (file->taille < file->k) file->taille++;
    }
}

void rechercheKNN(NoeudKD* noeud, float* point, FilePriorite* file, int m) {
    if (noeud == NULL) return;

    double dist = distanceEuclidienne(noeud->point, point, m);
    ajouterVoisin(file, noeud->point, noeud->classe, dist);

    int dim = noeud->dimension;
    int aller_gauche = point[dim] <= noeud->point[dim];

    NoeudKD* premier = aller_gauche ? noeud->gauche : noeud->droit;
    NoeudKD* second  = aller_gauche ? noeud->droit : noeud->gauche;

    rechercheKNN(premier, point, file, m);

    double distance_dim = fabs(point[dim] - noeud->point[dim]);
    if (file->taille < file->k || distance_dim < file->voisins[file->taille - 1].distance) {
        rechercheKNN(second, point, file, m);
    }
}
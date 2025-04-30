# 📈 Prédire la Bourse avec KNN

Ce projet implémente un algorithme basé sur la méthode des k-plus proches voisins (k-NN) pour prédire la **tendance d'une action boursière dans les 24 heures à venir**.

---

## 🔍 Principe de l'algorithme

1. **Récupération des données :**  
   On collecte les données de plusieurs actions sur les 7 derniers jours via l’API Yahoo Finance. Les données sont disponibles à une fréquence d’une minute, mais on travaille sur des échelles de temps plus larges.

2. **Préparation des exemples d'entraînement :**  
   Les données sont découpées en segments (ou "graphes") représentant l'évolution d’une action sur une période donnée.  
   Chaque segment est ensuite **étiqueté** selon la tendance observée juste après cet intervalle :
   - 🟢 *Haussier* si le prix monte,
   - 🔴 *Baissier* si le prix baisse.

3. **Représentation dans un espace latent :**  
   Chaque segment est représenté par un point dans un espace à *n dimensions*, où `n` correspond au nombre de points mesurés dans le segment. Cela permet de transformer les courbes de prix en vecteurs exploitables.

4. **Indexation avec un arbre k-d (k-dimensional) :**  
   Pour faciliter la recherche des voisins les plus proches dans l’espace latent, on construit un arbre k-d. Cela rend la classification plus rapide et plus efficace.

5. **Prédiction :**  
   Lorsqu’on souhaite prédire la tendance d’une action actuelle, on transforme son évolution récente en vecteur, puis on le compare aux exemples stockés pour déterminer sa classe (haussier ou baissier) via la majorité de ses `k` plus proches voisins.

---

## 📌 Objectif

Fournir une méthode simple, rapide et interprétable pour anticiper la tendance d'une action à court terme, en s'appuyant uniquement sur des données historiques et l’algorithme KNN.

## 💿 Utilisation

Compiler csv_to_kdtree.c avec Arbre.c puis lancer predict_from_ticker.py

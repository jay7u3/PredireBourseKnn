import subprocess
import pandas as pd
import numpy as np
import yfinance as yf

# === Génération du fichier CSV d'entraînement ===
from generate_stock_points import TICKERS, FENETRE_MINUTES

# === Création du vecteur d'entrée pour la prédiction ===
def get_input_vector_from_ticker(ticker):
    data = yf.download(ticker, interval="1m", period="2d")
    cours = data['Close'].dropna().values

    if len(cours) < FENETRE_MINUTES:
        raise ValueError("Pas assez de données pour le ticker.")

    fenetre = cours[-FENETRE_MINUTES:]
    min_val = np.min(fenetre)
    max_val = np.max(fenetre)
    if max_val - min_val == 0:
        raise ValueError("Pas de variation dans les données.")
    normalise = (fenetre - min_val) / (max_val - min_val)
    point = normalise.astype(float)
    point = point.tolist()
    point = [i[0] for i in point]
    return point

# === Écriture dans stdin pour le binaire C ===
def predire_point(point):
    point_str = ",".join(str(x) for x in point)
    print("\nEnvoi du point au programme C...")
    result = subprocess.run(["./csv_to_kdtree"], input=point_str.encode(), capture_output=True)
    print(result.stdout.decode())

# === Script principal ===
if __name__ == "__main__":
    for ticker in TICKERS:
        print(f"Téléchargement de {ticker}...")
        try:
            point = get_input_vector_from_ticker(ticker)
            print(point)
            predire_point(point)
        except Exception as e:
            print(f"Erreur pour {ticker} : {e}")
            continue

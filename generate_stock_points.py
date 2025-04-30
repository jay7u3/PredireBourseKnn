import yfinance as yf
import numpy as np

# === Paramètres ===
TICKERS = [
    "AAPL", "MSFT", "GOOGL", "AMZN", "META", "TSLA", "NVDA", "NFLX", "INTC", "AMD",
    "BABA", "ORCL", "CSCO", "QCOM", "IBM", "ADBE", "CRM", "PYPL", "AVGO", "TXN",
    "PEP", "KO", "MCD", "WMT", "JNJ", "PFE"
]
INTERVAL = "1m"
JOURS = 7
FENETRE_MINUTES = 290
DECALAGE = 60

CLASSE_HAUSSIER = 1
CLASSE_BAISSIER = 0

# === Collecte de tous les points ===
tous_les_points = []

for ticker in TICKERS:
    print(f"Téléchargement de {ticker}...")
    try:
        data = yf.download(tickers=ticker, interval=INTERVAL, period=f"{JOURS}d")
    except Exception as e:
        print(f"Erreur pour {ticker} : {e}")
        continue

    if data.empty:
        print(f"Aucune donnée pour {ticker}. Ignoré.")
        continue

    cours = data['Close'].dropna().astype(float).values  # tableau de float
    total_fenetres = len(cours) - 2 * FENETRE_MINUTES

    for i in range(0, total_fenetres, DECALAGE):
        fenetre = cours[i:i+FENETRE_MINUTES].tolist()
        fenetre = [i[0] for i in fenetre]  # transforme directement en liste plate
        min_val = min(fenetre)
        max_val = max(fenetre)
        if max_val - min_val == 0:
            continue

        normalise = [(x - min_val) / (max_val - min_val) for x in fenetre]

        lendemain = cours[i+FENETRE_MINUTES:i+2*FENETRE_MINUTES]
        if len(lendemain) < FENETRE_MINUTES:
            continue

        moyenne_lendemain = np.mean(lendemain)
        classe = CLASSE_HAUSSIER if moyenne_lendemain > max_val else CLASSE_BAISSIER

        tous_les_points.append(normalise + [classe])

print(f"Nombre total de points générés : {len(tous_les_points)}")

with open("points_classes.csv", "w") as f:
    for point in tous_les_points:
        ligne = ",".join(f"{x:.10f}" for x in point[:-1])
        ligne += f",{int(point[-1])}\n"
        f.write(ligne)

print("Fichier 'points_classes.csv' créé avec succès (sans crochets).")

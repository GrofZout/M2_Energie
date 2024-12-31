import matplotlib.pyplot as plt
import numpy as np
import argparse

# Parse command-line arguments
parser = argparse.ArgumentParser(description="Générer une fractale et l'enregistrer dans un fichier BMP.")
parser.add_argument("--taille", type=int, default=1000, help="Taille de l'image en pixels (par défaut 10000)")
parser.add_argument("--iterations", type=int, default=1000, help="Nombre maximal d'itérations (par défaut 1000)")
parser.add_argument("--output", type=str, default="fractale.bmp", help="Nom du fichier de sortie (par défaut fractale.bmp)")
args = parser.parse_args()

# Paramètres
xmin, xmax = -1, 1
ymin, ymax = -1, 1
a = -0.8
b = 0.156
taille = args.taille
iterationmax = args.iterations

# Initialisation des pixels
pixels = np.zeros((taille, taille, 3), dtype='uint8')

# Génération de la fractale
for line in range(taille):
    for col in range(taille):
        i = 1
        x = xmin + col * (xmax - xmin) / taille
        y = ymax - line * (ymax - ymin) / taille
        while i <= iterationmax and (x**2 + y**2) <= 4:
            x, y = x**2 - y**2 + a, 2 * x * y + b
            i += 1
        if i > iterationmax and (x**2 + y**2) <= 4:
            pixels[col, line] = (0, 0, 0)
        else:
            pixels[col, line] = ((4 * i) % 256, 2 * i % 256, (6 * i) % 256)

# Sauvegarde de l'image dans un fichier BMP
plt.imsave(args.output, pixels)

print(f"Fractale générée et enregistrée dans le fichier {args.output}")

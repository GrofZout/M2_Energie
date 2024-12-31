#include "libbmp/libbmp.h"
#include <stdio.h>
#include <stdlib.h>

void generateFractal(const char *filename, int taille, int iterationmax) {
    double xmin = -1.0, xmax = 1.0;
    double ymin = -1.0, ymax = 1.0;
    double a = -0.8, b = 0.156;

    // Création de l'image BMP
    bmp_img img;
    bmp_img_init_df(&img, taille, taille);

    // Génération de la fractale
    for (int y = 0; y < taille; y++) {
        for (int x = 0; x < taille; x++) {
            int i = 1;
            double cx = xmin + x * (xmax - xmin) / taille;
            double cy = ymax - y * (ymax - ymin) / taille;
            double zx = cx, zy = cy;

            while (i <= iterationmax && (zx * zx + zy * zy) <= 4) {
                double xtemp = zx * zx - zy * zy + a;
                zy = 2 * zx * zy + b;
                zx = xtemp;
                i++;
            }

            if (i > iterationmax && (zx * zx + zy * zy) <= 4) {
                bmp_pixel_init(&img.img_pixels[y][x], 0, 0, 0); // Pixel noir pour les points convergents
            } else {
                bmp_pixel_init(&img.img_pixels[y][x], (4 * i) % 256, (2 * i) % 256, (6 * i) % 256);
            }
        }
    }

    // Sauvegarde de l'image
    if (bmp_img_write(&img, filename) != BMP_OK) {
        fprintf(stderr, "Erreur lors de la sauvegarde du fichier BMP.\n");
    } else {
        printf("Fractale générée et sauvegardée dans le fichier %s\n", filename);
    }

    bmp_img_free(&img);
}

int main(int argc, char *argv[]) {
    int taille = 10000;       // Par défaut : 1000x1000 pixels
    int iterations = 1000;   // Par défaut : 1000 itérations
    const char *output = "fractale.bmp";

    if (argc > 1) taille = atoi(argv[1]);
    if (argc > 2) iterations = atoi(argv[2]);
    if (argc > 3) output = argv[3];

    generateFractal(output, taille, iterations);
    return 0;
}

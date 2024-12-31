#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "libbmp/libbmp.h"  // Assurez-vous que cette bibliothèque est incluse pour manipuler les fichiers BMP

#define XMIN -1.0
#define XMAX  1.0
#define YMIN -1.0
#define YMAX  1.0
#define A -0.8
#define B  0.156

void generate_fractal(int taille, int iterationmax, const char* output_file) {
    // Allocation mémoire pour l'image
    unsigned char *pixels = malloc(3 * taille * taille * sizeof(unsigned char));
    if (!pixels) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        exit(EXIT_FAILURE);
    }

    double scale_x = (XMAX - XMIN) / taille;
    double scale_y = (YMAX - YMIN) / taille;

    // Génération de la fractale avec OpenMP
    #pragma omp parallel for collapse(2)
    for (int line = 0; line < taille; line++) {
        for (int col = 0; col < taille; col++) {
            int i = 1;
            double x = XMIN + col * scale_x;
            double y = YMAX - line * scale_y;
            double x_temp;

            while (i <= iterationmax && (x * x + y * y) <= 4.0) {
                x_temp = x * x - y * y + A;
                y = 2.0 * x * y + B;
                x = x_temp;
                i++;
            }

            int idx = (line * taille + col) * 3;
            if (i > iterationmax) {
                pixels[idx] = 0;
                pixels[idx + 1] = 0;
                pixels[idx + 2] = 0;
            } else {
                pixels[idx] = (4 * i) % 256;
                pixels[idx + 1] = (2 * i) % 256;
                pixels[idx + 2] = (6 * i) % 256;
            }
        }
    }

    // Sauvegarde dans un fichier BMP
    bmp_img img;
    bmp_img_init_df(&img, taille, taille);

    for (int line = 0; line < taille; line++) {
        for (int col = 0; col < taille; col++) {
            int idx = (line * taille + col) * 3;
            bmp_pixel_init(&img.img_pixels[line][col], pixels[idx], pixels[idx + 1], pixels[idx + 2]);
        }
    }

    bmp_img_write(&img, output_file);
    bmp_img_free(&img);

    free(pixels);
}

int main(int argc, char *argv[]) {
    int taille = 10000;       // Par défaut : 1000x1000 pixels
    int iterations = 1000;   // Par défaut : 1000 itérations
    const char *output = "fractale.bmp";

    if (argc > 1) taille = atoi(argv[1]);
    if (argc > 2) iterations = atoi(argv[2]);
    if (argc > 3) output = argv[3];

    generate_fractal(taille, iterations, output);

    printf("Fractale générée et enregistrée dans le fichier %s\n", output);

    return EXIT_SUCCESS;
}

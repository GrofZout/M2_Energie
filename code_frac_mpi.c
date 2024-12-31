#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "libbmp/libbmp.h"

#define XMIN -1.0
#define XMAX 1.0
#define YMIN -1.0
#define YMAX 1.0

// Paramètres de la fractale
#define A -0.8
#define B 0.156

// Fonction pour calculer les couleurs d'un pixel
void compute_pixel(int col, int line, int taille, int iterationmax, unsigned char *r, unsigned char *g, unsigned char *b) {
    double x = XMIN + col * (XMAX - XMIN) / taille;
    double y = YMAX - line * (YMAX - YMIN) / taille;
    double zx = x, zy = y;
    int i = 0;

    while (i < iterationmax && (zx * zx + zy * zy) <= 4.0) {
        double tmp = zx * zx - zy * zy + A;
        zy = 2.0 * zx * zy + B;
        zx = tmp;
        i++;
    }

    if (i == iterationmax) {
        *r = *g = *b = 0;
    } else {
        *r = (4 * i) % 256;
        *g = (2 * i) % 256;
        *b = (6 * i) % 256;
    }
}

int main(int argc, char **argv) {
    int taille = 10000; // Taille par défaut
    int iterationmax = 1000; // Iterations par défaut
    const char *output_file = "fractale.bmp";

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Lecture des arguments si fournis
    if (argc >= 2) taille = atoi(argv[1]);
    if (argc >= 3) iterationmax = atoi(argv[2]);
    if (argc >= 4) output_file = argv[3];

    int rows_per_process = taille / size;
    int start_row = rank * rows_per_process;
    int end_row = (rank == size - 1) ? taille : start_row + rows_per_process;

    unsigned char *local_pixels = malloc(3 * rows_per_process * taille);
    if (!local_pixels) {
        fprintf(stderr, "Erreur d'allocation mémoire.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for (int line = start_row; line < end_row; line++) {
        for (int col = 0; col < taille; col++) {
            unsigned char r, g, b;
            compute_pixel(col, line, taille, iterationmax, &r, &g, &b);
            int local_line = line - start_row;
            local_pixels[(local_line * taille + col) * 3 + 0] = r;
            local_pixels[(local_line * taille + col) * 3 + 1] = g;
            local_pixels[(local_line * taille + col) * 3 + 2] = b;
        }
    }

    unsigned char *global_pixels = NULL;
    if (rank == 0) {
        global_pixels = malloc(3 * taille * taille);
        if (!global_pixels) {
            fprintf(stderr, "Erreur d'allocation mémoire.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    MPI_Gather(local_pixels, 3 * rows_per_process * taille, MPI_UNSIGNED_CHAR,
               global_pixels, 3 * rows_per_process * taille, MPI_UNSIGNED_CHAR,
               0, MPI_COMM_WORLD);

    if (rank == 0) {
        bmp_img img;
        bmp_img_init_df(&img, taille, taille);
        for (int line = 0; line < taille; line++) {
            for (int col = 0; col < taille; col++) {
                int idx = (line * taille + col) * 3;
                bmp_pixel_init(&img.img_pixels[line][col],
                               global_pixels[idx + 0],
                               global_pixels[idx + 1],
                               global_pixels[idx + 2]);
            }
        }
        bmp_img_write(&img, output_file);
        bmp_img_free(&img);
        printf("Fractale générée et enregistrée dans le fichier %s\n", output_file);
        free(global_pixels);
    }

    free(local_pixels);
    MPI_Finalize();
    return 0;
}

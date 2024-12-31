import java.awt.image.BufferedImage;
import java.io.File;
import javax.imageio.ImageIO;

public class FractalGenerator {

    public static void main(String[] args) {
        // Paramètres par défaut
        int taille = 1000;
        int iterationmax = 1000;
        String output = "fractale.bmp";

        // Lire les arguments de la ligne de commande
        for (int i = 0; i < args.length; i++) {
            switch (args[i]) {
                case "--taille":
                    taille = Integer.parseInt(args[++i]);
                    break;
                case "--iterations":
                    iterationmax = Integer.parseInt(args[++i]);
                    break;
                case "--output":
                    output = args[++i];
                    break;
            }
        }

        // Paramètres de la fractale
        double xmin = -1, xmax = 1;
        double ymin = -1, ymax = 1;
        double a = -0.8;
        double b = 0.156;

        // Créer l'image
        BufferedImage image = new BufferedImage(taille, taille, BufferedImage.TYPE_INT_RGB);

        // Génération de la fractale
        for (int line = 0; line < taille; line++) {
            for (int col = 0; col < taille; col++) {
                int i = 1;
                double x = xmin + col * (xmax - xmin) / taille;
                double y = ymax - line * (ymax - ymin) / taille;
                while (i <= iterationmax && (x * x + y * y) <= 4) {
                    double newX = x * x - y * y + a;
                    y = 2 * x * y + b;
                    x = newX;
                    i++;
                }
                int color;
                if (i > iterationmax && (x * x + y * y) <= 4) {
                    color = (0 << 16) | (0 << 8) | 0; // Noir
                } else {
                    color = ((4 * i) % 256 << 16) | ((2 * i) % 256 << 8) | ((6 * i) % 256); // Couleur RGB
                }
                image.setRGB(col, line, color);
            }
        }

        // Sauvegarder l'image dans un fichier BMP
        try {
            File outputfile = new File(output);
            ImageIO.write(image, "bmp", outputfile);
            System.out.println("Fractale générée et enregistrée dans le fichier " + output);
        } catch (Exception e) {
            System.err.println("Erreur lors de la sauvegarde de l'image: " + e.getMessage());
        }
    }
}

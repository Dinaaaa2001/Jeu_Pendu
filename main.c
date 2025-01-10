#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include "minirisc.h"
#include "uart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "harvey_platform.h"
#include <stdlib.h>
#include <time.h>
#include "font.h"


#define NBR_MOTS (sizeof(mots_possibles) / sizeof(mots_possibles[0]))


// Dimensions de l'écran
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

// Liste des mots possibles
static const char* mots_possibles[] = {
    "MINIRISC",
    "PROGRAMMATION",
    "VACHE",
    "TABLE",
    "GAUFRE",
    "CHIEN",
    "VOITURE",
    "MARSEILLE",
    "JUL",
    "ORDINATEUR",
    "ECRAN",
    "CLAVIER",
    "SOURIS",
    "BUREAU",
    "FENETRE",
    "PORTE",
    "LIVRE",
    "STYLO",
    "CAHIER",
    "CHAUSSURE",
    "MONTRE",
    "BOUTEILLE",
    "TELEPHONE",
    "POMME",
    "BANANE",
    "ORANGE",
    "SOLEIL",
    "LUNE",
    "ETOILE",
    "VIOLETTE",
    "NEIGE",
    "PLUIE",
    "MONTAGNE",
    "RIVIERE"
};


// Prototypes des fonctions non standards
void afficher_mot_ecran();
void draw_rectangle(int x, int y, int width, int height, uint32_t color);
void draw_line(int x1, int y1, int x2, int y2, uint32_t color, int thickness);
void draw_disk(int x, int y, int diam, uint32_t color);
void draw_line_single(int x1, int y1, int x2, int y2, uint32_t color);


// Variables globales pour le jeu
static char mot_a_deviner[20]; // Mot actuel
static char mot_affiche[20];   // Mot masqué
static int erreurs = 0;        // Compteur d'erreurs
static int max_erreurs = 11;    // Nombre maximal d'erreurs

// Tampon d'affichage
static uint32_t frame_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

// Fonction d'initialisation vidéo
void init_video() {
    memset(frame_buffer, 0, sizeof(frame_buffer)); // Effacer l'écran
    VIDEO->WIDTH = SCREEN_WIDTH;
    VIDEO->HEIGHT = SCREEN_HEIGHT;
    VIDEO->DMA_ADDR = frame_buffer;
    VIDEO->CR = VIDEO_CR_IE | VIDEO_CR_EN;
}

// Fonction pour afficher une chaîne de caractères sur la console
void afficher_mot() {
    printf("Mot : %s | Erreurs : %d/%d\n", mot_affiche, erreurs, max_erreurs);
}

// Gestion de cast

char to_uppercase(char c) {
    return toupper(c);
}

// Fonction pour sélectionner un mot aléatoire
void choisir_mot_aleatoire(char* mot_choisi, size_t taille) {
    srand(time(NULL));
    int index = rand() % NBR_MOTS;
    strncpy(mot_choisi, mots_possibles[index], taille - 1);
    mot_choisi[taille - 1] = '\0';

    // Convertir tout le mot en majuscules
    for (size_t i = 0; i < strlen(mot_choisi); i++) {
        mot_choisi[i] = to_uppercase(mot_choisi[i]);
    }
}

// Fonction pour représenter le pendu (prend en arguments le nombre d'erreurs)
void draw_part(int erreurs) {
    switch (erreurs) {
        case 1: // Barre principale (potence)
            draw_rectangle(100, 50, 10, 200, 0xFF0000); // Rectangle vertical
            break;
        case 2: // Barre de support au sol
            draw_rectangle(50, 250, 200, 10, 0xFF0000); // Rectangle horizontal
            break;
        case 3: // Barre horizontale
            draw_rectangle(100, 50, 100, 10, 0xFF0000); // Rectangle horizontal
            break;
        case 4: // Barre inclinée sur la potence
            draw_line(120, 60, 110, 80, 0xFF0000, 7); // Ligne inclinée épaisse
            break;
        case 5: // Corde
			draw_line(180, 60, 180, 100, 0xFF0000, 6); // Ligne verticale épaisse
			break;
        case 6: // Tête
            draw_disk(165, 80, 30, 0xFF0000); // Cercle pour la tête
            break;
        case 7: // Corps
            draw_rectangle(175, 110, 10, 80, 0xFF0000); // Rectangle vertical pour le corps
            break;
        case 8: // Bras gauche
            draw_line(175, 110, 140, 150, 0xFF0000,1); // Ligne inclinée pour le bras gauche
            break;
        case 9: // Bras droit
            draw_line(185, 110, 220, 150, 0xFF0000,1); // Ligne inclinée pour le bras droit
            break;
        case 10: // Jambe gauche
            draw_line(175, 190, 150, 240, 0xFF0000,1); // Ligne inclinée pour la jambe gauche
            break;
        case 11: // Jambe droite
            draw_line(185, 190, 210, 240, 0xFF0000,1); // Ligne inclinée pour la jambe droite
            break;
        default:
            printf("Erreur : aucune partie à dessiner pour %d erreurs.\n", erreurs);
            break;
    }
}


// Tâche pour capturer les entrées clavier
void saisie_tache(void *arg) {
    (void)arg;
    char lettre;

    while (1) {
        lettre = getchar();           // Lire la lettre saisie
        lettre = to_uppercase(lettre); // Convertir en majuscule
        printf("Lettre entrée : %c\n", lettre);

        int correct = 0;
        for (size_t i = 0; i < strlen(mot_a_deviner); i++) {
            if (mot_a_deviner[i] == lettre && mot_affiche[i] != lettre) {
                mot_affiche[i] = lettre;
                correct = 1;
            }
        }
        
         // Vérifier la validité de la lettre
        if (!isalpha(lettre)) {
            printf("Entrée invalide : %c. Veuillez entrer une lettre.\n", lettre);
            continue;
        }

        if (!correct) {
            erreurs++;
            draw_part(erreurs); // Dessiner une nouvelle partie du pendu
        }

        afficher_mot();
        afficher_mot_ecran();

        if (erreurs >= max_erreurs) {
            printf("Défaite ! Le mot était : %s\n", mot_a_deviner);
            minirisc_halt();
        } else if (strcmp(mot_a_deviner, mot_affiche) == 0) {
            printf("Victoire ! Vous avez trouvé le mot : %s\n", mot_a_deviner);
            minirisc_halt();
        }
    }
}

// Tâche pour gérer l'affichage à l'écran
void affichage_tache(void *arg) {
    (void)arg;

    while (1) {
        // Vous pouvez ajouter des animations ou un rafraîchissement de l'écran ici
        vTaskDelay(pdMS_TO_TICKS(100)); // Pause de 100 ms
    }
}

// Fonction pour dessiner des rectangles 
void draw_rectangle(int x, int y, int width, int height, uint32_t color) {
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
        return;
    }
    int i, j;
    int x_start = x < 0 ? 0 : x;
    int y_start = y < 0 ? 0 : y;
    int x_end = x + width;
    int y_end = y + height;
    if (x_end > SCREEN_WIDTH) {
        x_end = SCREEN_WIDTH;
    }
    if (y_end > SCREEN_HEIGHT) {
        y_end = SCREEN_HEIGHT;
    }
    for (j = y_start; j < y_end; j++) {
        for (i = x_start; i < x_end; i++) {
            frame_buffer[j * SCREEN_WIDTH + i] = color;
        }
    }
}

// Fonction pour dessiner des cercles (pour la tête)
void draw_disk(int x, int y, int diam, uint32_t color) {
    if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
        return;
    }
    int i, j;
    int x_start = x < 0 ? 0 : x;
    int y_start = y < 0 ? 0 : y;
    int x_end = x + diam;
    int y_end = y + diam;
    int rad2 = diam * diam / 4;
    int xc = x + diam / 2;
    int yc = y + diam / 2;
    if (x_end > SCREEN_WIDTH) {
        x_end = SCREEN_WIDTH;
    }
    if (y_end > SCREEN_HEIGHT) {
        y_end = SCREEN_HEIGHT;
    }
    for (j = y_start; j < y_end; j++) {
        int j2 = (yc - j) * (yc - j);
        for (i = x_start; i < x_end; i++) {
            int i2 = (xc - i) * (xc - i);
            if (j2 + i2 <= rad2) {
                frame_buffer[j * SCREEN_WIDTH + i] = color;
            }
        }
    }
}

// Fonction pour dessiner des lignes inclinées
void draw_line(int x1, int y1, int x2, int y2, uint32_t color, int thickness) {
    for (int offset = -thickness / 2; offset <= thickness / 2; offset++) {
        if (abs(x2 - x1) > abs(y2 - y1)) {
            // Ligne principalement horizontale : décaler en y
            draw_line_single(x1, y1 + offset, x2, y2 + offset, color);
        } else {
            // Ligne principalement verticale : décaler en x
            draw_line_single(x1 + offset, y1, x2 + offset, y2, color);
        }
    }
}

// Fonction pour tracer une ligne simple
void draw_line_single(int x1, int y1, int x2, int y2, uint32_t color) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1; 
    int err = dx + dy, e2;

    while (1) {
        if (x1 >= 0 && x1 < SCREEN_WIDTH && y1 >= 0 && y1 < SCREEN_HEIGHT) {
            frame_buffer[y1 * SCREEN_WIDTH + x1] = color; // Dessiner le pixel
        }
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}


void afficher_mot_ecran() {
    int x_start = 200;  // Position initiale en x
    int y_start = 350; // Position en y pour les soulignés et lettres
    int spacing = 20;  // Espacement entre chaque lettre ou souligné

    for (size_t i = 0; i < strlen(mot_affiche); i++) {
        int x_pos = x_start + i * spacing;

        if (mot_affiche[i] == '_') {
            // Dessiner un souligné
            draw_line(x_pos, y_start, x_pos + 10, y_start, 0xFFFFFF, 2); // Ligne blanche
        } else {
            // Afficher la lettre trouvée
            draw_rectangle(x_pos, y_start - 10, 10, 20, 0xFF0000); // Lettre en rouge
        }
    }
}



int main() {
    // Initialisation
    init_uart();
    choisir_mot_aleatoire(mot_a_deviner, sizeof(mot_a_deviner));
    memset(mot_affiche, '_', strlen(mot_a_deviner));
    mot_affiche[strlen(mot_a_deviner)] = '\0'; // Terminer la chaîne

    printf("Bienvenue dans le jeu du Pendu !\n");
    afficher_mot();
    init_video();
   
    afficher_mot_ecran();


    // Création des tâches FreeRTOS
    xTaskCreate(saisie_tache, "Saisie", 1024, NULL, 1, NULL);

    // Démarrage du planificateur
    vTaskStartScheduler();

    return 0;
}



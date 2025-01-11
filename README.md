
# Projet : Jeu du Pendu sur MiniRISC

## Objectif du projet

Le but de ce projet est de créer une version du jeu classique du pendu sur une plateforme MiniRISC Harvey. L'objectif principal est de développer une application interactive qui utilise des périphériques tels que l'écran pour afficher le jeu et le clavier pour entrer des lettres.

## Fonctionnalités prévues

1. Sélection aléatoire d'un mot à deviner.
2. Affichage graphique du pendu et du mot masqué sur l'écran.
3. Gestion des entrées utilisateur pour deviner les lettres.
4. Affichage des parties du pendu en cas d'erreur.
5. Victoire si le mot est deviné, défaite si le pendu est complété.

## Fonctionnalités réalisées

### Fonctionnalités fonctionnelles
- **Sélection aléatoire des mots à deviner**.
  
  ```c
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
  ```
  Cette fonction sélectionne un mot de manière aléatoire dans une liste prédéfinie, le copie dans une chaîne de destination, et le convertit entièrement en majuscules pour uniformiser l'affichage. Elle prend également en compte les problèmes de cast en utilisant une fonction dédiée (`to_uppercase`) pour garantir la conversion correcte des caractères en majuscules. Cela permet de gérer sans erreur les saisies du joueur, qu'elles soient en majuscules ou en minuscules.

---
  
- **Affichage graphique des parties du pendu.**

Pour afficher graphiquement les différentes parties du pendu en fonction des erreurs commises par le joueur, nous utilisons la méthode suivante :

```c
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
            draw_line(175, 110, 140, 150, 0xFF0000, 1); // Ligne inclinée pour le bras gauche
            break;
        case 9: // Bras droit
            draw_line(185, 110, 220, 150, 0xFF0000, 1); // Ligne inclinée pour le bras droit
            break;
        case 10: // Jambe gauche
            draw_line(175, 190, 150, 240, 0xFF0000, 1); // Ligne inclinée pour la jambe gauche
            break;
        case 11: // Jambe droite
            draw_line(185, 190, 210, 240, 0xFF0000, 1); // Ligne inclinée pour la jambe droite
            break;
        default:
            printf("Erreur : aucune partie à dessiner pour %d erreurs.\n", erreurs);
            break;
    }
}
```

Cette fonction prend en entrée le nombre d'erreurs et, en fonction de celui-ci, dessine une nouvelle partie du pendu. Elle utilise les fonctions suivantes :  

  * `draw_rectangle` : pour dessiner les barres de la potence et le corps.  
  * `draw_disk` : pour dessiner la tête.  
  * `draw_line` : pour dessiner les bras, les jambes, et les éléments inclinés.  

---
- **Affichage des lettres devinées et du mot.**

Cette fonction est responsable de l'affichage à l'écran du mot à deviner, en utilisant des soulignés pour les lettres non encore trouvées et des rectangles pour les lettres déjà découvertes.

```c
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
```
Cette fonction est appelée dans le `main`, au début, pour afficher sur l'écran le nombre de tirets correspondant à la longueur du mot à trouver `mot_affiche`. Si la lettre saisie par l'utilisateur est correcte, un carré rouge apparaît par-dessus le tiret correspondant. Dans le terminal, la lettre est affichée directement.

---


### Fonctionnalités non fonctionnelles  

- **Affichage des lettres sur l'écran** : Comme expliqué, lorsqu'une lettre correcte est trouvée, un carré rouge apparaît à la place du tiret. À l'origine, je souhaitais afficher la lettre directement sur l'écran. J'ai tenté d'utiliser la fonction `font_8x16_draw_char` contenue dans `font.c`, mais je n'ai pas réussi à la rendre fonctionnelle. Malgré plusieurs tests, aucune lettre ne s'affichait.  

  En améliorant cette partie, cela permettrait de jouer uniquement via l'écran, sans dépendre du terminal pour voir les lettres. De la même manière, des messages tels que "victoire" ou "défaite" pourraient être affichés à la fin de la partie, ainsi qu'une liste des lettres déjà proposées pour éviter les doublons.  

## Mécanique du projet

Le jeu du pendu fonctionne en trois étapes principales : saisie des lettres, vérification et affichage. 

### Rôle des tâches et fonctions

- **`main`** :  
  Initialise les composants du jeu (UART, vidéo, sélection du mot) et lance la tâche `saisie_tache`.

- **`saisie_tache`** :  
  Capture les lettres saisies par l'utilisateur. Si la lettre est correcte, elle est affichée dans le mot masqué. Sinon, une nouvelle partie du pendu est dessinée.

- **`afficher_mot_ecran`** :  
  Met à jour l'écran en affichant les lettres trouvées ou des tirets pour celles encore à deviner.

- **`draw_part`** :  
  Dessine une partie du pendu en fonction du nombre d'erreurs.

### Fonctionnement général

Le programme commence par choisir un mot aléatoire et l'afficher sous forme de tirets. À chaque lettre saisie, le mot masqué ou le pendu est mis à jour, jusqu'à ce que le mot soit trouvé ou que le joueur dépasse le nombre maximal d'erreurs.


## Périphériques utilisés

- **Écran** : Affiche le mot masqué, les lettres devinées et le pendu.
- **Clavier** : Permet à l'utilisateur d'entrer des lettres.

## Synchronisation

J'aurais voulu utiliser le port de FreeRTOS pour mettre en place plusieurs tâches concurrentes qui bloquent sur des queues. Cela aurait permis de structurer le jeu de manière plus modulaire et réactive. Voici comment cette approche aurait pu être appliquée dans le cadre du jeu du pendu :  

- **Tâche de saisie** : Capturer les entrées clavier de l'utilisateur et les transmettre via une queue à la tâche de vérification.  
- **Tâche de vérification** : Vérifier si la lettre saisie est correcte, mettre à jour le mot masqué (`mot_affiche`), et incrémenter le compteur d'erreurs si nécessaire.  
- **Tâche d'affichage** : Mettre à jour l'écran en fonction de l'état actuel du jeu (afficher les lettres trouvées, dessiner les parties du pendu, ou afficher des messages de victoire/défaite).  

Chaque tâche aurait été bloquée sur une queue ou un sémaphore, assurant ainsi une synchronisation fluide et une séparation claire des responsabilités.  

Malheureusement, je n'ai pas réussi à implémenter cette structure complètement fonctionnelle.

## Compilation et exécution

1. **Compilation** :
   - Placez-vous dans le dossier principal du projet.
   - Exécutez la commande :
     ```bash
     make
     ```

2. **Exécution** :
   - Lancez le jeu sur l'émulateur avec :
     ```bash
     make exec
     ```

## Conclusion

Ce projet a permis d'explorer :  
- L’affichage graphique simple sur un écran Mini-RISC.  
- La gestion des entrées utilisateur et la synchronisation des tâches.  

Cette version reste encore assez simple, mais l'ajout de nouvelles animations et fonctionnalités, comme l'affichage des lettres directement sur l'écran ou des messages de fin de partie, pourrait rendre le jeu plus abouti et attrayant.



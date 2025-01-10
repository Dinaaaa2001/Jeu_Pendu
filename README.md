
# Projet : Jeu du Pendu sur MiniRISC

## Objectif du projet

Le but de ce projet est de créer une version du jeu classique du pendu sur une plateforme MiniRISC en utilisant FreeRTOS pour la gestion des tâches. L'objectif principal est de développer une application interactive qui utilise des périphériques tels que l'écran pour afficher le jeu et le clavier pour entrer des lettres.

## Fonctionnalités prévues

1. Sélection aléatoire d'un mot à deviner.
2. Affichage graphique du pendu et du mot masqué sur l'écran.
3. Gestion des entrées utilisateur pour deviner les lettres.
4. Affichage des parties du pendu en cas d'erreur.
5. Victoire si le mot est deviné, défaite si le pendu est complété.

## Fonctionnalités réalisées

### Fonctionnalités fonctionnelles
- Sélection aléatoire des mots à deviner.
- Affichage graphique des lettres devinées et des parties du pendu.
- Gestion des erreurs et affichage des lettres non devinées.
- Synchronisation entre les tâches pour la saisie et la vérification des lettres.

### Fonctionnalités non fonctionnelles
- Pas de gestion des animations avancées.
- Aucun son ou retour haptique.

## Mécanique du projet

### Structure des tâches

- **Tâche de saisie** : 
  - Lit les entrées clavier.
  - Envoie chaque lettre saisie via une queue FreeRTOS pour traitement.

- **Tâche de vérification** :
  - Reçoit les lettres de la queue.
  - Met à jour le mot masqué si la lettre est correcte ou incrémente le compteur d'erreurs.
  - Dessine les parties du pendu en fonction des erreurs.

- **Tâche d'affichage** :
  - Gère le rendu visuel du mot masqué et des parties du pendu sur l'écran.
  - Rafraîchit l'écran régulièrement pour maintenir un affichage fluide.

### Périphériques utilisés

- **Écran** : Affiche le mot masqué, les lettres devinées et le pendu.
- **Clavier** : Permet à l'utilisateur d'entrer des lettres.

### Synchronisation

Les tâches sont synchronisées via une queue FreeRTOS qui transmet les lettres saisies de la tâche de saisie à la tâche de vérification.

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

3. **Dépendances** :
   - Assurez-vous que l'environnement FreeRTOS est correctement configuré.
   - Le compilateur `riscv32-MINIRISC-elf-gcc` doit être installé et accessible.

## Conclusion

Ce projet illustre l'utilisation des concepts de systèmes embarqués, tels que la gestion des tâches avec FreeRTOS, la synchronisation via des queues, et l'interaction utilisateur à travers des périphériques simples. Le code peut être amélioré pour ajouter des animations, des sons, ou des mécanismes de jeu supplémentaires.


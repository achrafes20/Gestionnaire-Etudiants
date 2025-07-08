# 📚 Système de Gestion des Étudiants en C

Un projet complet en langage C permettant de gérer une base de données d'étudiants avec des fonctionnalités avancées telles que la recherche, la modification, la suppression, le tri, le calcul de moyennes et la génération de rapports académiques.

## 🚀 Fonctionnalités

- 📄 Création d'une nouvelle liste d'étudiants
- ➕ Ajout de nouveaux étudiants
- 🔍 Recherche par ID, nom, prénom ou âge
- ✏️ Modification d'un étudiant (nom, prénom, âge ou notes)
- ❌ Suppression sécurisée avec confirmation
- 📊 Génération de rapport individuel avec moyenne et mention
- 📈 Résumé académique de toute la classe (moyennes, stats, classement)
- 💾 Sauvegarde automatique dans un fichier texte
- 💡 Utilisation d’une **table de hachage** pour des recherches rapides
- 📁 Chargement/sauvegarde des données via des fichiers (`.txt`, `.bin`)

## 🧠 Concepts abordés

- Structures (`struct`)
- Fichiers (`fopen`, `fread`, `fwrite`, `fprintf`, etc.)
- Manipulation de chaînes (`fgets`, `strcspn`, `toupper`, etc.)
- Table de hachage avec chaînes de collision
- Algorithme de tri (QuickSort)
- Allocation dynamique (`malloc`, `free`)

## 🏗️ Architecture du projet

```plaintext
.
├── main.c                  # Fichier principal
├── etudiants.txt           # Fichier texte contenant les données des étudiants
├── matieres.bin            # Fichier binaire contenant les matières et coefficients
└── resume_academique.txt   # Rapport académique de la classe généré automatiquement
```
## 📷 Aperçu du menu (console)
```plaintext
1. Creer une nouvelle liste d'etudiants (l'ancienne liste va être supprimée !)
2. Ajouter un étudiant
3. Supprimer un étudiant
4. Modifier un étudiant
5. Rechercher un étudiant
6. Générer un rapport pour un étudiant
7. Afficher un résumé académique
0. Quitter
```
## ⚙️ Compilation
Sous Windows (avec GCC - MinGW) :
```bash
gcc main.c -o gestion_etudiants
```
```bash
gestion_etudiants.exe
```

Sous Linux (remplacer conio.h) :
```bash
gcc main.c -o gestion_etudiants -D__LINUX__
```
```bash
./gestion_etudiants
```
🔧 Vous devrez supprimer ou remplacer les fonctions spécifiques à Windows (getch(), system("cls")) si vous utilisez Linux ou macOS.
## 📌 Recommandations
- Toujours sauvegarder les données (etudiants.txt) avant de quitter.
-Ne pas dépasser 8 matières (limite définie dans le code).
-L’ID de l’étudiant doit être unique.

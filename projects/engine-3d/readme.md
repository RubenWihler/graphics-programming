# Étude Projet Semestre

Auteur: Ruben Wihler
Projet : Moteur 3D OpenGL C

## Objectif du projet 

Faire évoluer mon moteur de rendu OpenGL 2D personnalisé vers la 3D et y intégrer une démo géométrique interactive (ex: système de particules 3D avec caméra spatiale).

#### 1. Mon approche technique

- **Technologies envisagées :** C, OpenGL, GLFW, GLEW, et la librairie mathématique CGLM.
- **Organisation générale :** Architecture "Moteur / Application". Le moteur (déjà partiellement développé) abstraira le pipeline OpenGL. L'application consistera à gérer la boucle de rendu 3D, le Z-buffer, et les calculs matriciels pour afficher des primitives géométriques ou un système de particules dans l'espace.
- **Pourquoi ce choix ?** J'ai déjà codé un moteur 2D complet cet été. Réutiliser cette base me permet de gagner du temps sur l'initialisation (fenêtre, buffers) pour me concentrer *exclusivement* sur la géométrie algorithmique complexe de la 3D : matrices de vue, perspective, rotations (quaternions/angles d'Euler) et transformations spatiales.

#### 2. Ce qui risque d’être difficile

- **Difficulté 1 : La caméra spatiale et la navigation (LookAt / Fly).**
  - *Pourquoi :* Contrairement à la 2D (projection orthographique simple), naviguer dans un espace 3D demande de calculer en permanence la matrice de Vue. Il faudra gérer correctement le vecteur "Forward", "Right" et "Up" sans subir le problème du *Gimbal Lock* (blocage de cardan).
- **Difficulté 2 : La gestion de la profondeur (Depth Testing / Z-buffer).**
  - *Pourquoi :* En 2D, l'ordre d'affichage suffisait. En 3D, il va falloir configurer correctement le pipeline OpenGL pour le Depth Testing, s'assurer que les objets se cachent correctement selon la position de la caméra, et adapter les shaders.

#### 3. Première action claire

- **Quelle est la toute première tâche à réaliser ?** Migrer le pipeline mathématique et le shader de la 2D vers la 3D. Concrètement : remplacer la projection orthographique par une matrice de projection perspective, et remplacer les coordonnées 2D (x,y) des *Vertex Buffers* par des coordonnées 3D (x,y,z).
- **Que doit-on obtenir concrètement ?** L'affichage d'un simple Cube en 3D tournant sur lui-même au centre de l'écran, avec les bonnes déformations de perspective (le fond du cube paraît plus petit que le devant) et le Z-Buffer actif pour que les faces arrière ne s'affichent pas par-dessus les faces avant.



> *document généré par IA (Gemini-3.1)
> J'ai écrit un brouillon en .txt et lui ai demandé de mettre au propre
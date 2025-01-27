# Rapport - Apprentissage programmation graphique

Auteur: [Ruben Wihler](https://github.com/RubenWihler)
Date de création: 2025-01-27
Dernière mise à jour: 2025-01-27

## Introduction

Je suis un étudiant de première année en ingénierie informatique à l'HEPIA. Ayant fini mon premier semestre, j'ai 3 semaines de vacances devant moi. J'ai décidé de profiter de ce temps pour essayer d'apprendre quelque chose de nouveau. Il a donc fallu que je choisisse parmi un nombre incalculable de domaines supper intéressants que je ne connais pas encore.

## Pourquoi la programmation graphique?

Je suis un grand fan de gamedev, c'est d'ailleurs en créant des jeux sur Unity que j'ai commencé à apprendre à programmer. Cependant, après avoir acquis un peu d'expérience, j'ai vite réalisé que mon travail consistait plus à créer une logique autour de librairies. 

Je pense que c'est vraiment une bonne chose de commencé par là, car cela m'a permis d'acquérir des bases solides dans tout ce qui est design pattern, architecture logicielle, etc. Sans avoir à me soucier du côté "plus bas niveau". Pendant environ 3 ans, j'ai donc uniquement poussé mes connaissances dans la conception de systèmes, essayer de trouver la manière la plus optimale en terme de maintenabilité, scalabilité, etc de représenter un problème. Je suis donc devenu assez bon pour faire du code propre et bien structuré. Aujourd'hui, je pense que pour m'améliorer, il est temps de m'attaquer à des problèmes plus complexes, plus bas niveau. 

Il y a quelques mois j'ai créé une petite simulation de particules en C avec la librairie SDL. J'ai bien aimé et mtn je veux avoir plus de particules, beaucoup plus de particules. Je me suis un peu renseigné sur le sujet et j'ai découvert que l'ont pouvait faire les calculs de manière parallèle sur le GPU avec des compute shaders. C'est donc le but final de ce projet, créer une simulation de particules avec des compute shaders.

Durant cette breve recherche, j'ai aussi découvert que je ne connais rien à la programmation graphique. Je n'ai jamais touché à un shader de ma vie. Ca parlait de vertex, fragment, buffer, etc. Je n'ai aucune idée de ce que c'est. C'est donc un domaine qui m'est complètement inconnu. C'est d'ailleurs surement ce qui m'attire, j'y vois une occasion de me prouver à moi-même, qu'avec de la motivation et toutes les ressources disponibles gratuitement sur internet, on peut apprendre n'importe quoi tout seul.

## Methodologie

Pour atteindre mon objectif, j'ai décidé de suivre une méthodologie en 3 étapes:

1. **Recherche**: Je vais deja commencer par regarder des vidéos assez générales sur le sujet pour comprendre les bases. Et savoir un peu par où commencer. 

2. **Recherche plus spécifique et mini-projets**: Ensuite, je vais me lancer dans des mini-projets pour apprendre les bases. Pour chaque projet, je vais m'informé sur le sujet en regardant des vidéos, et prendre des notes. Ensuite, je vais essayer de reproduire ce que j'ai vu ou un peu différent (un peu chiant de faire la même chose que quelqu'un d'autre).

3. **Projet final**: Une fois que j'aurai acquis les bases, je pourrai commencer à travailler sur mon projet final.

## ... A suivre

## Suivi de l'apprentissage

Dans cette section, je vais documenter mon apprentissage au fur et à mesure. Je vais essayer de faire un petit résumé de ce que j'ai appris à la fin de chaque journée.

### Recherche générale

Comme spécifié dans la [méthodologie](#methodologie), j'ai commencé par regarder des vidéos générales sur le sujet. Voici la liste de ce que j'ai regardé:

- [Nvidia CUDA in 100 Seconds - Fireship](https://youtu.be/pPStdjuYzSI?si=jLHnVgL2ZG-oOiqp)
- [Writing Code That Runs FAST on a GPU](https://youtu.be/8sDg-lD1fZQ?si=F70PixYlQ-yizMdq)
- [Building Collision Simulations: An Introduction to Computer Graphics](https://youtu.be/eED4bSkYCB8?si=wZ8LHz5d6M4bYY1Q)

**A revoir quand je m'y connaitrai un peu plus:**

- [Système de particules utilisant le Compute Shader // Série intermédiaire OpenGL](https://youtu.be/pzAZ0xjWDv8?si=9alOmLum_xILljsY)
- [Modern OpenGL Tutorial - Compute Shaders](https://youtu.be/nF4X9BIUzx0?si=6a63d9_bCu5KO9eJ)

Après avoir regardé ces vidéos, j'ai une idée un peu plus claire de ce que je vais devoir apprendre. Avant de directement me lancer dans le compute shader, je vais commencer par apprendre les bases de la programmation graphique. 

Pour cela j'ai vu que [The Cherno](https://www.youtube.com/c/TheCherno) avait une [série de vidéos](https://youtube.com/playlist?list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&si=mA2Kha-pVY-1e4FV) sur le sujet. Appreciant son style, je vais suivre cette série et je verrai ensuite si je me sens prêt à commencer à apprendre le compute shader.

### Serie de vidéos de The Cherno

Notes:

Un **shader** est un programme qui s'exécute sur le GPU. Il est écrit en **GLSL** (OpenGL Shading Language). Il est utilisé pour décrire comment les données doivent être traitées par le GPU.

Les shaders ne sont pas forcément liés aux effets visuels, genre lumière, ombre, etc. C'est juste un programme qui s'exécute sur le GPU. (je pensais que les shaders étaient le nom donné aux programmes qui s'occupent des effets visuels).

Donc un shader peut être utilisé pour faire des calculs, des transformations, etc.
(D'ailleurs, d'apres ce que j'ai compris, il sont utilisés dans le machine learning pour entrainer des modèles sur le GPU - c'est pour ça que Nvidia a sorti CUDA).

Il y a plusieurs librairies qui permettent de faire de la programmation graphique (OpenGL, DirectX, Vulkan, etc). **OpenGL** est la plus ancienne et la plus utilisée. Elle est supportée par tous les systèmes d'exploitation. **Vulkan** est plus récente et plus performante, mais plus complexe. **DirectX** est développée par Microsoft et est utilisée principalement pour les jeux sur Windows.

En soit, OpenGL n'est pas vraiment un librairie, car elle ne contient pas de code exécutable. C'est une **API** (Application Programming Interface) qui permet de communiquer avec le GPU. Elle fournit des fonctions pour charger des shaders, envoyer des données aux shaders, etc.
Comme le dit The Cherno, OpenGL est juste une spécification. C'est a dire que c'est juste un document qui décrit comment les choses doivent fonctionner. C'est aux fabricants de GPU de créer des drivers qui implémentent cette spécification.

C'est pour ça que OpenGL est supportée par tous les systèmes d'exploitation. Les fabricants de GPU (Nvidia, AMD, Intel) créent des drivers OpenGL pour leur GPU.


Etant donné que je suis sur Linux, je vais utiliser **OpenGL**.
OpenGL est une API qui permet de communiquer avec le GPU. Elle fournit des fonctions pour charger des shaders, envoyer des données aux shaders, etc.

#### C'est quoi un shader?

Il y a 2 grands types de shaders: les **vertex shaders** et les **fragment shaders**.

Les **vertex shaders** sont exécutés pour savoir où placer les vertices sur l'écran. Ils sont exécutés une fois par vertex. (Donc si ont a un triangle, le vertex shader sera exécuté 3 fois).

Les **fragment shaders** sont exécutés pour chaque pixel de l'écran. Ils sont exécutés après le vertex shader. Ils permettent de déterminer la couleur de chaque pixel.

#### Dans quel ordre sont exécutés les shaders?
La **pipeline graphique** est le processus par lequel les données sont transformées en pixels à l'écran. Elle est composée de plusieurs étapes, dont les shaders: 

- **Vertex Shader**: Transforme les coordonnées des sommets en coordonnées de l'écran.
- **Primitive Assembly**: Assemble les sommets en primitives (points, lignes, triangles).
- **Geometry Shader**: Transforme les primitives en d'autres primitives.
- **Rasterization**: Convertit les primitives en fragments (pixels).
- **Fragment Shader**: Détermine la couleur de chaque pixel.
- **Alpha Test and Blending**: Applique la transparence et le mélange des couleurs.
- **Framebuffer**: Envoie les pixels à l'écran.

OpenGL s'occupe de la pipeline graphique. Il fournit des fonctions pour charger des shaders, envoyer des données aux shaders, etc. En gros, nous notre boulot c'est d'écrire les shaders et de dire à OpenGL comment les utiliser.

#### Comment on écrit un shader?

Un shader est écrit en **GLSL** (OpenGL Shading Language). C'est un langage de programmation basé sur le C. Il est utilisé pour décrire comment les données doivent être traitées par le GPU.

### Installation de GLFW

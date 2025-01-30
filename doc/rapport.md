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

## Mes composants

Avant de commencer, je vais indiquer les composants et drivers de ma machine:

```bash
CPU: 10-core (2-mt/8-st) 13th Gen Intel Core i7-1355U (-MST AMCP-)
speed/min/max: 630/400/5000:3700 MHz Kernel: 6.1.0-30-amd64 x86_64 Up: 7h 5m
Mem: 5797.8/31784.2 MiB (18.2%) Storage: 953.87 GiB (3.7% used) Procs: 322
Shell: Zsh inxi: 3.3.26
Graphics:
  Device-1: Intel Raptor Lake-P [Iris Xe Graphics] driver: i915 v: kernel
  Device-2: NVIDIA GA107M [GeForce RTX 2050] driver: N/A
  Device-3: Chicony ACER FHD User Facing type: USB driver: uvcvideo
  Display: wayland server: X.Org v: 1.22.1.9 with: Xwayland v: 22.1.9
    compositor: gnome-shell v: 43.9 driver: X: loaded: vesa
    unloaded: fbdev,modesetting dri: iris gpu: i915 resolution:
    1: 1920x1200~60Hz 2: 1920x1080~60Hz
  API: OpenGL v: 4.6 Mesa 22.3.6 renderer: Mesa Intel Graphics (RPL-P)
```

> Comme on peut le voir, j'ai une carte graohique Nvidia RTX 2050.
Un truc assez marant, c'est que je n'avais pas vu que je n'avais pas les drivers Nvidia installés. 
Pour l'instant, ce n'est pas un problème, car je vais utiliser OpenGL pour apprendre les bases. Mais au bout d'un moment, je vais les installer, car j'aimerais bien utiliser CUDA pour tester un peu.

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

Je suis sur un Debian, donc j'ai installé GLFW avec le gestionnaire de paquets aptitude:

```bash
sudo apt install libglfw3
sudo apt install libglfw3-dev
```

> **Note**: Je n'ai pas eu besoin de compiler moi-même GLFW, car il est disponible dans les dépôts de Debian.

### Premier programme OpenGL

J'ai utiliser la template de projet que j'utilise abituellement pour mes projet C. 
Une template assez simple, avec un Makefile qui permet de compiler le projet en scanant tous les fichiers .c dans le dossier src.

> Je ne sais toujours pas si je vais faire les projets en C ou en C++. J'ai vu que The Cherno utilise du C++. Ayant beaucoup plus d'expérience en C, je pense que je vais commencer par faire les projets en C. Si je vois que ca devient vraiment un bordel au niveau orgnisation, je passerai en C++.

Mais bref j'ai fait 2 projet, un en C et un en C++. (Il y a juste le makefile qui change).

#### Contenu

J'ai copier coller le code de la doc de GLFW pour créer une fenêtre. J'ai ensuite ajouter un shader pour dessiner un triangle. (En gros j'ai rjt 5 lignes de code a l'endroit ou c'est marqué "Render here").

#### Compilation

Pour compiler le projet, j'ai ajouter les libraries suivante:

```makefile
LIBS := -lpthread -lglfw -lGL -lX11 -lXrandr -lXi -ldl -lXxf86vm -lXinerama -lXcursor
```

> Honnêtement, je ne sais pas à quoi servent toutes ces librairies. D'ailleurs, pour la plupart, si je les enlève, le programme compile quand même. Mais bon, je vais les laisser, on sait jamais.

### Utilisation de GLEW

Pour utiliser les fonctions definis par OpenGL, il faut charger aller chercher dans les fichier dll des drivers du GPU de la machine. Pour cela, on utilise une librairie qui s'occupe de charger ces fonctions. J'ai choisi d'utiliser GLEW.

> J'aurais aussi pu utiliser GLAD, mais on en a pas vraiment besoin pour l'instant. Si j'ai bien compris. GLAD fournit quelques fonctions en plus que GLEW, mais pour l'instant je n'en ai pas besoin.

J'ai encore une fois utiliser apt pour installer GLEW:

```bash
sudo apt-get install libglew-dev
```

J'ai ensuite ajouter GLEW dans le makefile:

```makefile
LIBS := -lpthread -lglfw -lGL -lGLEW -lX11 -lXrandr -lXi -ldl -lXxf86vm -lXinerama -lXcursor
```

J'ai ensuite modifier le 1er projet pour utiliser GLEW. J'ai egalement pris le temps de rearaanger un peu le code pour me familiariser avec les fonctions de base d'GLFW et GLEW.

> Note: 
En voulant afficher la version d'OpenGL, j'ai vu qu'il utilisait **Mesa**. Après une petite recherche, j'ai vu que Mesa est une implémentation open source d'OpenGL. Après avoir vu cela j'ai regarder si j'avais installer les drivers Nvidia. Et en effet, je n'avais pas les drivers Nvidia installés. Pour l'instant, ce n'est pas un problème, car je vais utiliser OpenGL pour apprendre les bases. Mais au bout d'un moment, je vais les installer, car j'aimerais bien utiliser CUDA pour tester un peu.

Maintenant que j'ai installer GLEW, je vais pouvoir commencer à apprendre les bases de la programmation graphique.

### Utilisation moderne d'OpenGL

Auparavant, OpenGL utilisait une approche fixe pour dessiner des objets. C'est à dire que OpenGL fournissait des fonctions pour dessiner des formes géométriques de base (carré, triangle, etc). C'était assez limité, car on ne pouvait pas vraiment personnaliser les objets. 

C'est ce que nous avons utilisé dans notre premier programme :

```c
glBegin(GL_TRIANGLES);
glVertex2f( 0.0f, 0.5f);
glVertex2f(-0.5f,-0.5f);
glVertex2f( 0.5f,-0.5f);
glEnd();
```

> On appelle cela le **fixed pipeline** ou **legacy OpenGL**. C'est une approche assez ancienne et limitée.

Aujourd'hui, OpenGL utilise une approche plus moderne, appelée **programmable pipeline**. Cela signifie que nous devons écrire des shaders pour décrire comment les données doivent être traitées par le GPU.

Pour l'instant je ne vais pas entrer dans les détails du fonctionnement de la pipeline.

Pour dessiner notre triangle, nous avons besoin de 2 choses:

- Un [**vertex buffer**](#vertex-buffer) pour stocker les données des sommets.
- Un [**vertex shader**](#vertex-shader) pour décrire comment les données doivent être traitées par le GPU.

#### Vertex buffer

Un **vertex buffer** est juste un buffer, un block de données dans la memoire. La différence avec un array normal, c'est que les données sont stockées sur la carte graphique (VRAM) et non sur la RAM.

En gros, on va stocker les données des sommets (vertices) dans un espace mémoire dans la VRAM.

Ensuite, on va dire à OpenGL d'effectuer un **draw call**. C'est à dire qu'on va dire à OpenGL de lire les données du vertex buffer et de faire le rendu sur l'écran. Et c'est là que le vertex shader entre en jeu.

#### Vertex shader

Le **vertex shader** sert a décrire comment les données des sommets doivent être traitées par le GPU. Comment les sommets doivent être placés sur l'écran.

> Pour rappel, un shader est juste un programme qui s'exécute sur le GPU.

#### OpenGL est une machine d'état

OpenGL est une **machine d'état**. Cela signifie que OpenGL a un état interne qui est modifié par les appels de fonctions. Par exemple, si on appelle `glClearColor`, cela modifie la couleur de fond. Si on appelle `glClear`, cela efface le buffer de couleur avec la couleur de fond.

#### Vertex Attribut

Pour decrire les données des sommets, on utilise des **vertex attributs**. Un **vertex attribut** est une variable qui est associée à un vertex. Par exemple, on peut avoir un vertex attribut pour la position, un autre pour la couleur, etc.

Prenons l'exemple de notre triangle :

```c
// Un vertex est composé de 1 attribut (la position xy): C'est un vec2 (2 floats)
float positions[] = {
     0.0f,  0.5f,//un vertex
    -0.5f, -0.5f,//un autre vertex
     0.5f, -0.5f,//encore un autre vertex
};
```

Ici nous voyons que chaque vertex est composé de 2 floats (x et y). OpenGL ne voit qu'un bloc de données.
Il faut donc dire à OpenGL comment lire ces données. C'est là que les **vertex attributs** entrent en jeu.

Dans notre cas, nous avons un seul attribut (la position xy). Nous devons donc dire à OpenGL comment lire ces données.

```c
//Declare un uint pour stocker l'id du buffer
uint buffer_id;
//On génère un buffer et on stocke l'id dans buffer_id
glGenBuffers(1, &buffer_id);
//On bind le buffer (on dit à OpenGL que le buffer courant de type GL_ARRAY_BUFFER est buffer_id)
glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
//On copie les données du tableau positions dans le buffer
glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), triangle_vertices, GL_STATIC_DRAW);

//attributs
//On dit à OpenGL comment lire les données
//0: l'index de l'attribut (ici nous avons un seul attribut)
//2: le nombre de composantes par attribut (ici 2 car nous avons des vec2 (2 floats))
//GL_FLOAT: le type des données
//GL_FALSE: si les données doivent être normalisées (ce sont deja des floats)
//2 * sizeof(float): le stride (la distance entre 2 attributs) (en gros la taille d'un vertex)
//0: l'offset (de combien de bytes on doit décaler pour arriver à l'attribut) (ici 0 car on commence au début)
glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
glEnableVertexAttribArray(0); // on active l'attribut 0
```

#### Création des shaders

Pour créer un shader, on doit écrire un programme en **GLSL**. Ensuite, on doit le compiler et le lier à un **programme** OpenGL.

Un **programme** OpenGL est juste un ensemble de shaders. On peut avoir plusieurs shaders dans un programme.

Par exemple, pour notre triangle, nous avons besoin d'un **vertex shader** et d'un **fragment shader**. Nous allons donc créer Un programme, compiler les shaders, les attacher au programme et link le programme (un peu comme un programme C).

```c
unsigned int program = glCreateProgram();
unsigned int vs = compile_shader(GL_VERTEX_SHADER, vertex_src);
unsigned int fs = compile_shader(GL_FRAGMENT_SHADER, fragment_src);

glAttachShader(program, vs);
glAttachShader(program, fs);
glLinkProgram(program);
glValidateProgram(program);

glDeleteShader(vs);
glDeleteShader(fs);
```

Maintenant que nous avons nos fonctions pour gérer les shaders, nous devons les créer.

#### Code vertex shader

Le `#version 330 core` indique la version de GLSL que nous utilisons. Ici, nous utilisons la version 3.3. `core` signifie que nous n'utilisons que les fonctionnalités de base de GLSL.

`layout(location = 0)` indique que cet attribut est à l'index 0. Cela correspond à l'index que nous avons donné à `glVertexAttribPointer`.

Le `in` signifie que c'est un attribut d'entrée. C'est à dire que c'est un attribut qui est envoyé par le CPU au GPU.

Le type `vec4` est un vecteur de 4 floats. C'est un type de base en GLSL. Ici, nous utilisons un `vec4` pour la position. Bien que nous n'utilisions que 2 floats (x et y), nous devons utiliser un `vec4` car OpenGL attend un `vec4`.

Ensuite, nous avons la fonction `main`. C'est la fonction qui sera exécutée par le GPU pour chaque vertex. Ici, nous devons juste retourner la position du vertex (c'est ce que fait `gl_Position`).

```glsl
#version 330 core

layout(location = 0) in vec4 position;

void main()
{
    gl_Position = position;
}
```

#### Code fragment shader

Pour le fragment shader, c'est un peu plus simple. Nous n'avons pas d'attributs d'entrée. Nous devons juste retourner la couleur du pixel.

Pour cela, nous definissons une variable `color` de type `vec4`. Remarquez que contrairement au vertex shader, nous utilisons le mot-clé `out` pour indiquer que c'est une variable de sortie.(GPU -> CPU). Pour les variables de sortie, nous n'avons pas besoin de spécifier l'index (`layout(...)`)

```glsl
#version 330 core

out vec4 color;

void main()
{
    color = vec4(0.0, 1.0, 0.0, 1.0);
}
```

#### Utilisation des shaders

Pour utiliser les shaders, nous devons les activer. C'est à dire que nous devons dire à OpenGL d'utiliser le programme que nous avons créé.

```c
glUseProgram(program);
```

> `program` est l'id du programme que nous avons créé (avec `glCreateProgram`).

#### Index Buffer

La manière dont nous avons dessiné notre rectangle est assez simple. Nous avons juste donné les coordonnées des sommets. Cependant, ce n'est pas la manière la plus optimale de dessiner des objets. En effet, plusieurs sommets ont les mêmes coordonnées, il y a donc de la redondance.

```c
const float positions[] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.5f,  0.5f,

    -0.5f, -0.5f, //redondant
     0.5f,  0.5f, //redondant
    -0.5f,  0.5f,
};
```

> Ici ce n'est pas un problème, car nous n'avons que 6 sommets. Mais si nous avons des milliers de sommets comme la plupart des modèles 3D, il est bien plus efficace de réutiliser les sommets.


Pour éviter cela, nous pouvons utiliser un **index buffer**. Un **index buffer** est un buffer qui contient les indices des sommets. Cela permet de réutiliser les sommets.

En gros, au lieu de dire à OpenGL de dessiner chaque sommet, nous lui spécifions les indices des sommets à dessiner.

Pour cela nous allons donc créer un autre tableau qui contient les indices des sommets a dessiner dans l'ordre.

```c
//les vertex qui representet les positions des sommets
const float positions[] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.5f,  0.5f,
    -0.5f,  0.5f,
};

//les indices des sommets à dessiner
const unsigned int indices[] = {
    0, 1, 2, //1er  triangle
    0, 2, 3, //2eme triangle
};
```

> Comme on peut le voir, nous avons 4 sommets et 6 indices. Cela signifie que nous avons 2 triangles. Le premier triangle est composé des sommets 0, 1 et 2. Le deuxième triangle est composé des sommets 0, 2 et 3.

Pour utiliser ces indices, nous devons créer notre **index buffer**. Pour cela nous allons suivre les mêmes étapes que pour le **vertex buffer**:

1. On génère un buffer et stocke son id
2. On bind le buffer
3. On copie les données dans le buffer

La seule différence est que nous utilisons `GL_ELEMENT_ARRAY_BUFFER` au lieu de `GL_ARRAY_BUFFER`.

```c
unsigned int indice_buffer;
glGenBuffers(1, &indice_buffer);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indice_buffer);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, 2 * 3 * sizeof(*indices), indices, GL_STATIC_DRAW);
```

Ensuite, pour le draw call, nous devons utiliser `glDrawElements` au lieu de `glDrawArrays`.

```c
glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, NULL);
```

> `GL_TRIANGLES` indique que nous dessinons des triangles.  
`2 * 3` indique que nous avons 2 triangles.  
`GL_UNSIGNED_INT` indique que les indices sont des `unsigned int`.  
`NULL` indique que nous n'avons pas d'offset.

#### Uniforms

Les **uniforms** sont des variables qui sont envoyées par le CPU au GPU. Contrairement aux **vertex attributs**, les **uniforms** sont les mêmes pour tous les vertices.

Pour utiliser les **uniforms**, nous devons les déclarer des 2 côtés (CPU et GPU).

Nous allons commencer par le GPU, autrement dit le **fragment shader**.

```glsl
#version 330 core

out vec4 color;

uniform vec4 u_color;

void main()
{
    color = u_color;
}
```

> `uniform vec4 u_color;` déclare un uniform de type `vec4` appelé `u_color`. Nous renvoyons ensuite `u_color` comme couleur du pixel.

Ensuite, nous devons définir la valeur de `u_color` du côté CPU. Pour cela, nous devons d'abord obtenir l'emplacement de `u_color` dans le programme. Une fois récupéré, nous pouvons définir la valeur de `u_color`.

```c
int color_location = glGetUniformLocation(program, "u_color");
assert(color_location != -1);//on vérifie que l'uniform existe
glUniform4f(color_location, 0.2f, 0.3f, 0.8f, 1.0f);
```

> Il est important que le nom de l'uniform soit le même dans le shader et dans le code CPU. Sinon, `glGetUniformLocation` renverra `-1`.

> `glUniform4f` définit la valeur de l'uniform. Ici, nous définissons `u_color` à `(0.2f, 0.3f, 0.8f, 1.0f)`. Il existe plusieurs fonctions `glUniform` pour différents types de données ([voir documentation](https://docs.gl/gl4/glUniform)). Ici, nous utilisons `glUniform4f` car `u_color` est de type `vec4` (4 floats).



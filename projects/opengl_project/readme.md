# OpenGl - simulation de particules

## 📦Dependances

Le projet utilise les bibliothèques suivantes:

- [GLFW](https://www.glfw.org/)
- [GLEW](https://glew.sourceforge.net/)

Header only:

- [CGLM](https://github.com/recp/cglm)
- [STB](https://github.com/nothings/stb/blob/master/stb_image.h)

> Les bibliothèques **cglm** et **stb** sont incluses dans le projet(`src/gllib/vendor`)

Installation des dépendances avec apt:

```bash
#glfw
sudo apt install libglfw3
sudo apt install libglfw3-dev

#glew
sudo apt-get install libglew-dev
```

## 🛠️ Compilation

Pour compiler le projet, vous pouvez utiliser le makefile fourni:

```bash
make release && ./bin/release/app
```

> `make help` pour plus d'informations


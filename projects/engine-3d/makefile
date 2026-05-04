# Description: Makefile pour la compilation d'un projet C
# Date: 2025-01-27
# Auteur: Ruben Wihler
# License: GPL v3
# Version: 1.0
# Fonctionnalités: Compilation en mode debug et release, nettoyage du projet
# Usage: make [all|debug|release|clean|help]
#
# ---------- Fonctionnement ----------: 
#
#   Le makefile scanne les fichiers sources dans le répertoire src et les compile en fichiers objets dans le répertoire obj.
#   Les fichiers objets sont ensuite liés pour créer l'exécutable dans le répertoire bin (debug/$NAME ou release/$NAME).
#   Les options de compilation sont définies dans les variables CFLAGS, DEBUG_FLAGS et RELEASE_FLAGS.
#   Les librairies sont définies dans la variable LIBS.
#   Les répertoires sont définis dans les variables SRC_DIR, OBJ_DIR et BIN_DIR.
#   Les fichiers sources sont recherchés récursivement dans le répertoire src.
#   Les fichiers objets sont générés à partir des fichiers sources.
#   regles:
#   	- La règle all compile en mode debug et release.
#   	- La règle debug compile en mode debug et lance le débugueur gdb.
#   	- La règle release compile en mode release.
#   	- La règle clean supprime les fichiers objets et les exécutables.
#   	- La règle help affiche l'aide.
#
# ---------- Aide pour makefile ----------
#
# 	$@ : nom de la cible/regle
# 	$< : nom de la première dépendance
# 	$^ : liste des dépendances
# 	$* : nom du fichier sans suffixe
# 	$% : nom du fichier d'archive
# 	$? : liste des dépendances plus récentes que la cible
#
# 	variables:
# 	VAR=valeur : définition de la variable VAR
# 	$(VAR) : valeur de la variable VAR
#
# 	fonctions:
# 	$(patsubst motif,remplacement,texte) : remplace les motifs par le remplacement dans le texte
# 	$(wildcard motif) : recherche de fichiers
# 	$(shell commande) : exécution de commande shell

# Variables
CC := gcc
DEBUGER := gdb
CFLAGS := -Wall -Wextra -MMD -MP
SANITIZE_FLAGS := -fsanitize=undefined,address,leak -fno-omit-frame-pointer
DEBUG_FLAGS := -g -O0 -DDEBUG $(SANITIZE_FLAGS)
RELEASE_FLAGS := -O3
NAME := particle

# Librairies
LIBS := -lm -lpthread -lglfw -lGL -lX11 -lXrandr -lXi -ldl -lXxf86vm -lXinerama -lXcursor

#definition des preprocesseurs
CFLAGS += 

# Répertoires
SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin
DEBUG_BIN := $(BIN_DIR)/debug/$(NAME)
RELEASE_BIN := $(BIN_DIR)/release/$(NAME)

# Recherche récursive des fichiers source et headers
# (excluant les fichier cachés .*)
SRC_FILES := $(shell find $(SRC_DIR) -type f -name '*.c' -not -path '*/\.*')
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Règles principales
.PHONY: all debug release clean help

help:
	@echo "Usage: make [all|debug|release|clean]"

all: debug release

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_BIN)
	$(DEBUGER) $(DEBUG_BIN)

release: CFLAGS += $(RELEASE_FLAGS)
release: $(RELEASE_BIN)

# Compilation des exécutables
$(DEBUG_BIN): $(OBJ_FILES)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(RELEASE_BIN): $(OBJ_FILES)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

# Compilation des fichiers objets
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

# Nettoyage
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

-include $(OBJ_FILES:.o=.d)

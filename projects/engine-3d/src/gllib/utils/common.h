#pragma once

#include <stddef.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
 * @brief Récupère la structure parente à partir d'un pointeur vers un de ses membres.
 * * @param ptr Le pointeur vers le membre.
 * @param type Le type de la structure parente.
 * @param member Le nom du membre dans la structure parente.
 */
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

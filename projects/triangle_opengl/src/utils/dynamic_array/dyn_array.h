#pragma once

#include <stddef.h>

typedef struct _dyn_array_t dyn_array_t;

/// @brief Crée un tableau dynamique
/// @param capacity Capacité initiale du tableau
/// @param element_size Taille en octets d'un élément du tableau
/// @return Un pointeur vers le tableau dynamique
/// @note Le tableau doit être détruit avec dyn_array_destroy
/// @see dyn_array_destroy
dyn_array_t* dyn_array_create(const size_t capacity, const size_t element_size);

/// @brief Détruit un tableau dynamique
/// @param arr Pointeur vers le tableau dynamique à détruire
/// @note Le tableau ne doit plus être utilisé après cet appel
/// @note Si les éléments du tableau ont des ressources à libérer, utilisez dyn_array_destroy_complex
/// @see dyn_array_destory_complex
void dyn_array_destroy(dyn_array_t *arr);

/// @brief Détruit un tableau dynamique qui contient des éléments qui ont des ressources à libérer
/// @param arr Pointeur vers le tableau dynamique à détruire
/// @param destroy_element Pointeur vers une fonction qui détruit un élément du tableau
/// @note Le tableau ne doit plus être utilisé après cet appel
/// @see dyn_array_destroy
void dyn_array_destroy_complex(dyn_array_t *arr, void (*destroy_element)(void*));

/// @brief Retourne le nombre d'éléments du tableau
/// @param arr Pointeur vers le tableau dynamique
/// @return Le nombre d'éléments du tableau
size_t dyn_array_count(dyn_array_t *arr);

/// @brief Retourne la taille en octets d'un élément du tableau
/// @param arr Pointeur vers le tableau dynamique
/// @return La taille en octets d'un seul élément du tableau
size_t dyn_array_size(dyn_array_t *arr);

/// @brief Retourne la capacité du tableau
/// @param arr Pointeur vers le tableau dynamique
/// @return La capacité du tableau (le nombre d'éléments que le tableau peut contenir sans être redimensionné)
/// @note La capacité est toujours supérieure ou égale au nombre d'éléments du tableau
size_t dyn_array_capacity(dyn_array_t *arr);

/// @brief Retourne un pointeur vers l'élément à l'index donné
/// @param arr Pointeur vers le tableau dynamique
/// @param index Index de l'élément à récupérer
/// @return Un pointeur vers l'élément à l'index donné
/// @note si l'index est out of bounds, NULL est retourné et un message d'erreur est affiché sur stderr
void* dyn_array_get(dyn_array_t *arr, const size_t index);

/// @brief Modifie la valeur de l'élément à l'index donné
/// @param arr Pointeur vers le tableau dynamique
/// @param index Index de l'élément à modifier
/// @param value_ptr Pointeur vers la nouvelle valeur de l'élément
/// @note si l'index est out of bounds, un message d'erreur est affiché sur stderr
void dyn_array_set(dyn_array_t *arr, const size_t index, const void* value_ptr);

/// @brief Réserve de la mémoire pour un certain nombre d'éléments
/// @param arr Pointeur vers le tableau dynamique
/// @param count Nombre d'éléments à réserver
/// @return Un pointeur vers la première case réservée
/// @note si la capacité est insuffisante, le tableau est redimensionné à la capacité MINIMAL nécessaire
/// @note chaque élément reservé augmente arr->count (comme si c'était des éléments ajoutés)
void* dyn_array_reserve(dyn_array_t *arr, const size_t count);

/// @brief Ajoute une copie de l'élément à la fin du tableau
/// @param arr Pointeur vers le tableau dynamique
/// @param value_ptr Pointeur vers la valeur à ajouter
/// @note si la capacité du tableau est insuffisante, le tableau est redimensionné
/// @note Si vous voulez ajouter un grand nombre d'éléments ou simplement eviter une copie inutile, utilisez dyn_array_reserve.
/// @see dyn_array_reserve
void dyn_array_push_back(dyn_array_t *arr, const void* value_ptr);

/// @brief Supprime le dernier élément du tableau
/// @param arr Pointeur vers le tableau dynamique
/// @note si le tableau est vide, un message d'erreur est affiché sur stderr
/// @note si le nombre d'éléments est inférieur à la moitié de la capacité, le tableau est redimensionné (/2)
/// @note aucun message d'erreur n'est affiché si l'array est vide
void dyn_array_pop_back(dyn_array_t *arr);

/// @brief Insère une copie de l'élément à l'index donné
/// @param arr Pointeur vers le tableau dynamique
/// @param index Index de l'élément à insérer
/// @param value_ptr Pointeur vers la valeur à insérer
/// @note si l'index est out of bounds, un message d'erreur est affiché sur stderr
/// @note si la capacité du tableau est insuffisante, le tableau est redimensionné
/// @note les éléments après l'index sont décalés d'une case vers la droite
/// @note si l'index est égal à la taille du tableau, l'élément est ajouté à la fin
void dyn_array_insert(dyn_array_t *arr, const size_t index, const void* value_ptr);

/// @brief Supprime l'élément à l'index donné
/// @param arr Pointeur vers le tableau dynamique
/// @param index Index de l'élément à supprimer
/// @note si l'index est out of bounds, un message d'erreur est affiché sur stderr
/// @note les éléments après l'index sont décalés d'une case vers la gauche
/// @note si le nombre d'éléments est inférieur à la moitié de la capacité, le tableau est redimensionné (/2)
/// @note si le tableau est vide, un message d'erreur est affiché sur stderr
void dyn_array_remove(dyn_array_t *arr, const size_t index);

/// @brief Vide le tableau
/// @param arr Pointeur vers le tableau dynamique
/// @note la capacité du tableau reste inchangée
/// @note les éléments ne sont pas détruits
/// @note le nombre d'éléments est mis à 0
/// @note le tableau peut être réutilisé sans redimensionnement
void dyn_array_clear(dyn_array_t *arr);

/// @brief Redimensionne le tableau à la taille donnée
/// @param arr Pointeur vers le tableau dynamique
void dyn_array_shrink_to_fit(dyn_array_t *arr);

/// @brief Redimensionne le tableau à la taille donnée
/// @param arr Pointeur vers le tableau dynamique
/// @param new_size Nouvelle taille du tableau
/// @note si la nouvelle taille est plus grande que l'ancienne, les nouvelles cases sont initialisées à 0
/// @note si la taille est inférieure au nombre d'éléments, la taille est ajustée au nombre d'éléments
void dyn_array_resize(dyn_array_t *arr, const size_t new_size);

/// @brief Retourn vrai si le tableau est vide
/// @param arr Pointeur vers le tableau dynamique
/// @return true si le tableau est vide, false sinon
bool dyn_array_empty(dyn_array_t *arr);

/// @brief Affiche le contenu du tableau
/// @param arr Pointeur vers le tableau dynamique
/// @param print Pointeur vers une fonction qui affiche un élément du tableau
/// @note print ne doit pas être NULL sinon SEGFAULT aie aie aie
void dyn_array_print(dyn_array_t *arr, void (*print)(void*));

/// @brief Retourne la taille en octets de la structure dyn_array_t
/// @return La taille en octets de la structure dyn_array_t
size_t dyn_array_sizeof();

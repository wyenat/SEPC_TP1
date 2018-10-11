/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <assert.h>
#include "mem.h"
#include "mem_internals.h"

void* next_ad(void* pointeur){
    /* */
    void **head = (void **) pointeur;
    return *head;


}

void* take_head_arena(){
    /* Retourne le premier element de la chunkpool et le supprime */
    void* pointeur = arena.chunkpool;
    arena.chunkpool = next_ad(pointeur);
    return pointeur;
}

void construire_chunkpool(){
    unsigned long size = mem_realloc_small();
    void* element = arena.chunkpool;
    int compteur = 0;
    while (size-compteur*96 > 0){
        *((long *) (element)) = (*((long *)(element)) + 12);
        element = next_ad(element);
        compteur++;
    }
    *((long *) (element)) = *((long *) (element));
}

void *
emalloc_small(unsigned long size)
{
    /* Enlève le premier élément de la
     * liste chainée pointée par arena.chunkpool, appelle la fonction de marquage en lui donnant
     * l’adresse de l’élément, sa taille (CHUNKSIZE) et le type (SMALL_KIND) puis renvoie le résultat
     * de la fonction de marquage
     */
    if (arena.chunkpool == 0){
        construire_chunkpool();
    }
    void* premier_ele_arena_chunkpool = take_head_arena();
    void *pointeur =  mark_memarea_and_get_user_ptr(premier_ele_arena_chunkpool, CHUNKSIZE, SMALL_KIND);
    return (void *) pointeur;
}

void efree_small(Alloc a) {
    /* ecrire votre code ici */
}

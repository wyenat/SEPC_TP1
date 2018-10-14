/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <stdint.h>
#include <assert.h>
#include "mem.h"
#include "mem_internals.h"

unsigned int puiss2(unsigned long size) {
    unsigned int p=0;
    size = size -1; // allocation start in 0
    while(size) {  // get the largest bit
	p++;
	size >>= 1;
    }
    if (size > (1 << p))
	p++;
    return p;
}


void * decoupe_recursive(unsigned long puissance2, unsigned long size) {
  /* découpage récursif du bloc de taille 2^puissance2
  * jusqu'à obtenir un bloc de taille 2^size (+ autres blocs)
  * ATTENTION : bien insérer tous les blocs finaux dans la TZL */

  //TODO;
}


void * decoupe_bloc_taille_superieure(unsigned long size) {
  unsigned long puissance2 = ; // indice correspondant à la taille suivant size
  while (puissance2 < FIRST_ALLOC_MEDIUM_EXPOSANT + arena.medium_next_exponant
          && arena.TZL[puissance2] == NULL) {
  /* tant que pas de bloc dispo dans les tailles supérieures à la taille souhaitée */
        puissance2++;
  }
  if (puissance2 == FIRST_ALLOC_MEDIUM_EXPOSANT + arena.medium_next_exponant) {
    mem_realloc_medium();
  }
  /* sinon, on a trouvé un bloc dispo de taille 2^puissance2
  * Il reste à le découper récursivt pour obtenir un bloc de taille size */
  decoupe_recursive(puissance2, size);
  //TODO
}


void *
emalloc_medium(unsigned long size)
{
    assert(size < LARGEALLOC);
    assert(size > SMALLALLOC);
    /* ecrire votre code ici */
    /* Calcul de l'indice de la TZL à sélectionner */
    //TODO;
    /* voir si un bloc de la bonne taille est dispo :
    * on regarde TZL[i] : il pointe vers la tête de liste des blocs
    * de taille 2^i */
    if (arena.TZL[indice] == NULL) { /* si pas de bloc dispo */
      //TODO;
      decoupe_bloc_taille_superieure(size);
      return;
    }
    /* si bloc dispo : */
    void* bloc = arena.TZL[indice];
    mark_memarea_and_get_user_ptr(bloc, size, MEDIUM_KIND);

    return (void *) 0;
}


void efree_medium(Alloc a) {
    /* ecrire votre code ici */
}

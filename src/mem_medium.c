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


void * adresse_buddy(void* adresse_bloc, unsigned long taille_bloc) {
  unsigned long adresse = ((unsigned long) adresse_bloc) ^ taille_bloc;
  return (void*)adresse;
}

void * decoupe_recursive(void* adresse_debut_bloc, unsigned long indice_TZL_courant, unsigned long size) {
  /* découpage récursif du bloc de taille 2^indice_TZL_courant
  * jusqu'à obtenir un bloc de taille size (+ autres blocs)
  * ATTENTION : bien insérer tous les blocs finaux dans la TZL */

  if ((1 << indice_TZL_courant) == size) {
    arena.TZL[indice_TZL_courant] = adresse_buddy(adresse_debut_bloc, size); // insertion du buddy dans la TZL
    return adresse_debut_bloc; // renvoie l'adresse du bloc, qui est de la bonne taille
  }

  if ((1 << indice_TZL_courant) > size) {
    void* adr2 = adresse_buddy(adresse_debut_bloc, 1 << indice_TZL_courant);
    arena.TZL[indice_TZL_courant] = adr2;
    indice_TZL_courant--;
    decoupe_recursive(adresse_debut_bloc, indice_TZL_courant, size);
  }

}


void * decoupe_bloc_taille_superieure(unsigned long size) {
  unsigned int puissance2 = puiss2(size); // indice correspondant à la taille suivant size
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
  void* adresse_bloc_init = arena.TZL[puissance2];
  decoupe_recursive(adresse_bloc_init, puissance2, size);
}


void *
emalloc_medium(unsigned long size)
{
    assert(size < LARGEALLOC);
    assert(size > SMALLALLOC);
    /* ecrire votre code ici */
    /* Calcul de l'indice de la TZL à sélectionner */
    unsigned int indice = puiss2(size);
    /* voir si un bloc de la bonne taille est dispo :
    * on regarde TZL[i] : il pointe vers la tête de liste des blocs
    * de taille 2^i */
    if (arena.TZL[indice] == NULL) { /* si pas de bloc dispo */
      decoupe_bloc_taille_superieure(size);
    }
    /* si bloc dispo : */
    void* bloc = arena.TZL[indice];
    mark_memarea_and_get_user_ptr(bloc, size, MEDIUM_KIND);

    return (void *) 0;
}


void efree_medium(Alloc a) {
    /* ecrire votre code ici */
}

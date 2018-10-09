/******************************************************
 * Copyright Grégory Mounié 2018                      *
 * This code is distributed under the GLPv3+ licence. *
 * Ce code est distribué sous la licence GPLv3+.      *
 ******************************************************/

#include <sys/mman.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "mem.h"
#include "mem_internals.h"
#include <assert.h>


unsigned long knuth_mmix_one_round(unsigned long in)
{  /* Calcule le nombre magic */
    return in * 6364136223846793005UL % 1442695040888963407UL;
}


void *mark_memarea_and_get_user_ptr(void *ptr, unsigned long size, MemKind k)
{
    /* Écrit le marquage dans les 16 premiers et les 16 derniers octets du bloc pointé par ptr et
     * d’une longueur de size octets. Elle renvoie l’adresse de la zone utilisable par l’utilisateur,
     * 16 octets après ptr */
    unsigned long magical_number = knuth_mmix_one_round((unsigned long) ptr);
    magical_number &= ~(0b11UL);
    magical_number |= k;
    memcpy(ptr, &size, 8);
    memcpy(ptr+8, &magical_number, 8);
    memcpy(ptr+size+16+DELTA, &magical_number, 8);
    memcpy(ptr+size+DELTA+24, &size, 8);
    return ptr+16;
}

Alloc mark_check_and_get_alloc(void *ptr)
{
    unsigned long size;
    memcpy(&size, ptr-16, 8);
    uint8_t memkind;
    memcpy(&memkind, ptr-1, 1);
    memkind &= 00000011;
    MemKind k = memkind;
    unsigned long valeur_magique = (knuth_mmix_one_round((unsigned long) (ptr-16)) & ~(0b11UL)) | k;
    unsigned long actual_val1;
    unsigned long actual_val2;
    memcpy(&actual_val1, ptr-8, 8);
    memcpy(&actual_val2, ptr+size+DELTA, 8);
    assert(actual_val1 == valeur_magique);
    assert(actual_val1 == actual_val2);
    Alloc a = {ptr, k, size};
    return a;
}


unsigned long mem_realloc_small() {
    assert(arena.chunkpool == 0);
    unsigned long size = (FIRST_ALLOC_SMALL << arena.small_next_exponant);
    arena.chunkpool = mmap(0,
			   size,
			   PROT_READ | PROT_WRITE | PROT_EXEC,
			   MAP_PRIVATE | MAP_ANONYMOUS,
			   -1,
			   0);
    if (arena.chunkpool == MAP_FAILED)
	handle_fatalError("small realloc");
    arena.small_next_exponant++;
    return size;
}

unsigned long mem_realloc_medium() {
    uint32_t indice = FIRST_ALLOC_MEDIUM_EXPOSANT + arena.medium_next_exponant;
    assert(arena.TZL[indice] == 0);
    unsigned long size = (FIRST_ALLOC_MEDIUM << arena.medium_next_exponant);
    assert( size == (1 << indice));
    arena.TZL[indice] = mmap(0,
			     size*2, // twice the size to allign
			     PROT_READ | PROT_WRITE | PROT_EXEC,
			     MAP_PRIVATE | MAP_ANONYMOUS,
			     -1,
			     0);
    if (arena.TZL[indice] == MAP_FAILED)
	handle_fatalError("medium realloc");
    // align allocation to a multiple of the size
    // for buddy algo
    arena.TZL[indice] += (size - (((intptr_t)arena.TZL[indice]) % size));
    arena.medium_next_exponant++;
    return size; // lie on allocation size, but never free
}


// used for test in buddy algo
unsigned int nb_TZL_entries() {
    int nb = 0;

    for(int i=0; i < TZL_SIZE; i++)
	if ( arena.TZL[i] )
	    nb ++;

    return nb;
}

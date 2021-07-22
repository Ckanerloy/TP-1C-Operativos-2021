#ifndef SWAP_H_
#define SWAP_H_

#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <sys/time.h>
#include "paginacion.h"




void* area_swap;
void* contenido_swap;

t_list* paginas_swap;

int32_t cantidad_paginas_swap;

int archivo_swap;


void inicializar_swap(void);
void* iniciar_area_swap(void);
int obtener_marco_libre(void);

int32_t aplicar_LRU(void);


#endif /* SWAP_H_ */

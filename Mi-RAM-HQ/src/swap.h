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
sem_t* sem_swap;
sem_t* sem_lru;
sem_t* sem_clock;

void inicializar_swap(void);
void* iniciar_area_swap(void);
void liberar_frame_swap(int32_t numero_frame);
int32_t obtener_frame_libre_swap(void);

int32_t frame_disponible_segun_algoritmo(void);
int32_t aplicar_LRU(void);
int32_t aplicar_CLOCK(void);

int guardar_pagina_en_swap(void* buffer, t_pagina* pagina, int32_t espacio_ocupado);
void* recuperar_en_swap(int32_t numero_pagina, int32_t* espacio_usado);
int32_t buscar_frame_por_pagina(int32_t numero_pagina);
void* leer_frame_en_swap(int32_t numero_frame, int32_t espacio_ocupado);

#endif /* SWAP_H_ */

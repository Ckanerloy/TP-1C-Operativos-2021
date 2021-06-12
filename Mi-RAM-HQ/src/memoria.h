#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "Mi-RAM.h"
#include "utils/estructuras.h"

void* MEMORIA_PRINCIPAL;
void* AREA_SWAP;

char esquema_elegido;

t_segmentos* segmento_patota;
t_paginas* pagina_patota;

//tablas_segmenos_patotas* lista_de_patotas;

void elegir_esquema_de_memoria(char* ESQUEMA);


void guardar_patota(t_pcb* patota, t_tarea** tareas, uint32_t cant_tareas, uint32_t cantidad_tripulantes);



#endif /* MEMORIA_H_ */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "Mi-RAM.h"
#include "utils/estructuras.h"

void* MEMORIA_PRINCIPAL;
void* AREA_SWAP;


//tablas_segmenos_patotas* lista_de_patotas;


void inicializar_tabla_segmentos_de_patota(t_segmentos_patota* segmento_patota);
void inicializar_tabla_segmentos_de_tripulante(t_segmentos_tripulantes* segmento_tripulante);
void inicializar_tabla_segmentos_de_tarea(t_segmentos_tarea* segmento_tarea);



#endif /* MEMORIA_H_ */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "Mi-RAM.h"
#include "utils/estructuras.h"

void* MEMORIA_PRINCIPAL;
void* AREA_SWAP;

t_segmentos_patota* segmento_patota;
t_paginas_patota* pagina_patota;

//tablas_segmenos_patotas* lista_de_patotas;

void elegir_esquema_de_memoria(char* ESQUEMA);

void inicializar_tabla_segmentos_de_patota(t_segmentos_patota* segmento_patota);
void inicializar_tabla_segmentos_de_tripulante(t_segmentos_tripulantes* segmento_tripulante);
void inicializar_tabla_segmentos_de_tarea(t_segmentos_tarea* segmento_tarea);

void inicializar_tabla_paginas_de_patota(t_paginas_patota* pagina_tarea);
void inicializar_tabla_paginas_de_tripulante(t_paginas_tripulantes* pagina_tripulante);
void inicializar_tabla_paginas_de_tareas(t_paginas_tarea* pagina_tarea);


t_segmentos_patota* crear_segmento_patota(t_segmentos_patota* tabla_segmentos_patota, t_pcb* patota_recibida, t_tarea** tarea_patota, uint32_t cant_tareas, uint32_t cant_tripulantes);
void crear_segmento_tarea(t_segmentos_tarea* tabla_segmento_tarea, t_tarea** tarea_patota);
void crear_segmento_tripulante(t_segmentos_patota* tabla_segmento_patota, t_tcb* tripulante);

#endif /* MEMORIA_H_ */

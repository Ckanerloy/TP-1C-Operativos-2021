#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "Mi-RAM.h"
#include "utils/estructuras.h"

t_list* tablas_segmentos;
t_list* segmentos_libres;
t_list* segmentos_ocupados;

t_list* tablas_paginas;

// Estructuras para Mi RAM HQ
// SEGMENTACION
typedef struct segmento
{
	uint32_t numero_de_segmento;		// Esta tabla va a tener el numero de segmento
	uint32_t inicio;					// Direccion fisica de donde empieza el segmento
	uint32_t tamanio_segmento;			// Tamanio total del segmento
	tipo_segmento tipo_estructura;		// Si es una PATOTA, TAREAS, TRIPULANTE
	estado estado_segemento;			// Si esta LIBRE u OCUPADO
} t_segmento;


typedef struct	tabla_segmentos_patota
{
	t_pcb* patota;
	char* ids_tripus;
	t_list* segmentos;
} t_tabla_segmentos_patota; //una por patota



// PAGINACION
typedef struct pagina
{
	int32_t numero_de_marco;
	uint32_t numero_de_pagina;
	int32_t ocupado;

} t_pagina;

typedef struct tabla_paginas_patota{
	t_list* paginas;

}t_tabla_paginas_patota;


bool memoria_igual_o_mas_grande(t_segmento* elemento, uint32_t tamanio_buscado);
bool menor_a_mayor(t_segmento* segmento, t_segmento* segmento_siguiente);
t_segmento* obtener_segmento_libre(uint32_t tamanio_buscado);

t_tabla_segmentos_patota* crear_tabla_segmentos(t_pcb* nueva_patota);
void elegir_esquema_de_memoria(char* ESQUEMA);
criterio_seleccion elegir_criterio_seleccion(char* criterio);
algoritmo_reemplazo elegir_algoritmo_reemplazo(char* algoritmo);



t_segmento* crear_segmento(void* estructura, tipo_segmento tipo_estructura);
t_tabla_segmentos_patota* buscar_tabla_de_patota(t_pcb* patota_buscada);
t_segmento* buscar_por_tipo_de_segmento(t_list* tabla, tipo_segmento tipo_de_segmento);

void* traducir_segmento(t_segmento* segmento_a_traducir);
t_pcb* encontrar_patota(t_segmento* segmento);
t_list* encontrar_tarea(t_segmento* segmento);
t_tcb* encontrar_tripulante(t_segmento* segmento);

uint32_t administrar_guardar_patota(t_pcb* nueva_patota);
uint32_t administrar_guardar_tareas(t_list* tareas_de_la_patota);
uint32_t administrar_guardar_tripulante(t_tcb* nuevo_tripulante);

#endif /* MEMORIA_H_ */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "Mi-RAM.h"
#include "utils/estructuras.h"

// Segmentacion
t_list* tablas_segmentos;
t_list* segmentos;

// Paginacion
t_list* tablas_paginas;

// Estructuras para Mi RAM HQ
// SEGMENTACION
typedef struct segmento {
	uint32_t numero_de_segmento;		// Esta tabla va a tener el numero de segmento
	uint32_t inicio;					// Direccion fisica de donde empieza el segmento
	uint32_t tamanio_segmento;			// Tamanio total del segmento
	tipo_segmento tipo_segmento;		// Si es una PATOTA, TAREAS, TRIPULANTE
	estado estado_segmento;				// Si esta LIBRE u OCUPADO
	uint32_t id_segmento;				// ID de Tripulante / Id de Patota / NULL por tarea
} t_segmento;


typedef struct {
	uint32_t numero_de_segmento;
	void* estructura;
} id_segmento;


typedef struct	tabla_segmentos_patota
{
	t_pcb* patota;
	t_list* segmentos;
} t_tabla_segmentos_patota;



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

// Eleccion de algoritmos para la memoria
void elegir_esquema_de_memoria(char* ESQUEMA);
criterio_seleccion elegir_criterio_seleccion(char* criterio);
algoritmo_reemplazo elegir_algoritmo_reemplazo(char* algoritmo);


t_tabla_segmentos_patota* crear_tabla_segmentos(t_pcb* nueva_patota);

t_segmento* registrar_segmento(void* estructura, tipo_segmento tipo_segmento, uint32_t tamanio_estructura);
t_segmento* asignar_segmento(t_segmento* segmento_libre, tipo_segmento tipo_segmento, void* estructura, uint32_t tamanio_estructura);
t_segmento* crear_primer_segmento(void);
t_segmento* obtener_segmento_libre(uint32_t tamanio_buscado);
t_list* segmentos_libres(void);

void actualizar_segmento(void* estructura, tipo_segmento tipo_segmento, t_segmento* segmento);
t_segmento* crear_segmento(void* estructura, tipo_segmento tipo_estructura);

t_segmento* administrar_guardar_segmento(void* estructura, tipo_segmento tipo_segmento, uint32_t tamanio);

t_tabla_segmentos_patota* buscar_tabla_de_patota(uint32_t id_patota_buscada);
t_segmento* buscar_por_id_tripulante(t_list* segmentos, tipo_segmento tipo_de_segmento, uint32_t valor);

bool validar_existencia_segmento_libre_suficiente(uint32_t tamanio_buscado);


void* traducir_segmento(t_segmento* segmento_a_traducir);
t_pcb* encontrar_patota(t_segmento* segmento);
t_list* encontrar_tarea(t_segmento* segmento);
t_tcb* encontrar_tripulante(t_segmento* segmento);

void guardar_patota(t_pcb* nueva_patota);
void recuperar_patota(t_pcb* nueva_patota);
void guardar_tareas(t_list* tareas_de_la_patota);
void recuperar_tareas(t_list* tareas_de_la_patota);
void guardar_tripulante(t_tcb* nuevo_tripulante);
void recuperar_tripulante(t_tcb* nuevo_tripulante);

#endif /* MEMORIA_H_ */

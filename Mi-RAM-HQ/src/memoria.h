#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "Mi-RAM.h"
#include "utils/estructuras.h"



// Estructuras para Mi RAM HQ
// SEGMENTACION
typedef struct segmento {
	uint32_t numero_de_segmento;		// Esta tabla va a tener el numero de segmento
	uint32_t inicio;					// Direccion fisica de donde empieza el segmento
	uint32_t tamanio_segmento;			// Tamanio total del segmento
	tipo_segmento tipo_segmento;		// Si es una PATOTA, TAREAS, TRIPULANTE
	estado estado_segmento;				// Si esta LIBRE u OCUPADO
	uint32_t id_segmento;				// ID de Tripulante si es TRIPULANTE/ Id de Patota si es PATOTA / Cantidad de tareas si es TAREA
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


// Segmentacion
t_list* tablas_segmentos;
t_list* segmentos;

// Paginacion
t_list* tablas_paginas;




// Eleccion de algoritmos para la memoria
void elegir_esquema_de_memoria(char* ESQUEMA);
criterio_seleccion elegir_criterio_seleccion(char* criterio);
algoritmo_reemplazo elegir_algoritmo_reemplazo(char* algoritmo);

// Inicio en Segmentación
t_tabla_segmentos_patota* crear_tabla_segmentos(t_pcb* nueva_patota);
t_segmento* crear_segmento(void* estructura, tipo_segmento tipo_estructura);
t_segmento* crear_segmento_libre(uint32_t inicio_segmento, uint32_t tamanio_libre_segmento);
void liberar_segmento(t_segmento* segmento_a_liberar) ;
void verificar_compactacion(void);
void compactar(void);

// Guardar en Memoria
void guardar_patota(t_pcb* nueva_patota);
void guardar_tareas(t_list* tareas_de_la_patota);
void guardar_tripulante(t_tcb* nuevo_tripulante);

// Actualizar Segmento en Memoria
void actualizar_segmento(void* estructura, tipo_segmento tipo_segmento, t_segmento* segmento);
void actualizar_patota(t_pcb* patota, uint32_t inicio_segmento);
void actualizar_tareas(t_list* tareas_patota, uint32_t inicio_segmento);
void actualizar_tripulante(t_tcb* tripulante, uint32_t inicio_segmento);

// Obtener en Memoria
void* obtener_contenido_de_segmento(t_segmento* segmento_a_traducir);
t_pcb* encontrar_patota(t_segmento* segmento);
t_list* encontrar_tarea(t_segmento* segmento);
t_tcb* encontrar_tripulante(t_segmento* segmento);

t_tabla_segmentos_patota* buscar_tabla_de_patota(uint32_t id_patota_buscada);
t_segmento* buscar_por_id(t_list* segmentos, tipo_segmento tipo_de_segmento, uint32_t valor);
t_tarea* buscar_proxima_tarea_del_tripulante(t_list* segmentos, tipo_segmento tipo_de_segmento, uint32_t id_proxima_tarea_del_tripu);

int obtener_indice(t_list* lista, void* valor);
t_segmento* obtener_segmento_libre(uint32_t tamanio_buscado);
t_list* segmentos_libres(void);
t_list* segmentos_ocupados(void);
bool validar_existencia_segmento_libre_suficiente(uint32_t tamanio_buscado);
bool menor_a_mayor_segun_inicio(void* segmento, void* segmento_siguiente);
bool menor_a_mayor_segun_tamanio(void* segmento, void* segmento_siguiente);
bool menor_a_mayor_por_segmento(void* segmento, void* segmento_siguiente);
bool menor_a_mayor_por_pid(void* segmento, void* segmento_siguiente);


#endif /* MEMORIA_H_ */

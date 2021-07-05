#ifndef SEGMENTACION_H_
#define SEGMENTACION_H_

#include "Mi-RAM.h"
#include "utils/estructuras.h"


typedef struct segmento {
	uint32_t numero_de_segmento;		// Esta tabla va a tener el numero de segmento
	uint32_t inicio;					// Direccion fisica de donde empieza el segmento
	uint32_t tamanio_segmento;			// Tamanio total del segmento
	tipo_estructura tipo_segmento;		// Si es una PATOTA, TAREAS, TRIPULANTE
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


t_list* tablas_segmentos;
t_list* segmentos;


// Inicio en Segmentación
t_tabla_segmentos_patota* crear_tabla_segmentos(t_pcb* nueva_patota);
t_segmento* crear_segmento(void* estructura, tipo_estructura tipo_segmento);
t_segmento* crear_segmento_libre(uint32_t inicio_segmento, uint32_t tamanio_libre_segmento);
void liberar_segmento(t_segmento* segmento_a_liberar) ;
void verificar_compactacion(void);
void compactar(void);

// Guardar en Memoria
t_segmento* administrar_guardar_segmento(void* estructura, tipo_estructura tipo_segmento, uint32_t tamanio);
void guardar_patota(t_pcb* nueva_patota);
void guardar_tareas(t_list* tareas_de_la_patota);
void guardar_tripulante(t_tcb* nuevo_tripulante);

// Actualizar Segmento en Memoria
void actualizar_segmento(void* estructura, tipo_estructura tipo_segmento, t_segmento* segmento);
void actualizar_patota(t_pcb* patota, uint32_t inicio_segmento);
void actualizar_tareas(t_list* tareas_patota, uint32_t inicio_segmento);
void actualizar_tripulante(t_tcb* tripulante, uint32_t inicio_segmento);

// Obtener en Memoria
void* obtener_contenido_de_segmento(t_segmento* segmento_a_traducir);
t_pcb* encontrar_patota(t_segmento* segmento);
t_list* encontrar_tarea(t_segmento* segmento);
t_tcb* encontrar_tripulante(t_segmento* segmento);

// Búsqueda de Segmento
t_tabla_segmentos_patota* buscar_tabla_de_patota(uint32_t id_patota_buscada);
t_segmento* buscar_por_id(t_list* segmentos, tipo_estructura tipo_de_segmento, uint32_t valor);
t_tarea* buscar_proxima_tarea_del_tripulante(t_list* segmentos, tipo_estructura tipo_de_segmento, uint32_t id_proxima_tarea_del_tripu);

// Otras funciones
int obtener_indice(t_list* lista, void* valor);
t_segmento* obtener_segmento_libre(uint32_t tamanio_buscado);
t_list* segmentos_libres(void);
t_list* segmentos_ocupados(void);
bool validar_existencia_segmento_libre_suficiente(uint32_t tamanio_buscado);
bool menor_a_mayor_segun_inicio(void* segmento, void* segmento_siguiente);
bool menor_a_mayor_segun_tamanio(void* segmento, void* segmento_siguiente);
bool menor_a_mayor_por_segmento(void* segmento, void* segmento_siguiente);
bool menor_a_mayor_por_pid(void* segmento, void* segmento_siguiente);


#endif /* SEGMENTACION_H_ */

#ifndef UTILS_ESTRUCTURAS_H_
#define UTILS_ESTRUCTURAS_H_

#include <semaphore.h>
#include <pthread.h>
#include "tareas.h"

typedef enum
{
	INICIAR_PATOTA,
	LISTAR_TRIPULANTES,
	EXPULSAR_TRIPULANTE,
	INICIAR_PLANIFICACION,
	PAUSAR_PLANIFICACION,
	OBTENER_BITACORA,
	RECIBIR_PATOTA,
	TERMINAR_PROGRAMA,
	INICIAR_TRIPULANTE,

	ACTUALIZAR_UBICACION_TRIPULANTE,
	PEDIDO_TAREA,
	PEDIR_UBICACION_TRIPULANTE,
	ACTUALIZAR_ESTADO_TRIPULANTE,
	ENVIAR_SABOTAJE,
	RECIBIR_SABOTAJE,
	CERRAR_MODULO,

	RESPUESTA_INICIAR_PATOTA,
	RESPUESTA_OK_UBICACION,
	RESPUESTA_NUEVA_UBICACION,
	RESPUESTA_OK_ESTADO,
	RESPUESTA_NUEVA_TAREA,
	RESPUESTA_TRIPULANTE_ELIMINADO

} codigo_operacion;




typedef enum
{
	PAGINACION,
	SEGMENTACION
} codigo_memoria;


typedef enum
{
	LRU,
	CLOCK
} algoritmo_reemplazo;


typedef enum
{
	PATOTA,
	TAREAS,
	TRIPULANTE,
	VACIO
} tipo_segmento;


typedef enum
{
	LIBRE,
	OCUPADO
} estado;


typedef enum
{
	BEST_FIT,
	FIRST_FIT
} criterio_seleccion;



// Estructuras de Sockets
typedef struct
{
	uint32_t size;
	void* stream;
} t_buffer;


typedef struct
{
	codigo_operacion op_code;
	t_buffer* buffer;
} t_paquete;




// Estructuras para Discordiador
typedef struct {
	uint32_t posicion_x;
	uint32_t posicion_y;
} posiciones;





typedef struct {
	uint32_t id_tripulante;
	uint32_t numero_patota;
	char estado;

	t_tarea* tarea_a_realizar;

	bool elegido_sabotaje;
	sem_t* mutex_elegido;
	t_sabotaje* tarea_sabotaje;

	bool expulsado;

	sem_t* sem_tripu;
	sem_t* sem_planificacion;

	sem_t* mutex_estado;
} tripulante_plani;


typedef struct {
	uint32_t sabotaje_on;
	t_sabotaje* tarea_sabotaje;             //    ---------------
	posiciones* posicion_sabotaje;
} t_respuesta_mongo;




// Estructuras para Mi-RAM HQ
typedef struct {
	uint32_t cantidad_tripulantes;
	char* tareas_de_patota;
	uint32_t tamanio_tareas;
	char* posiciones;
	uint32_t tamanio_posiciones;
} t_iniciar_patota;


typedef struct {
	uint32_t id_tripulante;
	uint32_t id_patota;
} t_tripulante;


typedef struct {
	uint32_t id_tripulante;
	uint32_t id_patota;
	uint32_t posicion_x;
	uint32_t posicion_y;
} t_tripulante_ubicacion;


typedef struct {
	uint32_t id_tripulante;
	uint32_t id_patota;
	char estado;
} t_tripulante_estado;




// Estructura para la Respuesta de Mi-RAM HQ
typedef struct {
	uint32_t respuesta;
	char* ids_tripu;
	uint32_t tamanio_ids;
	uint32_t numero_de_patota;
} t_respuesta_iniciar_patota;


typedef struct {
	uint32_t respuesta;
	uint32_t id_tripulante;
	uint32_t posicion_x;
	uint32_t posicion_y;
} t_respuesta_tripulante_ubicacion;


typedef struct {
	uint32_t respuesta;
	uint32_t id_tripulante;
	t_tarea* tarea;
} t_respuesta_tarea_tripulante;


typedef struct {
	uint32_t respuesta;
	uint32_t id_tripulante;
} t_respuesta_tripulante;





// Patota Control Block (PCB)
typedef struct {
	uint32_t pid;						// ID patota
	uint32_t tareas;					// Direccion de memoria de donde estan las tareas
} t_pcb;
// Tamaño del PCB = 8 bytes


// Tripulante Control Block (TCB)
typedef struct {
	uint32_t id_tripulante;				// ID tripulante
	char estado_tripulante;				// (N/R/E/B)
	uint32_t posicion_x;
	uint32_t posicion_y;
	uint32_t id_tarea_a_realizar;		// Indice de la tarea a realizar
	uint32_t puntero_PCB;				// Dirección de memoria del PCB de la patota
} t_tcb;
// Tamaño del TCB = 24 bytes
// Deberia ser 21 bytes


// TODO porque cuando hago sizeof(t_tcb) = 24, pero cuando hago la suma de cada sizeof(registro) = 21 bytes?

#endif /* UTILS_ESTRUCTURAS_H_ */

#ifndef UTILS_ESTRUCTURAS_H_
#define UTILS_ESTRUCTURAS_H_
#include <semaphore.h>
#include <pthread.h>

#include <semaphore.h>

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
	RECIBIR_UBICACION_TRIPULANTE,
	ENVIAR_PROXIMA_TAREA,

	RESPUESTA_INICIAR_PATOTA
} codigo_operacion;


typedef enum
{
	GENERAR_OXIGENO,
	CONSUMIR_OXIGENO,
	GENERAR_COMIDA,
	CONSUMIR_COMIDA,
	GENERAR_BASURA,
	DESCARTAR_BASURA,
	MOVERSE,
	ABURRIRSE
} codigo_tarea;


typedef enum
{
	PAGINACION,
	SEGMENTACION
} codigo_memoria;


// Estructura para la Respuesta
typedef struct {
	uint32_t respuesta;
	char* ids_tripu;
	uint32_t tamanio_ids;
} t_respuesta;


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




// Estructuras del Discordiador
typedef struct {
	uint32_t id_tripulante;
	pthread_t hilo_id_tripulante;
	sem_t sem_execute;
} t_iniciar_tripulante;


typedef struct {
	uint32_t cantidad_tripulantes;
	char* tareas_de_patota;
	uint32_t tamanio_tareas;
	char* posiciones;
	uint32_t tamanio_posiciones;
	//uint32_t pid_patota;
} t_iniciar_patota;


typedef struct {
	uint32_t id_tripulante;
	uint32_t id_patota;
} t_id_tripulante;


typedef struct {
	uint32_t posicion_x;
	uint32_t posicion_y;
} posiciones;



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
	uint32_t id_proxima_instruccion;	// Linea del archivo de texto
	uint32_t puntero_PCB;				// Dirección de memoria del PCB de la patota
} t_tcb;
// Tamaño del TCB = 17 bytes




// Estructuras de Tareas

typedef struct {
    codigo_tarea operacion;
    int32_t cantidad;
    uint32_t posicion_x;
    uint32_t posicion_y;
    int32_t tiempo;
} t_tarea;

typedef struct {
	uint32_t id_tripulante;
	uint32_t numero_patota;
	char estado;
	t_tarea* tarea_a_realizar;
	sem_t* sem_tripu;
	sem_t* sem_planificacion;
} tripulante_plani;

// Estructuras para Mi RAM HQ

// Tabla de Paginas
typedef struct tabla_paginas
{
	int32_t numero_de_marco;
	char* estado_proceso;				// Libre u Ocupado
	uint32_t id_proceso;				// Proceso de Patota
	uint32_t numero_de_pagina;


	struct tabla_paginas* ant_pagina;
	struct tabla_paginas* sig_pagina;
} t_paginas;


// Tabla de Segmentos de cada Tripulante
typedef struct tabla_segmentos_tripulante
{
	t_tcb* tripulante;

	struct tabla_segmentos_tripulante* ant_segmento;
	struct tabla_segmentos_tripulante* sig_segmento;
} t_segmentos_tripulantes;


// Tabla de Segmentos de cada Tarea
typedef struct tabla_segmentos_tarea
{
	t_tarea* tarea;

	struct tabla_segmentos_tarea *ant_segmento;
	struct tabla_segmentos_tarea *sig_segmento;
} t_segmentos_tarea;


// Tabla de Segmentos de cada Patota
typedef struct tabla_segmentos_patota
{
	uint32_t numero_de_segmento;		// Esta tabla va a tener el numero de segmento
	uint32_t inicio;					// Direccion fisica de donde empieza el segmento
	uint32_t tamanio_segmento;			// Tamanio total del segmento

	t_pcb* patota;
	uint32_t cantidad_tripulantes;
	t_segmentos_tripulantes* tripulantes;

	uint32_t cantidad_tareas;
	t_segmentos_tarea* tareas;

	struct tabla_segmentos_patota* ant_segmento;
	struct tabla_segmentos_patota* sig_segmento;
} t_segmentos_patota;




typedef struct espacio
{
	uint32_t numeroDeEspacio;
	uint32_t espacioOcupado;

	struct espacio* sig_espacio;
} espacio;



#endif /* UTILS_ESTRUCTURAS_H_ */

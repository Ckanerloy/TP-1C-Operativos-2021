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
	GUARDAR_PATOTA,
	GUARDAR_TRIPULANTE,
	GUARDAR_TAREAS,
} codigo_guardado;

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
	uint32_t numero_de_patota;
} t_respuesta_iniciar_patota;


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
	uint32_t id_tripulante;
	//sem_t* sem_execute;
} tripulante_plani;

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
typedef struct{
	int32_t cantidad;
	uint32_t posicion_x;
	uint32_t posicion_y;
	int32_t tiempo;
} t_parametros_tarea;


typedef struct {
	codigo_tarea operacion;
	t_parametros_tarea* parametros;
} t_tarea;


#endif /* UTILS_ESTRUCTURAS_H_ */

#ifndef UTILS_ESTRUCTURAS_H_
#define UTILS_ESTRUCTURAS_H_

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
	ENVIAR_PROXIMA_TAREA
} codigo_operacion;


typedef enum
{
	GENERAR_OXIGENO,
	CONSUMIR_OXIGENO,
	GENERAR_COMIDA,
	CONSUMIR_COMIDA,
	GENERAR_BASURA,
	DESCARTAR_BASURA
} codigo_tarea;


typedef struct {
	uint32_t id_tripulante;
	char estado_tripulante;
	uint32_t tamanio_estado_tripulante;
	int posicion_x;
	int posicion_y;
	uint32_t peso_tripulante;
} t_tripulante;


typedef struct {
	uint32_t cantidad_tripulantes;
	char* tareas_de_patota;
	uint32_t tamanio_tareas;
	char* posiciones;
	uint32_t tamanio_posiciones;
	uint32_t pid_patota;
} t_iniciar_patota;


typedef struct {
	uint32_t id_tripulante;
} t_id_tripulante;


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


// Patota Control Block (PCB)
typedef struct {
	uint32_t pid;						// ID patota
	uint32_t tareas;					// Dirección lógica del inicio de las Tareas
}t_pcb;
// Tamaño del PCB = 8 bytes


// Tripulante Control Block (TCB)
typedef struct {
	uint32_t tid;						// ID tripulante
	char estado_tripulante;				// (N/R/E/B)
	uint32_t posicion_x;
	uint32_t posicion_y;
	uint32_t id_proxima_instruccion;	// Linea del archivo de texto
	uint32_t puntero_PCB;				// Dirección lógica del PCB
}t_tcb;
// Tamaño del TCB = 17 bytes


#endif /* UTILS_ESTRUCTURAS_H_ */

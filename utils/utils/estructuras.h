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
	EXIT
}codigo_operacion;


typedef enum
{
	GENERAR_OXIGENO,
	CONSUMIR_OXIGENO,
	GENERAR_COMIDA,
	CONSUMIR_COMIDA,
	GENERAR_BASURA,
	DESCARTAR_BASURA
}codigo_tarea;


typedef struct {
	uint32_t id_tripulante;
	char* estado_tripulante;
	uint32_t tamanio_estado_tripulante;
	int posicion_x;
	int posicion_y;
	uint32_t peso_tripulante;
} t_tripulante;


typedef struct {
	uint32_t cantidad_tripulantes;
	char* archivo_tareas;
	uint32_t tamanio_tareas;
	//t_tripulante** tripulantes;
	//uint32_t tamanio_tripulantes;
} t_patota;


typedef struct {
	uint32_t id_tripulante;
} t_id_tripulante;

typedef struct {
	uint32_t id;
	int posicion_x;
	int posicion_y;
} t_datos_hilo;

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
	uint32_t id_patota;
	void* direccion_tareas;
}t_pcb;

// Tamaño del PCB = 8 bytes


// Tripulante Control Block (TCB)
typedef struct {
	uint32_t id_tripulante;
	char estado_tripulante;				// (N/R/E/B)
	uint32_t posicion_x;
	uint32_t posicion_y;
	uint32_t id_proxima_instruccion;		// Linea del archivo de texto
}t_tcb;

// Tamaño del TCB = 17 bytes


#endif /* UTILS_ESTRUCTURAS_H_ */

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
	uint32_t cantidad_tripulantes;
	char* archivo_tareas;
	uint32_t tamanio_tareas;
	// Faltaria el vector para la posicion de los tripulantes
	// y el tamaño de dicho vector
	//	uint32_t posicion_tripulantes[];
} patota;


typedef struct {
	uint32_t id_tripulante;
} tripulante;





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
	uint32_t id_patota;					// El pid
	void* direccion_tareas;
}pcb;

// Tamaño del PCB = 8 bytes


// Tripulante Control Block (TCB)
typedef struct {
	uint32_t id_tripulante;				// El tid
	char estado_tripulante;				// (N/R/E/B)
	uint32_t posicion_x;
	uint32_t posicion_y;
	uint32_t id_proxima_instruccion;
}tcb;


#endif /* UTILS_ESTRUCTURAS_H_ */

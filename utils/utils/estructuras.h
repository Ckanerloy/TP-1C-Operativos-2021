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

	RECIBIR_PATOTA
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
//	uint32_t posicion_tripulantes[];
	uint32_t tamanio_posiciones;
} patota;

typedef struct {
	uint32_t id_tripulante;
} bitacora;

// Patota Control Block (PCB)
typedef struct {
	uint32_t id_patota;
	void* direccion_tareas;
}pcb;

// Tamaño del PCB = 8 bytes


// Tripulante Control Block (TCB)
typedef struct {
	uint32_t id_tripulante;
	char estado_tripulante;
	uint32_t posicion_x;
	uint32_t posicion_y;
	uint32_t id_proxima_instruccion;
}tcb;


#endif /* UTILS_ESTRUCTURAS_H_ */

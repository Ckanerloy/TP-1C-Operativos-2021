#ifndef PAGINACION_H_
#define PAGINACION_H_

#include "Mi-RAM.h"
#include "utils/estructuras.h"
#include "swap.h"


typedef struct {
	int32_t numero_de_pagina;			// Número que identifica a la página
	int32_t numero_de_frame;			// Frame en donde está guardada la página
	int32_t U;							// Bit de Uso
	int32_t P;							// Bit de Presencia
	uint32_t tiempo_referencia;			// Tiempo de referencia de la página (para aplicar el LRU)
	int32_t estado;						// Si la página esta LIBRE u OCUPADO
} t_pagina;


// DIRECCION_LOGICA = NUMERO_PAGINA * TAMANIO_PAGINA + DESPLAZAMIENTO;
// DIRECCION_FISICA = NUMERO_FRAME * TAMANIO_FRAME + DESPLAZAMIENTO;

typedef struct {
	t_pcb* patota;
	t_list* paginas;					// Páginas que componen a la Patota (incluyendo el PCB, Tareas y TCBs)
	int32_t direccion_patota;			// Dirección lógica del PCB
	t_list* direccion_tripulantes;		// Direcciones lógicas de los tripulantes que componen a la patota con su respectivo id_tripulante
	uint32_t tamanio_tareas;
	int32_t cantidad_tripulantes;
} t_tabla_paginas_patota;


typedef struct {
	int32_t estado;						// Si el Frame esta LIBRE u OCUPADO
	int32_t proceso;					// El Proceso o Patota que está ocupando dicho Frame
	int32_t pagina;						// La página que tiene cargada dicho Frame
	int32_t espacio_libre;				// El espacio libre del frame (para verificar si esta libre)
	int32_t puntero_frame;				// Puntero que apunta el último byte usado del Frame
} frame;


typedef struct {
	int32_t estado;
	int32_t pagina;
	int32_t espacio_libre;
} frame_swap;


typedef struct {
	int32_t id_tripulante;
	int32_t direccion_logica;
} t_dl_tripulante;


t_list* tablas_paginas;

int32_t contador_pagina;
int32_t puntero_inicio;					// Puntero que indica cuando inicia la siguiente estructura
uint32_t cantidad_frames;
frame** frames;
frame_swap** frames_swap;

// Inicio de Paginación
t_tabla_paginas_patota* crear_tabla_paginas(t_pcb* patota, int32_t tamanio_total, int32_t cantidad_tripulantes);
void iniciar_tabla_patota(t_tabla_paginas_patota* tabla_patota, int32_t tamanio_total, tareas_patota* tareas_de_la_patota, int32_t cantidad_tripulantes);
void guardar_estructura_en_memoria(void* estructura, tipo_estructura tipo, t_tabla_paginas_patota* tabla_patota, int32_t tamanio_estructura);


// Uso de Frames
void inicializar_frames(void);
void asignar_frame_disponible(t_pagina* pagina, uint32_t pid);
int32_t obtener_frame_disponible(void);
bool hay_frame_libre(void);
int32_t obtener_siguiente_frame(t_list* paginas, int32_t contador);
int32_t obtener_frame_libre(void);


// Serializaciones para guardar estructuras
void* serializar_patota(t_pcb* patota, uint32_t tamanio);
void* serializar_tareas(tareas_patota* tareas_de_la_patota, uint32_t tamanio);
void* serializar_tripulante(t_tcb* tripulante, uint32_t tamanio);

t_tabla_paginas_patota* buscar_tabla_patota(uint32_t id_patota);
int32_t buscar_pagina_por_id(t_tabla_paginas_patota* tabla_patota_buscada, uint32_t id_tripulante_a_buscar);
int32_t obtener_direc_fisica_con_direccion_logica(int32_t direccion_logica, t_tabla_paginas_patota* tabla_patota_buscada);
int32_t buscar_frame(int32_t nro_pagina, t_list* paginas);
t_pagina* buscar_pagina(int32_t nro_pagina, t_list* paginas);

void* obtener_tripulante_de_memoria(uint32_t direccion_fisica, uint32_t direccion_logica, t_tabla_paginas_patota* tabla_patota);
t_tcb* encontrar_tripulante_memoria(uint32_t direccion_fisica, uint32_t direccion_logica, t_tabla_paginas_patota* tabla_patota);
void actualizar_tripulante_memoria(t_tcb* tripulante, uint32_t direccion_fisica);
void actualizar_referencia(t_list* paginas, uint32_t direccion_logica);

void* obtener_tareas_de_memoria(uint32_t direccion_fisica, uint32_t tamanio_tareas, t_tabla_paginas_patota* tabla_patota);
char* encontrar_tareas_en_memoria(uint32_t direccion_fisica, uint32_t tamanio_tareas, t_tabla_paginas_patota* tabla_patota_buscada);
t_tarea* buscar_proxima_tarea_del_tripulante_paginacion(uint32_t direccion_fisica, uint32_t id_tarea_buscada, uint32_t tamanio_tareas, t_tabla_paginas_patota* tabla_patota_buscada);


// Otras funciones
int32_t cantidad_paginas_usadas(int32_t tamanio);
bool menor_a_mayor_por_frame(void* pagina, void* pagina_siguiente);
bool menor_a_mayor_segun_num(void* pagina, void* pagina_siguiente);
uint32_t get_timestamp(void);
void liberar_frame(uint32_t num_frame);

#endif /* PAGINACION_H_ */

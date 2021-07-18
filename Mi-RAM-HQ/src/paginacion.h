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
	int32_t tiempo_referencia;			// Tiempo de referencia de la página (para aplicar el LRU)
} t_pagina;


// DIRECCION_LOGICA = NUMERO_PAGINA * TAMANIO_PAGINA + DESPLAZAMIENTO;
// DIRECCION_FISICA = NUMERO_FRAME * TAMANIO_FRAME + DESPLAZAMIENTO;

typedef struct {
	t_list* paginas;					// Páginas que componen a la Patota (incluyendo el PCB, Tareas y TCBs)
	int32_t direccion_patota;			// Dirección lógica del PCB
	int32_t direccion_tareas;			// Dirección lógica de las Tareas de la Patota
	t_list* direccion_tripulantes;		// Direcciones lógicas de los tripulantes que componen a la patota
} t_tabla_paginas_patota;


typedef struct {
	int32_t estado;						// Si el Frame esta LIBRE u OCUPADO
	int32_t proceso;					// El Proceso o Patota que está ocupando dicho Frame
	int32_t pagina;						// La página que tiene cargada dicho Frame
	int32_t espacio_libre;				//
} frame;


typedef struct {
	int32_t numero_pagina;
	int32_t numero_frame;
} tlb;							// Capaz es al pedo


t_list* tablas_paginas;

int32_t contador_pagina;
int32_t puntero_inicio;					// Puntero que indica cuando inicia la siguiente estructura
uint32_t cantidad_frames;
frame** frames;


// Inicio de Paginación
t_tabla_paginas_patota* crear_tabla_paginas(void);
void administrar_paginas_patota(t_tabla_paginas_patota* tabla_patota, int32_t tamanio_total, tareas_patota* tareas_de_la_patota, int32_t cantidad_tripulantes);


// Uso de Frames
void inicializar_frames(void);
int32_t obtener_frame_disponible(void);
int hay_frame_libre(void);
int32_t obtener_siguiente_frame(t_list* paginas, int32_t contador);
int32_t obtener_frame_libre(void);

// Otras funciones
int32_t cantidad_paginas_usadas(int32_t tamanio);
bool menor_a_mayor_por_frame(void* pagina, void* pagina_siguiente);

#endif /* PAGINACION_H_ */

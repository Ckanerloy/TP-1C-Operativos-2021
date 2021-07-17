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
} frame;


t_list* tablas_paginas;

uint32_t base_pagina;
uint32_t cantidad_paginas;
uint32_t cantidad_frames;
frame** frames;



void inicializar_frames(void);
t_tabla_paginas_patota* crear_tabla_paginas(t_pcb* nueva_patota);



bool menor_a_mayor_por_frame(void* pagina, void* pagina_siguiente);

#endif /* PAGINACION_H_ */

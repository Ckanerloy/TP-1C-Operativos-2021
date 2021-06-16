#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "Mi-RAM.h"
#include "utils/estructuras.h"

char esquema_elegido;

t_list* tabla_segmentos;
t_list* tabla_paginas;


void elegir_esquema_de_memoria(char* ESQUEMA);


// Estructuras para Mi RAM HQ

// PAGINACION
typedef struct tabla_paginas
{
	int32_t numero_de_marco;
	uint32_t numero_de_pagina;

} t_paginas;


// SEGMENTACION
typedef struct segmento
{
	uint32_t numero_de_segmento;		// Esta tabla va a tener el numero de segmento
	uint32_t inicio;					// Direccion fisica de donde empieza el segmento
	uint32_t tamanio_segmento;			// Tamanio total del segmento

} t_segmento;


typedef struct	tabla_segmentos_patota
{
	t_pcb* patota;
	//t_list* segmentos;
} t_segmentos_patota;

typedef struct
{

};



typedef struct espacio
{
	uint32_t numeroDeEspacio;
	uint32_t espacioOcupado;

	struct espacio* sig_espacio;
} espacio;


#endif /* MEMORIA_H_ */

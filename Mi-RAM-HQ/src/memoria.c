#include "memoria.h"


codigo_memoria mapeo_esquema_memoria(char* ESQUEMA)
{
	codigo_memoria esquema_memoria;

	if(strcmp(ESQUEMA, "PAGINACION") == 0) {
		esquema_memoria = PAGINACION;
	}

	if(strcmp(ESQUEMA, "SEGMENTACION") == 0) {
		esquema_memoria = SEGMENTACION;
	}

	return esquema_memoria;
}


void elegir_esquema_de_memoria(char* ESQUEMA)
{
	codigo_memoria cod_mem;

	cod_mem = mapeo_esquema_memoria(ESQUEMA);

	switch(cod_mem) {

		case PAGINACION:

			//t_pagina_patota* pagina_patota = malloc(sizeof(t_pagina_patota));
			//inicializar_tabla_paginas_de_patota(pagina_patota);

			break;
		case SEGMENTACION:

			//t_segmentos_patota* segmento_patota = malloc(sizeof(t_segmentos_patota));
			//inicializar_tabla_segmentos_de_patota();

			break;
		default:
			break;
	}
}


void inicializar_tabla_segmentos_de_patota(t_segmentos_patota* segmento_patota)
{
	segmento_patota->numero_de_segmento = 0;
	segmento_patota->inicio = 0;
	segmento_patota->tamanio_segmento = 0;

	segmento_patota->patota = NULL;

	segmento_patota->cantidad_tripulantes = 0;
	segmento_patota->tripulantes = malloc(sizeof(t_segmentos_tripulantes));
	inicializar_tabla_segmentos_de_tripulante(segmento_patota->tripulantes);

	segmento_patota->cantidad_tareas = 0;
	segmento_patota->tareas = malloc(sizeof(t_segmentos_tarea));
	inicializar_tabla_segmentos_de_tarea(segmento_patota->tareas);

	segmento_patota->ant_segmento = NULL;
	segmento_patota->sig_segmento = NULL;
}


void inicializar_tabla_segmentos_de_tripulante(t_segmentos_tripulantes* segmento_tripulante)
{
	segmento_tripulante->tripulante = NULL;

	segmento_tripulante->ant_segmento = NULL;
	segmento_tripulante->sig_segmento = NULL;
}

void inicializar_tabla_segmentos_de_tarea(t_segmentos_tarea* segmento_tarea)
{

	segmento_tarea->tarea = NULL;

	segmento_tarea->ant_segmento = NULL;
	segmento_tarea->sig_segmento = NULL;
}





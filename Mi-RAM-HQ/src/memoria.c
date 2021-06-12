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

			pagina_patota = malloc(sizeof(t_paginas_patota));
			inicializar_tabla_paginas_de_patota(pagina_patota);

			break;
		case SEGMENTACION:

			segmento_patota = malloc(sizeof(t_segmentos_patota));
			inicializar_tabla_segmentos_de_patota(segmento_patota);

			break;
		default:
			break;
	}
}


// Tabla de Segmentos
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




// Tablas de Paginas
void inicializar_tabla_paginas_de_patota(t_paginas_patota* pagina_tarea)
{
	pagina_tarea->numero_de_marco = 0;
	pagina_tarea->numero_de_pagina = 0;

	pagina_tarea->patota = NULL;

	pagina_tarea->cantidad_tripulantes = 0;
	inicializar_tabla_paginas_de_tripulante(pagina_tarea->tripulantes);

	pagina_tarea->cantidad_tareas = 0;
	inicializar_tabla_paginas_de_tareas(pagina_tarea->tareas);

	pagina_tarea->ant_pagina = NULL;
	pagina_tarea->sig_pagina = NULL;
}


void inicializar_tabla_paginas_de_tripulante(t_paginas_tripulantes* pagina_tripulante)
{
	pagina_tripulante->tripulante = NULL;

	pagina_tripulante->ant_pagina = NULL;
	pagina_tripulante->sig_pagina = NULL;
}

void inicializar_tabla_paginas_de_tareas(t_paginas_tarea* pagina_tarea)
{
	pagina_tarea->tarea = NULL;

	pagina_tarea->ant_pagina = NULL;
	pagina_tarea->sig_pagina = NULL;
}




t_segmentos_patota* crear_segmento_patota(t_segmentos_patota* tabla_segmentos_patota, t_pcb* patota_recibida, t_tarea** tarea_patota, uint32_t cant_tareas, uint32_t cant_tripulantes)
{
	t_segmentos_patota* nuevo_segmento_patota = malloc(sizeof(t_segmentos_patota));

	while(tabla_segmentos_patota->sig_segmento != NULL) {
		tabla_segmentos_patota = tabla_segmentos_patota->sig_segmento;
	}


	tabla_segmentos_patota->sig_segmento = nuevo_segmento_patota;

	nuevo_segmento_patota->numero_de_segmento = tabla_segmentos_patota->numero_de_segmento++;

	nuevo_segmento_patota->patota = patota_recibida;

	nuevo_segmento_patota->cantidad_tripulantes = cant_tripulantes;
	//inicializar_tabla_segmentos_de_tripulante(nuevo_segmento_patota->tripulantes);

	// Agregar en la misma, la cantida de tareas y la lista de tareas
	nuevo_segmento_patota->cantidad_tareas = cant_tareas;
	crear_segmento_tarea(nuevo_segmento_patota->tareas, tarea_patota);

	nuevo_segmento_patota->ant_segmento = tabla_segmentos_patota;
	nuevo_segmento_patota->sig_segmento = NULL;

	return nuevo_segmento_patota;
}

void crear_segmento_tarea(t_segmentos_tarea* tabla_segmento_tarea, t_tarea** tarea_patota)
{
	int posicion = 0;
	while(tarea_patota[posicion] != NULL) {
		t_segmentos_tarea* nuevo_segmento_tarea = malloc(sizeof(t_segmentos_tarea));

		while(tabla_segmento_tarea->sig_segmento != NULL || tabla_segmento_tarea->tarea != NULL) {
			tabla_segmento_tarea = tabla_segmento_tarea->sig_segmento;
		}

		tabla_segmento_tarea->sig_segmento = nuevo_segmento_tarea;

		nuevo_segmento_tarea->tarea = tarea_patota[posicion];

		nuevo_segmento_tarea->ant_segmento = tabla_segmento_tarea;
		nuevo_segmento_tarea->sig_segmento = NULL;

		posicion++;
	}
}


void crear_segmento_tripulante(t_segmentos_patota* tabla_segmento_patota, t_tcb* tripulante)
{
	t_segmentos_tripulantes* nuevo_segmento_tripulante = malloc(sizeof(t_segmentos_tripulantes));

	while(tabla_segmento_patota->tripulantes->sig_segmento != NULL) {
		tabla_segmento_patota->tripulantes = tabla_segmento_patota->tripulantes->sig_segmento;
	}

	tabla_segmento_patota->tripulantes->sig_segmento = nuevo_segmento_tripulante;

	nuevo_segmento_tripulante->tripulante = tripulante;

	nuevo_segmento_tripulante->ant_segmento = tabla_segmento_patota->tripulantes;
	nuevo_segmento_tripulante->sig_segmento = NULL;
}

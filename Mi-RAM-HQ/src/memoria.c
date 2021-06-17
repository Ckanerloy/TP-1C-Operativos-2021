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

			esquema_elegido = 'P';
			tabla_paginas = list_create();

			break;
		case SEGMENTACION:

			esquema_elegido = 'S';
			tablas_segmentos = list_create();

			break;
		default:
			break;
	}
}


algoritmo_reemplazo elegir_algoritmo_reemplazo(char* algoritmo)
{
	algoritmo_reemplazo algoritmo_reemplazo;

	if(strcmp(algoritmo, "LRU") == 0) {
		algoritmo_reemplazo = LRU;
	}
	if(strcmp(algoritmo, "CLOCK") == 0) {
		algoritmo_reemplazo = CLOCK;
	}

	return algoritmo_reemplazo;
}


criterio_seleccion elegir_criterio_seleccion(char* criterio)
{
	criterio_seleccion criterio_seleccionado;

	if(strcmp(criterio, "BEST_FIT") == 0) {
		criterio_seleccionado = BEST_FIT;
	}
	if(strcmp(criterio, "FIRST_FIT") == 0) {
		criterio_seleccionado = FIRST_FIT;
	}

	return criterio_seleccionado;
}




t_tabla_segmentos_patota* crear_tabla_segmentos(t_pcb* nueva_patota){
	pthread_mutex_lock(&mutexTablasDeSegmentos);
	t_tabla_segmentos_patota* tabla = malloc(sizeof(t_tabla_segmentos_patota));
	tabla->patota = malloc(sizeof(t_pcb));
	tabla->patota = nueva_patota;
	tabla->segmentos = list_create();

	list_add(tablas_segmentos,tabla);
	pthread_mutex_unlock(&mutexTablasDeSegmentos);//preguntar a nico y cami

	return tabla;

}





uint32_t administrar_guardar_patota(t_pcb* nueva_patota)
{
	int32_t desplazamiento = 0;
	uint32_t tamanio_segmento;

	memcpy(memoria_principal + desplazamiento, &(nueva_patota->pid), sizeof(nueva_patota->pid));
	desplazamiento += sizeof(nueva_patota->pid);

	memcpy(memoria_principal + desplazamiento, &(nueva_patota->tareas), sizeof(nueva_patota->tareas));
	desplazamiento += sizeof(nueva_patota->tareas);

	tamanio_segmento = sizeof(t_pcb);

	return tamanio_segmento;
}



uint32_t administrar_guardar_tareas(t_queue* tareas_de_la_patota)
{
	int32_t desplazamiento = 0;
	uint32_t tamanio_segmento;

	memcpy(memoria_principal + desplazamiento, &(tareas_de_la_patota), (sizeof(t_tarea) * queue_size(tareas_de_la_patota)));
	desplazamiento += (sizeof(t_tarea) * queue_size(tareas_de_la_patota));

	tamanio_segmento = sizeof(t_tarea) * queue_size(tareas_de_la_patota);

	return tamanio_segmento;
}


uint32_t administrar_guardar_tripulante(t_tcb* nuevo_tripulante)
{
	int32_t desplazamiento = 0;
	uint32_t tamanio_segmento;

	memcpy(memoria_principal + desplazamiento, &(nuevo_tripulante->id_tripulante), sizeof(nuevo_tripulante->id_tripulante));
	desplazamiento += sizeof(nuevo_tripulante->id_tripulante);

	memcpy(memoria_principal + desplazamiento, &(nuevo_tripulante->estado_tripulante), sizeof(nuevo_tripulante->estado_tripulante));
	desplazamiento += sizeof(nuevo_tripulante->estado_tripulante);

	memcpy(memoria_principal + desplazamiento, &(nuevo_tripulante->posicion_x), sizeof(nuevo_tripulante->posicion_x));
	desplazamiento += sizeof(nuevo_tripulante->posicion_x);

	memcpy(memoria_principal + desplazamiento, &(nuevo_tripulante->posicion_y), sizeof(nuevo_tripulante->posicion_y));
	desplazamiento += sizeof(nuevo_tripulante->posicion_y);

	memcpy(memoria_principal + desplazamiento, &(nuevo_tripulante->id_proxima_instruccion), sizeof(nuevo_tripulante->id_proxima_instruccion));
	desplazamiento += sizeof(nuevo_tripulante->id_proxima_instruccion);

	memcpy(memoria_principal + desplazamiento, &(nuevo_tripulante->puntero_PCB), sizeof(nuevo_tripulante->puntero_PCB));
	desplazamiento += sizeof(nuevo_tripulante->puntero_PCB);

	tamanio_segmento = sizeof(t_tcb);

	return tamanio_segmento;
}


void crear_segmento(void* estructura, tipo_segmento tipo_de_segmento)
{
	t_segmento* segmento = malloc(sizeof(t_segmento));

	segmento->numero_de_segmento = contador_segmento;
	segmento->tipo_de_segmento = tipo_de_segmento;

	segmento->inicio = inicio;

	switch(tipo_de_segmento){
		case PATOTA:
			segmento->tamanio_segmento = administrar_guardar_patota(estructura);
			break;
		case TAREAS:
			segmento->tamanio_segmento = administrar_guardar_tareas(estructura);
			break;
		case TRIPULANTE:
			segmento->tamanio_segmento = administrar_guardar_tripulante(estructura);
			break;
		default:
			break;
	}

	inicio += segmento->tamanio_segmento;

	// Guardar el segmento en la tabla

	contador_segmento++;

	memoria_restante -= segmento->tamanio_segmento;

}

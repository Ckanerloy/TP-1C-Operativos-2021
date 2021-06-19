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


algoritmo_reemplazo elegir_algoritmo_reemplazo(char* algoritmo){
	algoritmo_reemplazo algoritmo_reemplazo;

	if(strcmp(algoritmo, "LRU") == 0) {
		algoritmo_reemplazo = LRU;
	}
	if(strcmp(algoritmo, "CLOCK") == 0) {
		algoritmo_reemplazo = CLOCK;
	}

	return algoritmo_reemplazo;
}


criterio_seleccion elegir_criterio_seleccion(char* criterio){
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

	t_tabla_segmentos_patota* tabla = malloc(sizeof(t_tabla_segmentos_patota));
	tabla->patota = malloc(sizeof(t_pcb));
	tabla->patota = nueva_patota;
	tabla->segmentos = list_create();

	return tabla;
}


uint32_t administrar_guardar_patota(t_pcb* nueva_patota){
	int32_t desplazamiento = 0;
	uint32_t tamanio_segmento;

	memcpy(memoria_principal + desplazamiento, &(nueva_patota->pid), sizeof(nueva_patota->pid));
	desplazamiento += sizeof(nueva_patota->pid);

	memcpy(memoria_principal + desplazamiento, &(nueva_patota->tareas), sizeof(nueva_patota->tareas));
	desplazamiento += sizeof(nueva_patota->tareas);

	tamanio_segmento = sizeof(t_pcb);

	return tamanio_segmento;
}



uint32_t administrar_guardar_tareas(t_list* tareas_de_la_patota)
{
	int32_t desplazamiento = 0;
	uint32_t tamanio_segmento;

	memcpy(memoria_principal + desplazamiento, &(tareas_de_la_patota), (sizeof(t_tarea) * list_size(tareas_de_la_patota)));
	desplazamiento += (sizeof(t_tarea) * list_size(tareas_de_la_patota));

	tamanio_segmento = sizeof(t_tarea) * list_size(tareas_de_la_patota);

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


t_segmento* crear_segmento(void* estructura, tipo_estructura tipo_estructura){
	t_segmento* segmento = malloc(sizeof(t_segmento));

	segmento->numero_de_segmento = contador_segmento;
	segmento->tipo_estructura = tipo_estructura;

	segmento->inicio = inicio;

	switch(tipo_estructura){
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

	contador_segmento++;

	memoria_restante -= segmento->tamanio_segmento;
	sem_post(crear_segmento_sem);

	return segmento;

}


t_tabla_segmentos_patota* buscar_tabla_de_patota(t_pcb* patota_buscada)
{
	bool se_encuentra_patota(void* tabla){

		t_tabla_segmentos_patota* patota_tabla = (t_tabla_segmentos_patota*) tabla;

		return patota_tabla->patota->pid == patota_buscada->pid;
	}

	t_tabla_segmentos_patota* tabla_buscada = (t_tabla_segmentos_patota*) list_find(tablas_segmentos, (void*) se_encuentra_patota);

	return tabla_buscada;
}

/*
t_segmento* buscar_por_tipo_de_segmento(t_list* tabla, tipo_estructura tipo_de_segmento)
{
	bool mismo_tipo_segmento(void* segmento) {
		t_segmento* segmento_a_buscar = (t_segmento*) segmento;
		return (segmento_a_buscar->tipo_estructura == tipo_de_segmento);
	}

	t_tabla_segmentos_patota* segmento_patota = malloc(sizeof(t_tabla_segmentos_patota));
	segmento_patota = (t_tabla_segmentos_patota*)list_get(tabla, 0);

	t_segmento* segmento_buscado = (t_segmento*) list_find(segmento_patota->segmentos, (void*) mismo_tipo_segmento);

	return segmento_buscado;
}*/


t_segmento* buscar_por_tipo_de_segmento(t_list* tabla, tipo_estructura tipo_de_segmento)
{
	bool mismo_tipo_segmento(t_segmento* segmento) {
		t_segmento* segmento_a_buscar = malloc(sizeof(t_segmento));
		segmento_a_buscar = segmento;
		return (segmento_a_buscar->tipo_estructura == tipo_de_segmento);
	}

	t_tabla_segmentos_patota* segmento_patota = malloc(sizeof(t_tabla_segmentos_patota));
	segmento_patota = list_get(tabla, 0);

	//t_segmento* segmento_buscado = list_find(segmento_patota->segmentos, (void*) mismo_tipo_segmento);
	t_segmento* segmento_buscado = segmento_patota->segmentos->head->data;

	while(segmento_buscado->tipo_estructura != tipo_de_segmento) {
		segmento_buscado = (t_segmento*)segmento_patota->segmentos->head->next;
	}

	return segmento_buscado;
}

/*
tabla general = tabla_segmentos patota

		dentro de tabla_segmentos_patota = tabla_segmentos_patota->segmentos
*/


void* traducir_segmento(t_segmento* segmento_a_traducir)
{
	void* contenido;

	switch(segmento_a_traducir->tipo_estructura) {
		case PATOTA:
			contenido = malloc(sizeof(t_pcb));
			contenido = encontrar_patota(segmento_a_traducir);
			break;
		case TAREAS:
			//contenido = malloc(sizeof(t_list));
			//contenido = encontrar_tarea(segmento_a_traducir);
			break;
		case TRIPULANTE:
			contenido = malloc(sizeof(t_tcb));
			contenido = encontrar_tripulante(segmento_a_traducir);
			break;
		default:
			break;
	}

	return contenido;
}


t_pcb* encontrar_patota(t_segmento* segmento)
{
	t_pcb* patota = malloc(sizeof(t_pcb));

	void* inicio = (void*) segmento->inicio;
	uint32_t desplazamiento = 0;

	memcpy(&(patota->pid), inicio + desplazamiento, sizeof(patota->pid));
	desplazamiento += sizeof(patota->pid);

	memcpy(&(patota->tareas), inicio + desplazamiento, sizeof(patota->tareas));
	desplazamiento += sizeof(patota->tareas);

	if(segmento->tamanio_segmento == desplazamiento) {
		return patota;
	}
	else {
		return NULL;
	}
}


t_list* encontrar_tarea(t_segmento* segmento)
{
	t_list* tareas_de_la_patota = list_create();

	uint32_t tamanio_segmento = segmento->tamanio_segmento;

	void* inicio = (void*) segmento->inicio;
	uint32_t desplazamiento = 0;

	memcpy(&(tareas_de_la_patota), inicio + desplazamiento, tamanio_segmento);
	desplazamiento += tamanio_segmento;

	if(segmento->tamanio_segmento == desplazamiento) {
			return tareas_de_la_patota;
	}
	else {
		return NULL;
	}
}

t_tcb* encontrar_tripulante(t_segmento* segmento)
{
	t_tcb* tripulante = malloc(sizeof(t_tcb));

	void* inicio = (void*) segmento->inicio;
	uint32_t desplazamiento = 0;

	memcpy(&(tripulante->id_tripulante), inicio + desplazamiento, sizeof(tripulante->id_tripulante));
	desplazamiento += sizeof(tripulante->id_tripulante);

	memcpy(&(tripulante->estado_tripulante), inicio + desplazamiento, sizeof(tripulante->estado_tripulante));
	desplazamiento += sizeof(tripulante->estado_tripulante);

	memcpy(&(tripulante->posicion_x), inicio + desplazamiento, sizeof(tripulante->posicion_x));
	desplazamiento += sizeof(tripulante->posicion_x);

	memcpy(&(tripulante->posicion_y), inicio + desplazamiento, sizeof(tripulante->posicion_y));
	desplazamiento += sizeof(tripulante->posicion_y);

	memcpy(&(tripulante->id_proxima_instruccion), inicio + desplazamiento, sizeof(tripulante->id_proxima_instruccion));
	desplazamiento += sizeof(tripulante->id_proxima_instruccion);

	memcpy(&(tripulante->puntero_PCB), inicio + desplazamiento, sizeof(tripulante->puntero_PCB));
	desplazamiento += sizeof(tripulante->puntero_PCB);

	if(segmento->tamanio_segmento == desplazamiento) {
			return tripulante;
	}
	else {
		return NULL;
	}
}

/*
void* buscar_estructura(void* estructura, tipo_estructura tipo_estructura)
{

	segmento->tipo_de_estructura = tipo_estructura;
	buscar en la tabla de segmentos: segmento->id_tripulante


// PARA ENVIAR LA PROXIMA TAREA A ENVIAR
	recibo un ID_TRIPULANTE
	lo busco en los segmentos (tpo_segmento == TRIPULANTE)
	una vez que encuentro el segmento por el ID de tripulante -> obtener el id_proxima_instruccion
	y obtener la direccion logica del PCB
	una vez que voy a la direccion del segmento de esa patota -> obtener la direccion de las tareas
	y en las tareas : TRADUCIRLAS de bytes a una t_list
	y buscar el ID_PROXIMA INSTRUCCION
	retornar esa tarea al Discordiador

}
*/


/*

bool noHayNingunaSuficientementeGrande(uint32_t tamStream){

	for(int i =0; i<sizeListaMutex(particionesLibres); i++){
		t_segmento* segmento = (t_segmento*)getListaMutex(particionesLibres, i);
		if(segmento->tamanio_segmento >= tamStream){
			return false;
		}
	}
	return true;
}


t_segmento* obtener_segmento_libre(uint32_t tamanio){

	if(noHayNingunaSuficientementeGrande(tamanio) || memoria_restante < tamanio){
		log_error(logger, "No hay espacio suficiente para guardar el segmento.");
		return NULL;
	}

	if (criterio_elegido == FIRST_FIT){

		if(sizeListaMutex(particionesLibres)>1)
		{
			list_sort_Mutex(particionesLibres, menorAmayorSegunOffset);
		}

		t_segmento* pSeleccionadaFIRST = (t_segmento*)list_remove_by_condition_Mutex(particionesLibres, esSuficientementeGrandeParaElMSG);

		return pSeleccionadaFIRST;

	} else if(criterio_elegido == BEST_FIT){

		if(sizeListaMutex(particionesLibres)>1)
		{
			list_sort_Mutex(particionesLibres, menorAmayorSegunSize);
		}

		t_segmento* pSeleccionadaBEST = (t_segmento*)list_remove_by_condition_Mutex(particionesLibres, esSuficientementeGrandeParaElMSG);

		return pSeleccionadaBEST;
	}

	return NULL;
}
*/

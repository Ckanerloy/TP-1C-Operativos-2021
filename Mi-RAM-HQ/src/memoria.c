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

// Elige el esquema de memoria a utilizar e inicializa la misma
void elegir_esquema_de_memoria(char* ESQUEMA)
{
	codigo_memoria cod_mem;

	cod_mem = mapeo_esquema_memoria(ESQUEMA);

	switch(cod_mem) {

		case PAGINACION:

			esquema_elegido = 'P';
			tablas_paginas = list_create();
			// Poner la cantidad de paginas, los frames, etc.

			break;

		case SEGMENTACION:

			esquema_elegido = 'S';
			tablas_segmentos = list_create();
			segmentos = list_create();
			memoria_libre_por_segmento = 0;
			memoria_compactada = memoria_restante + memoria_libre_por_segmento;		// memoria_compactada = MEMORIA TOTAL LIBRE = TAMANIO_MEMORIA - memoria ocupada

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


/*
t_segmento* obtener_segmento_libre(uint32_t tamanio_buscado) {

	if(!segmento_libre_suficientemente_grande(tamanio_buscado)) {
		return NULL;
	}

	bool _es_suficientemente_grande(void* valor){
		t_segmento* segmento = (t_segmento*) valor;
		return segmento->tamanio_segmento >= tamanio_buscado;
	}

	t_list* segmentos_lib = segmentos_libres();

	if(criterio_elegido == BEST_FIT){

		if(list_size(segmentos_libres()) > 1) {

			t_list* segmentos_con_espacio_ordenados;
			bool _menor_a_mayor_segun_tamanio(void* primero, void* segundo) {
				return ((t_segmento*)primero)->tamanio_segmento < ((t_segmento*)segundo)->tamanio_segmento;
			}
			segmentos_con_espacio_ordenados = list_sorted(segmentos_lib, (void*)_menor_a_mayor_segun_tamanio);


			t_segmento* mejor_segmento = (t_segmento*) list_remove_by_condition(segmentos_con_espacio_ordenados, _es_suficientemente_grande);
			return mejor_segmento;
		}
	}

	else if(criterio_elegido == FIRST_FIT){

		if(list_size(segmentos_libres()) > 1) {

			t_list* segmentos_ordenados_por_inicio;
			bool _menor_a_mayor_segun_inicio(void* primero, void* segundo) {
				return ((t_segmento*)primero)->inicio < ((t_segmento*)segundo)->inicio;
			}
			segmentos_ordenados_por_inicio = list_sorted(segmentos_lib, (void*) _menor_a_mayor_segun_inicio);

			t_segmento* primer_segmento = (t_segmento*) list_remove_by_condition(segmentos_ordenados_por_inicio, _es_suficientemente_grande);
			return primer_segmento;
		}
	}
	return NULL;
}


*/




t_segmento* crear_segmento_libre(uint32_t inicio_segmento, uint32_t tamanio_libre_segmento) {

    t_segmento* segmento = malloc(sizeof(t_segmento));

    segmento->numero_de_segmento = contador_segmento;
    segmento->tipo_segmento = VACIO;

    segmento->inicio = inicio_segmento;

    segmento->estado_segmento = LIBRE;

    contador_segmento++;

    list_add(segmentos, segmento);

    return segmento;
}

t_segmento* administrar_guardar_segmento(void* estructura, tipo_segmento tipo_segmento, uint32_t tamanio) {

	if (memoria_restante >= tamanio) {
		return crear_segmento(estructura, tipo_segmento);
	}
	else if(validar_existencia_segmento_libre_suficiente(tamanio)) {
			t_segmento* segmento_libre = obtener_segmento_libre(tamanio);
			int32_t diferencia = segmento_libre->tamanio_segmento - tamanio;

			if(diferencia > 0){
				crear_segmento_libre(tamanio, diferencia);
				memoria_libre_por_segmento+= diferencia;
				segmento_libre->tamanio_segmento = tamanio;
			}
			actualizar_segmento(estructura, tipo_segmento, segmento_libre);


			return segmento_libre;
		}
	else {
		//compactar();
		return crear_segmento(estructura, tipo_segmento);
	}
}


t_segmento* crear_segmento(void* estructura, tipo_segmento tipo_segmento) {

	t_segmento* segmento = malloc(sizeof(t_segmento));

	segmento->numero_de_segmento = contador_segmento;
	segmento->tipo_segmento = tipo_segmento;

	segmento->inicio = base_segmento;

	switch(tipo_segmento){
		case PATOTA:
			segmento->tamanio_segmento = tamanio_patota;
			segmento->id_segmento = ((t_pcb*)estructura)->pid;
			guardar_patota(estructura);
			break;
		case TAREAS:
			segmento->tamanio_segmento = sizeof(t_tarea) * list_size(estructura);
			segmento->id_segmento = 0;
			guardar_tareas(estructura);
			break;
		case TRIPULANTE:
			segmento->tamanio_segmento = tamanio_tripulante;
			segmento->id_segmento = ((t_tcb*)estructura)->id_tripulante;
			guardar_tripulante(estructura);
			break;
		default:
			break;
	}

	segmento->estado_segmento = OCUPADO;

	contador_segmento++;

	memoria_restante -= segmento->tamanio_segmento;

	list_add(segmentos, segmento);

	sem_post(crear_segmento_sem);

	return segmento;

}

// Implementacion para obtener un segmento libre (segun BEST FIT o FIRST FIT)
bool memoria_igual_o_mas_grande(t_segmento* segmento, uint32_t tamanio_buscado)
{
	return tamanio_buscado >= segmento->tamanio_segmento;
}

bool menor_a_mayor(t_segmento* segmento, t_segmento* segmento_siguiente)
{
	return (segmento->tamanio_segmento < segmento_siguiente->tamanio_segmento);
}

bool esta_libre(void* segmento) {
	return ((t_segmento*)segmento)->estado_segmento == LIBRE;
}

t_list* segmentos_libres(void) {

	return list_filter(segmentos, (void*) esta_libre);
}

t_segmento* obtener_segmento_libre(uint32_t tamanio_buscado)
{

	t_list* segmentos_vacios = list_create();
	segmentos_vacios = segmentos_libres();

	if(criterio_elegido == BEST_FIT){
		if(list_size(segmentos_vacios) > 1){

			t_list* segmentos_con_espacio = list_filter(segmentos_vacios, (void*)memoria_igual_o_mas_grande);
			t_list* segmentos_con_espacio_ordenados = list_sorted(segmentos_con_espacio, (void*)menor_a_mayor);

			t_segmento* mejor_segmento = (t_segmento*) list_get(segmentos_con_espacio_ordenados, 0);

			return mejor_segmento;
		}

	}
	else if(criterio_elegido == FIRST_FIT){
		if(list_size(segmentos_vacios) >1) {
			t_segmento* primer_segmento = (t_segmento*) list_find(segmentos_vacios, (void*)memoria_igual_o_mas_grande);

			return primer_segmento;
		}
	}

	return NULL;
}

bool validar_existencia_segmento_libre_suficiente(uint32_t tamanio_buscado) {

	bool _memoria_igual_o_mas_grande(void* segmento) {
		return tamanio_buscado <= ((t_segmento*)segmento)->tamanio_segmento;
	}

	return list_any_satisfy(segmentos_libres(), (void*) _memoria_igual_o_mas_grande);
}




void libero_segmento(t_segmento* segmento) {

	segmento->estado_segmento = LIBRE;
	memoria_libre_por_segmento += segmento->tamanio_segmento;
}





// Funciones para guardar/recuperar Patota
void guardar_patota(t_pcb* nueva_patota) {

	memcpy(memoria_principal + base_segmento, &(nueva_patota->pid), sizeof(nueva_patota->pid));
	base_segmento += sizeof(nueva_patota->pid);

	memcpy(memoria_principal + base_segmento, &(nueva_patota->tareas), sizeof(nueva_patota->tareas));
	base_segmento += sizeof(nueva_patota->tareas);

}

void recuperar_patota(t_pcb* nueva_patota) {

	memcpy(&(nueva_patota->pid), memoria_principal + base_segmento, sizeof(nueva_patota->pid));
	base_segmento += sizeof(nueva_patota->pid);

	memcpy(&(nueva_patota->tareas), memoria_principal + base_segmento, sizeof(nueva_patota->tareas));
	base_segmento += sizeof(nueva_patota->tareas);
}


// Funciones para guardar/recuperar Tareas
void guardar_tareas(t_list* tareas_de_la_patota) {

	memcpy(memoria_principal + base_segmento, &(tareas_de_la_patota), (sizeof(t_tarea) * list_size(tareas_de_la_patota)));
	base_segmento += (sizeof(t_tarea) * list_size(tareas_de_la_patota));
}

void recuperar_tareas(t_list* tareas_de_la_patota) {

	memcpy(&(tareas_de_la_patota), memoria_principal + base_segmento, (sizeof(t_tarea) * list_size(tareas_de_la_patota)));
	base_segmento += (sizeof(t_tarea) * list_size(tareas_de_la_patota));
}


// Funciones para guardar/recuperar Tripulante
void guardar_tripulante(t_tcb* nuevo_tripulante) {

	memcpy(memoria_principal + base_segmento, &(nuevo_tripulante->id_tripulante), sizeof(nuevo_tripulante->id_tripulante));
	base_segmento += sizeof(nuevo_tripulante->id_tripulante);

	memcpy(memoria_principal + base_segmento, &(nuevo_tripulante->estado_tripulante), sizeof(nuevo_tripulante->estado_tripulante));
	base_segmento += sizeof(nuevo_tripulante->estado_tripulante);

	memcpy(memoria_principal + base_segmento, &(nuevo_tripulante->posicion_x), sizeof(nuevo_tripulante->posicion_x));
	base_segmento += sizeof(nuevo_tripulante->posicion_x);

	memcpy(memoria_principal + base_segmento, &(nuevo_tripulante->posicion_y), sizeof(nuevo_tripulante->posicion_y));
	base_segmento += sizeof(nuevo_tripulante->posicion_y);

	memcpy(memoria_principal + base_segmento, &(nuevo_tripulante->id_tarea_a_realizar), sizeof(nuevo_tripulante->id_tarea_a_realizar));
	base_segmento += sizeof(nuevo_tripulante->id_tarea_a_realizar);

	memcpy(memoria_principal + base_segmento, &(nuevo_tripulante->puntero_PCB), sizeof(nuevo_tripulante->puntero_PCB));
	base_segmento += sizeof(nuevo_tripulante->puntero_PCB);

}

void recuperar_tripulante(t_tcb* nuevo_tripulante) {

	memcpy(&(nuevo_tripulante->id_tripulante), memoria_principal + base_segmento, sizeof(nuevo_tripulante->id_tripulante));
	base_segmento += sizeof(nuevo_tripulante->id_tripulante);

	memcpy(&(nuevo_tripulante->estado_tripulante), memoria_principal + base_segmento, sizeof(nuevo_tripulante->estado_tripulante));
	base_segmento += sizeof(nuevo_tripulante->estado_tripulante);

	memcpy(&(nuevo_tripulante->posicion_x), memoria_principal + base_segmento, sizeof(nuevo_tripulante->posicion_x));
	base_segmento += sizeof(nuevo_tripulante->posicion_x);

	memcpy(&(nuevo_tripulante->posicion_y), memoria_principal + base_segmento, sizeof(nuevo_tripulante->posicion_y));
	base_segmento += sizeof(nuevo_tripulante->posicion_y);

	memcpy(&(nuevo_tripulante->id_tarea_a_realizar), memoria_principal + base_segmento, sizeof(nuevo_tripulante->id_tarea_a_realizar));
	base_segmento += sizeof(nuevo_tripulante->id_tarea_a_realizar);

	memcpy(&(nuevo_tripulante->puntero_PCB), memoria_principal + base_segmento, sizeof(nuevo_tripulante->puntero_PCB));
	base_segmento += sizeof(nuevo_tripulante->puntero_PCB);
}




void actualizar_segmento(void* estructura_actualizar, tipo_segmento tipo_segmento_a_guardar, t_segmento* segmento_libre) {

	segmento_libre->tipo_segmento = tipo_segmento_a_guardar;

	segmento_libre->estado_segmento = OCUPADO;

	switch(tipo_segmento_a_guardar){
		case PATOTA:
			guardar_patota(estructura_actualizar);
			break;
		case TAREAS:
			//segmento_libre->tamanio_segmento = sizeof(t_tarea) * list_size(estructura_actualizar);
			guardar_tareas(estructura_actualizar);
			break;
			guardar_tripulante(estructura_actualizar);
			break;
		default:
			break;
	}

	sem_post(crear_segmento_sem);
}


t_tabla_segmentos_patota* buscar_tabla_de_patota(uint32_t id_patota) {

	bool se_encuentra_patota(t_tabla_segmentos_patota* tabla){
		return tabla->patota->pid == id_patota;
	}

	 t_tabla_segmentos_patota* tabla_buscada = list_find(tablas_segmentos, se_encuentra_patota);

	return tabla_buscada;
}


t_segmento* buscar_por_id_tripulante(t_list* segmentos, tipo_segmento tipo_de_segmento, uint32_t valor) {

	bool mismo_segmento(void* segmento) {
		return (((t_segmento*)segmento)->tipo_segmento == tipo_de_segmento) && (((t_segmento*)segmento)->id_segmento == valor);
	}

	//t_list* segmentos_por_tipo = list_filter(segmentos, mismo_tipo_segmento);

	t_segmento* segmento_buscado = list_find(segmentos, mismo_segmento);

	return segmento_buscado;
}

/*
tabla general = tabla_segmentos patota

		dentro de tabla_segmentos_patota = tabla_segmentos_patota->segmentos
*/


void* obtener_contenido_de_segmento(t_segmento* segmento_a_traducir)
{
	void* contenido;

	switch(segmento_a_traducir->tipo_segmento) {
		case PATOTA:
			contenido = encontrar_patota(segmento_a_traducir);
			break;
		case TAREAS:
			contenido = encontrar_tarea(segmento_a_traducir);
			break;
		case TRIPULANTE:
			contenido = encontrar_tripulante(segmento_a_traducir);
			break;
		default:
			break;
	}

	return contenido;
}


t_pcb* encontrar_patota(t_segmento* segmento) {

	t_pcb* patota = malloc(sizeof(t_pcb));

	void* inicio = (void*) memoria_principal + segmento->inicio;
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


t_list* encontrar_tarea(t_segmento* segmento) {

	t_list* tareas_de_la_patota = list_create();

	uint32_t tamanio_segmento = segmento->tamanio_segmento;

	void* inicio = (void*) memoria_principal + segmento->inicio;
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

t_tcb* encontrar_tripulante(t_segmento* segmento) {

	t_tcb* tripulante = malloc(sizeof(t_tcb));

//TODO revisar copia de memoria en estructura tripulante

	void* inicio = (void*) memoria_principal + segmento->inicio;
	uint32_t desplazamiento = 0;

	memcpy(&(tripulante->id_tripulante), inicio + desplazamiento, sizeof(tripulante->id_tripulante));
	desplazamiento += sizeof(tripulante->id_tripulante);

	memcpy(&(tripulante->estado_tripulante), inicio + desplazamiento, sizeof(tripulante->estado_tripulante));
	desplazamiento += sizeof(tripulante->estado_tripulante);

	memcpy(&(tripulante->posicion_x), inicio + desplazamiento, sizeof(tripulante->posicion_x));
	desplazamiento += sizeof(tripulante->posicion_x);

	memcpy(&(tripulante->posicion_y), inicio + desplazamiento, sizeof(tripulante->posicion_y));
	desplazamiento += sizeof(tripulante->posicion_y);

	memcpy(&(tripulante->id_tarea_a_realizar), inicio + desplazamiento, sizeof(tripulante->id_tarea_a_realizar));
	desplazamiento += sizeof(tripulante->id_tarea_a_realizar);

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

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
			log_info(logger, "Las páginas tendran un tamaño de %u bytes cada una.\n", TAMANIO_PAGINA);
			log_info(logger, "Se utilizará el algoritmo de %s para reemplazar las páginas.\n", ALGORITMO_REEMPLAZO);
			break;

		case SEGMENTACION:

			esquema_elegido = 'S';
			log_info(logger, "Se utilizará el criterio de %s para colocar el segmento en memoria.\n", CRITERIO_SELECCION);
			tablas_segmentos = list_create();
			segmentos = list_create();
			memoria_libre_por_segmento = 0;
			memoria_libre_total = memoria_restante + memoria_libre_por_segmento;		// memoria_compactada = MEMORIA TOTAL LIBRE = TAMANIO_MEMORIA - memoria ocupada

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



// TODO no le estamos pasando el segmento para modificar, sino que nos basamos en el segmento libre que encuentre, pero no si le estoy pasando uno para modificarlo
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
				memoria_libre_total = memoria_restante + memoria_libre_por_segmento;
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
			segmento->id_segmento = list_size((t_list*)estructura);
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
	memoria_libre_total = memoria_restante + memoria_libre_por_segmento;

	list_add(segmentos, segmento);

	sem_post(crear_segmento_sem);

	return segmento;

}


t_segmento* crear_segmento_libre(uint32_t inicio_segmento, uint32_t tamanio_libre_segmento) {

	t_segmento* segmento = malloc(sizeof(t_segmento));

	segmento->numero_de_segmento = contador_segmento;
	segmento->tipo_segmento = VACIO;

	segmento->inicio = inicio_segmento;

	segmento->id_segmento = 0;

	segmento->tamanio_segmento = tamanio_libre_segmento;

	segmento->estado_segmento = LIBRE;

	contador_segmento++;

	list_add(segmentos, segmento);

	return segmento;
}


void liberar_segmento(t_segmento* segmento_a_liberar) {
	segmento_a_liberar->tipo_segmento = VACIO;
	segmento_a_liberar->estado_segmento = LIBRE;
	segmento_a_liberar->id_segmento = 0;
	memoria_libre_por_segmento += segmento_a_liberar->tamanio_segmento;
	memoria_libre_total = memoria_restante + memoria_libre_por_segmento;

	printf("Memoria a liberar: %u\n", memoria_libre_por_segmento);
}

// Implementacion para obtener un segmento libre (segun BEST FIT o FIRST FIT)
bool memoria_igual_o_mas_grande(t_segmento* segmento, uint32_t tamanio_buscado)
{
	return tamanio_buscado >= segmento->tamanio_segmento;
}

bool menor_a_mayor_por_segmento(void* segmento, void* segmento_siguiente) {
	return ((t_segmento*)segmento)->numero_de_segmento < ((t_segmento*)segmento_siguiente)->numero_de_segmento;
}

bool menor_a_mayor_por_pid(void* segmento, void* segmento_siguiente) {
	return ((t_tabla_segmentos_patota*)segmento)->patota->pid < ((t_tabla_segmentos_patota*)segmento_siguiente)->patota->pid;
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




// Funciones para guardar la Patota
void guardar_patota(t_pcb* nueva_patota) {

	memcpy(memoria_principal + base_segmento, &(nueva_patota->pid), sizeof(nueva_patota->pid));
	base_segmento += sizeof(nueva_patota->pid);

	memcpy(memoria_principal + base_segmento, &(nueva_patota->tareas), sizeof(nueva_patota->tareas));
	base_segmento += sizeof(nueva_patota->tareas);

}



// Funciones para guardar las Tareas

void guardar_tareas(t_list* tareas_de_la_patota) {

	uint32_t cantidad_tareas = list_size(tareas_de_la_patota);

	for(int i=0; i<cantidad_tareas; i++){
		t_tarea* tarea_a_guardar = list_get(tareas_de_la_patota, i);

		memcpy(memoria_principal + base_segmento, &(tarea_a_guardar->operacion), sizeof(tarea_a_guardar->operacion));
		base_segmento += sizeof(tarea_a_guardar->operacion);

		memcpy(memoria_principal + base_segmento, &(tarea_a_guardar->cantidad), sizeof(tarea_a_guardar->cantidad));
		base_segmento += sizeof(tarea_a_guardar->cantidad);

		memcpy(memoria_principal + base_segmento, &(tarea_a_guardar->posicion_x), sizeof(tarea_a_guardar->posicion_x));
		base_segmento += sizeof(tarea_a_guardar->posicion_x);

		memcpy(memoria_principal + base_segmento, &(tarea_a_guardar->posicion_y), sizeof(tarea_a_guardar->posicion_y));
		base_segmento += sizeof(tarea_a_guardar->posicion_y);

		memcpy(memoria_principal + base_segmento, &(tarea_a_guardar->tiempo), sizeof(tarea_a_guardar->tiempo));
		base_segmento += sizeof(tarea_a_guardar->tiempo);
	}
}


// Funciones para guardar la Tripulante
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
/*
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
*/

void actualizar_tripulante(t_tcb* tripulante, uint32_t inicio_segmento) {

	void* inicio = (void*) memoria_principal + inicio_segmento;
	uint32_t desplazamiento = 0;

	memcpy(inicio + desplazamiento, &(tripulante->id_tripulante), sizeof(tripulante->id_tripulante));
	desplazamiento += sizeof(tripulante->id_tripulante);

	memcpy(inicio + desplazamiento, &(tripulante->estado_tripulante), sizeof(tripulante->estado_tripulante));
	desplazamiento += sizeof(tripulante->estado_tripulante);

	memcpy(inicio + desplazamiento, &(tripulante->posicion_x), sizeof(tripulante->posicion_x));
	desplazamiento += sizeof(tripulante->posicion_x);

	memcpy(inicio + desplazamiento, &(tripulante->posicion_y), sizeof(tripulante->posicion_y));
	desplazamiento += sizeof(tripulante->posicion_y);

	memcpy(inicio + desplazamiento, &(tripulante->id_tarea_a_realizar), sizeof(tripulante->id_tarea_a_realizar));
	desplazamiento += sizeof(tripulante->id_tarea_a_realizar);

	memcpy(inicio + desplazamiento, &(tripulante->puntero_PCB), sizeof(tripulante->puntero_PCB));
	desplazamiento += sizeof(tripulante->puntero_PCB);

}


void actualizar_segmento(void* estructura_actualizar, tipo_segmento tipo_segmento_a_guardar, t_segmento* segmento_libre) {

	segmento_libre->tipo_segmento = tipo_segmento_a_guardar;

	segmento_libre->estado_segmento = OCUPADO;

	switch(tipo_segmento_a_guardar){
		case PATOTA:
			guardar_patota(estructura_actualizar);
			break;
		case TAREAS:
			guardar_tareas(estructura_actualizar);
			break;
		case TRIPULANTE:
			actualizar_tripulante(estructura_actualizar, segmento_libre->inicio);
			break;
		default:
			break;
	}

	sem_post(crear_segmento_sem);
}


t_tabla_segmentos_patota* buscar_tabla_de_patota(uint32_t id_patota) {

	bool se_encuentra_patota(void* tabla){
		return ((t_tabla_segmentos_patota*)tabla)->patota->pid == id_patota;
	}

	 t_tabla_segmentos_patota* tabla_buscada = list_find(tablas_segmentos, se_encuentra_patota);

	return tabla_buscada;
}


int obtener_indice(t_list* lista, void* valor) {

	int indice;
	for(int i = 0; i<list_size(lista); i++) {
		if(list_get(lista, i) == valor) {
			indice = i;
		}
	}
	return indice;
}


t_segmento* buscar_por_id(t_list* segmentos, tipo_segmento tipo_de_segmento, uint32_t valor) {

	bool mismo_segmento(void* segmento) {
		return (((t_segmento*)segmento)->tipo_segmento == tipo_de_segmento) && (((t_segmento*)segmento)->id_segmento == valor);
	}

	t_segmento* segmento_buscado = list_find(segmentos, mismo_segmento);

	return segmento_buscado;
}


t_tarea* buscar_proxima_tarea_del_tripulante(t_list* segmentos, tipo_segmento tipo_de_segmento, uint32_t id_proxima_tarea_del_tripu) {

	bool misma_tarea(void* segmento) {
		return ((t_segmento*)segmento)->tipo_segmento == tipo_de_segmento;
	}

	t_segmento* segmento_tareas = list_find(segmentos, misma_tarea);

	t_list* tareas_de_la_patota = obtener_contenido_de_segmento(segmento_tareas);

	t_tarea* tarea_buscada = list_get(tareas_de_la_patota, id_proxima_tarea_del_tripu);

	return tarea_buscada;
}




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


// TODO problemas al deserializar las tareas
t_list* encontrar_tarea(t_segmento* segmento) {

	t_list* tareas_totales = list_create();

	void* inicio = (void*) memoria_principal + segmento->inicio;
	uint32_t desplazamiento = 0;

	uint32_t cantidad_tareas = segmento->id_segmento;

	for(int i=0; i<cantidad_tareas; i++){

		t_tarea* tarea_a_sacar = malloc(sizeof(t_tarea));

		memcpy(&(tarea_a_sacar->operacion), inicio + desplazamiento, sizeof(tarea_a_sacar->operacion));
		desplazamiento += sizeof(tarea_a_sacar->operacion);

		memcpy(&(tarea_a_sacar->cantidad), inicio + desplazamiento, sizeof(tarea_a_sacar->cantidad));
		desplazamiento += sizeof(tarea_a_sacar->cantidad);

		memcpy(&(tarea_a_sacar->posicion_x), inicio + desplazamiento, sizeof(tarea_a_sacar->posicion_x));
		desplazamiento += sizeof(tarea_a_sacar->posicion_x);

		memcpy(&(tarea_a_sacar->posicion_y), inicio + desplazamiento, sizeof(tarea_a_sacar->posicion_y));
		desplazamiento += sizeof(tarea_a_sacar->posicion_y);

		memcpy(&(tarea_a_sacar->tiempo), inicio + desplazamiento, sizeof(tarea_a_sacar->tiempo));
		desplazamiento += sizeof(tarea_a_sacar->tiempo);

		list_add_in_index(tareas_totales, i, tarea_a_sacar);
	}

	if(segmento->tamanio_segmento == desplazamiento) {
			return tareas_totales;
	}
	else {
		return NULL;
	}

	/*t_list* tareas_de_la_patota = list_create();
	//malloc(sizeof(t_tarea) * segmento->id_segmento)
	void* inicio = (void*) memoria_principal + segmento->inicio;
	uint32_t desplazamiento = 0;

	memcpy(&(tareas_de_la_patota), inicio + desplazamiento, (sizeof(t_tarea) * segmento->id_segmento));
	desplazamiento += (sizeof(t_tarea) * segmento->id_segmento);

	printf("Tamanio segmento = %u\n", sizeof(t_tarea) * segmento->id_segmento);
	printf("desplazamiento = %u\n", desplazamiento);

	if(segmento->tamanio_segmento == desplazamiento) {
			return tareas_de_la_patota;
	}
	else {
		return NULL;
	}*/
}

t_tcb* encontrar_tripulante(t_segmento* segmento) {

	t_tcb* tripulante = malloc(sizeof(t_tcb));

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

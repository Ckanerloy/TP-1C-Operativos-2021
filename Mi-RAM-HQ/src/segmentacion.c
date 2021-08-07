#include "segmentacion.h"


t_tabla_segmentos_patota* crear_tabla_segmentos(void){

	t_tabla_segmentos_patota* tabla = malloc(sizeof(t_tabla_segmentos_patota));
	tabla->patota = crear_pcb();
	tabla->segmentos = list_create();
	tabla->direccion_tareas = list_create();
	tabla->direccion_tripulantes = list_create();

	return tabla;
}


t_segmento* administrar_guardar_segmento(void* estructura, tipo_estructura tipo_segmento, uint32_t tamanio, t_tabla_segmentos_patota* tabla_patota) {

	if(validar_existencia_segmento_libre_suficiente(tamanio)) {
		t_segmento* segmento_libre_a_ser_ocupado = obtener_segmento_libre(tamanio);
		int32_t diferencia = segmento_libre_a_ser_ocupado->tamanio_segmento - tamanio;

		int32_t nuevo_inicio = segmento_libre_a_ser_ocupado->inicio + tamanio;

		if(diferencia > 0){
			crear_segmento_libre(nuevo_inicio, diferencia);
			memoria_libre_total -= tamanio;
			segmento_libre_a_ser_ocupado->tamanio_segmento = tamanio;
		}
		if(diferencia == 0) {
			memoria_libre_total -= tamanio;
		}
		actualizar_segmento(estructura, tipo_segmento, segmento_libre_a_ser_ocupado, tabla_patota);
		return segmento_libre_a_ser_ocupado;
	}
	else if(memoria_libre_total > 0) {

		compactar();

		if(validar_existencia_segmento_libre_suficiente(tamanio)) {
			t_segmento* segmento_compactado = obtener_segmento_libre(tamanio);
			int32_t diferencia = segmento_compactado->tamanio_segmento - tamanio;

			int32_t nuevo_inicio = segmento_compactado->inicio + tamanio;

			if(diferencia > 0){
				crear_segmento_libre(nuevo_inicio, diferencia);
				memoria_libre_total -= tamanio;
				segmento_compactado->tamanio_segmento = tamanio;
			}
			if(diferencia == 0) {
				memoria_libre_total -= tamanio;
			}
			actualizar_segmento(estructura, tipo_segmento, segmento_compactado, tabla_patota);
			return segmento_compactado;
		}
	}
	else {
		log_error(logger, "No hay segmentos libres para poder guardar en memoria.\n");
	}
	return NULL;
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


void verificar_compactacion(void) {

	if(esquema_elegido == 'S') {
		t_list* seg_vacio = segmentos_libres();
		if(seg_vacio > 0) {
				compactar();
		}
		else {
			log_warning(logger, "No hay segmentos libres para poder compactar.\n");
		}
	}
	else{
		log_error(logger, "Se ha elegido el esquema de Paginación. ¡Para Compactar hacelo en Segmentación!\n");
	}

}


void compactar(void) {

	log_info(logger, "Inicio de rutina de compactación de memoria...\n");

	t_segmento* segmento;
	uint32_t inicio = 0;
	t_list* ocupados_ordenados = list_sorted(segmentos_ocupados(), menor_a_mayor_segun_inicio);

	for(int i=0; i<list_size(ocupados_ordenados); i++){

		segmento = (t_segmento*) list_get(ocupados_ordenados, i);

		void* aux = malloc(segmento->tamanio_segmento);

		sem_wait(mutex_copia);
		memcpy(aux, memoria_principal + segmento->inicio, segmento->tamanio_segmento);
		sem_post(mutex_copia);

		sem_wait(mutex_copia);
		memcpy(memoria_principal + inicio, aux, segmento->tamanio_segmento);
		sem_post(mutex_copia);

		segmento->inicio = inicio;
		segmento->numero_de_segmento = i;

		free(aux);

		inicio += segmento->tamanio_segmento;
	}

	t_list* seg_vacios = segmentos_libres();
	int32_t cantidad_segmentos_libres = list_size(seg_vacios);

	for(int i=0; i<list_size(seg_vacios); i++) {
		t_segmento* segmento_obtenido = list_get(seg_vacios, i);

		int indice = obtener_indice(segmentos, segmento_obtenido);
		list_remove(segmentos, indice);
		contador_segmento--;
	}

	list_clean_and_destroy_elements(seg_vacios, free);

	crear_segmento_libre(inicio, memoria_libre_total);

	log_info(logger, "Se compactaron %u segmentos libres de la memoria.\n", cantidad_segmentos_libres);
}


void liberar_segmento(t_segmento* segmento_a_liberar) {
	segmento_a_liberar->tipo_segmento = VACIO;
	segmento_a_liberar->estado_segmento = LIBRE;
	segmento_a_liberar->id_segmento = 0;
	memoria_libre_total += segmento_a_liberar->tamanio_segmento;
}


// FUNCIONES PARA ORDENAR O USAR EN LISTAS
// Implementacion para obtener un segmento libre (segun BEST FIT o FIRST FIT)
bool menor_a_mayor_por_segmento(void* segmento, void* segmento_siguiente) {
	return ((t_segmento*)segmento)->numero_de_segmento < ((t_segmento*)segmento_siguiente)->numero_de_segmento;}


bool menor_a_mayor_por_pid(void* segmento, void* segmento_siguiente) {
	return ((t_tabla_segmentos_patota*)segmento)->patota->pid < ((t_tabla_segmentos_patota*)segmento_siguiente)->patota->pid;}


bool menor_a_mayor_segun_inicio(void* segmento, void* segmento_siguiente) {
	return ((t_segmento*)segmento)->inicio < ((t_segmento*)segmento_siguiente)->inicio;}


bool menor_a_mayor_segun_tamanio(void* segmento, void* segmento_siguiente) {
	return ((t_segmento*)segmento)->tamanio_segmento < ((t_segmento*)segmento_siguiente)->tamanio_segmento;}


bool esta_libre(void* segmento) {
	return ((t_segmento*)segmento)->estado_segmento == LIBRE;}


bool esta_ocupado(void* segmento) {
	return ((t_segmento*)segmento)->estado_segmento == OCUPADO;}


t_list* segmentos_libres(void) {
	return list_filter(segmentos, (void*) esta_libre);}


t_list* segmentos_ocupados(void) {
	return list_filter(segmentos, (void*) esta_ocupado);}


t_segmento* obtener_segmento_libre(uint32_t tamanio_buscado) {

	t_list* segmentos_vacios = segmentos_libres();

	bool memoria_igual_o_mas_grande(void* segmento) {
		return tamanio_buscado <= ((t_segmento*)segmento)->tamanio_segmento;
	}

	if(criterio_elegido == BEST_FIT){
		if(list_size(segmentos_vacios) >= 1){

			t_list* segmentos_con_espacio =	list_filter(segmentos_vacios, memoria_igual_o_mas_grande);

			t_list* segmentos_con_espacio_ordenados = list_sorted(segmentos_con_espacio, menor_a_mayor_segun_tamanio);

			t_segmento* mejor_segmento = list_remove(segmentos_con_espacio_ordenados, 0);

			list_destroy(segmentos_vacios);
			list_destroy(segmentos_con_espacio);
			list_destroy(segmentos_con_espacio_ordenados);

			return mejor_segmento;
		}

	}
	else if(criterio_elegido == FIRST_FIT){
		if(list_size(segmentos_vacios) >= 1) {

			t_segmento* primer_segmento = list_find(segmentos_vacios, memoria_igual_o_mas_grande);

			list_destroy(segmentos_vacios);
			return primer_segmento;
		}
	}
	free(segmentos_vacios);
	return NULL;
}


bool validar_existencia_segmento_libre_suficiente(uint32_t tamanio_buscado) {

	t_list* segmentos_vacios = segmentos_libres();

	bool _memoria_igual_o_mas_grande(void* segmento) {
		return tamanio_buscado <= ((t_segmento*)segmento)->tamanio_segmento;
	}

	bool respuesta = list_any_satisfy(segmentos_vacios, (void*) _memoria_igual_o_mas_grande);

	list_destroy(segmentos_vacios);

	return respuesta;
}


// ACTUALIZACIONES EN MEMORIA
void actualizar_patota(t_pcb* patota, uint32_t inicio_segmento) {

	void* inicio = (void*) memoria_principal + inicio_segmento;
	uint32_t desplazamiento = 0;

	sem_wait(mutex_copia);
	memcpy(inicio + desplazamiento, &(patota->pid), sizeof(patota->pid));
	desplazamiento += sizeof(patota->pid);
	sem_post(mutex_copia);

	sem_wait(mutex_copia);
	memcpy(inicio + desplazamiento, &(patota->tareas), sizeof(patota->tareas));
	desplazamiento += sizeof(patota->tareas);
	sem_post(mutex_copia);
}


void actualizar_tareas(t_list* tareas_de_la_patota, uint32_t inicio_segmento, t_tabla_segmentos_patota* tabla_patota) {

	void* inicio = (void*) memoria_principal + inicio_segmento;
	uint32_t desplazamiento = 0;

	for(int i=0; i<list_size(tareas_de_la_patota); i++) {
		tarea* tarea_a_guardar = list_get(tareas_de_la_patota, i);
		t_dl_tarea* direccion_tarea = malloc(sizeof(t_dl_tarea));

		direccion_tarea->id_tarea = i;
		direccion_tarea->direccion_logica = desplazamiento;

		sem_wait(mutex_copia);
		memcpy(inicio + desplazamiento, tarea_a_guardar->tarea, tarea_a_guardar->tamanio_tarea);
		desplazamiento += tarea_a_guardar->tamanio_tarea;
		sem_post(mutex_copia);

		direccion_tarea->tamanio_tarea = tarea_a_guardar->tamanio_tarea;

		list_add_in_index(tabla_patota->direccion_tareas, i, direccion_tarea);

		free(tarea_a_guardar->tarea);
		free(tarea_a_guardar);
	}
}


void actualizar_tripulante(t_tcb* tripulante, uint32_t inicio_segmento, t_tabla_segmentos_patota* tabla_patota) {

	void* inicio = (void*) memoria_principal + inicio_segmento;
	uint32_t desplazamiento = 0;

	t_dl_tripulante* direccion_tripulante = malloc(sizeof(t_dl_tripulante));
	direccion_tripulante->direccion_logica = inicio_segmento;
	direccion_tripulante->id_tripulante = tripulante->id_tripulante;

	list_add(tabla_patota->direccion_tripulantes, direccion_tripulante);

	sem_wait(mutex_copia);
	memcpy(inicio + desplazamiento, &(tripulante->id_tripulante), sizeof(tripulante->id_tripulante));
	desplazamiento += sizeof(tripulante->id_tripulante);
	sem_post(mutex_copia);

	sem_wait(mutex_copia);
	memcpy(inicio + desplazamiento, &(tripulante->estado_tripulante), sizeof(tripulante->estado_tripulante));
	desplazamiento += sizeof(tripulante->estado_tripulante);
	sem_post(mutex_copia);

	sem_wait(mutex_copia);
	memcpy(inicio + desplazamiento, &(tripulante->posicion_x), sizeof(tripulante->posicion_x));
	desplazamiento += sizeof(tripulante->posicion_x);
	sem_post(mutex_copia);

	sem_wait(mutex_copia);
	memcpy(inicio + desplazamiento, &(tripulante->posicion_y), sizeof(tripulante->posicion_y));
	desplazamiento += sizeof(tripulante->posicion_y);
	sem_post(mutex_copia);

	sem_wait(mutex_copia);
	memcpy(inicio + desplazamiento, &(tripulante->id_tarea_a_realizar), sizeof(tripulante->id_tarea_a_realizar));
	desplazamiento += sizeof(tripulante->id_tarea_a_realizar);
	sem_post(mutex_copia);

	sem_wait(mutex_copia);
	memcpy(inicio + desplazamiento, &(tripulante->puntero_PCB), sizeof(tripulante->puntero_PCB));
	desplazamiento += sizeof(tripulante->puntero_PCB);
	sem_post(mutex_copia);
}


void actualizar_segmento(void* estructura_actualizar, tipo_estructura tipo_segmento_a_guardar, t_segmento* segmento_libre, t_tabla_segmentos_patota* tabla_patota) {

	segmento_libre->tipo_segmento = tipo_segmento_a_guardar;

	segmento_libre->estado_segmento = OCUPADO;

	switch(tipo_segmento_a_guardar){
		case PATOTA:
			segmento_libre->id_segmento = ((t_pcb*)estructura_actualizar)->pid;
			actualizar_patota(estructura_actualizar, segmento_libre->inicio);
			break;
		case TAREAS:
			segmento_libre->id_segmento = list_size(estructura_actualizar);
			actualizar_tareas(estructura_actualizar, segmento_libre->inicio, tabla_patota);
			break;
		case TRIPULANTE:
			segmento_libre->id_segmento = ((t_tcb*)estructura_actualizar)->id_tripulante;
			actualizar_tripulante(estructura_actualizar, segmento_libre->inicio, tabla_patota);
			break;
		default:
			break;
	}
	sem_post(crear_segmento_sem);
}




// FUNCIONES PARA OBTENER DE MEMORIA
t_pcb* encontrar_patota(t_segmento* segmento) {

	t_pcb* patota = malloc(sizeof(t_pcb));

	void* inicio = (void*) memoria_principal + segmento->inicio;
	uint32_t desplazamiento = 0;

	sem_wait(mutex_copia);
	memcpy(&(patota->pid), inicio + desplazamiento, sizeof(patota->pid));
	desplazamiento += sizeof(patota->pid);
	sem_post(mutex_copia);

	sem_wait(mutex_copia);
	memcpy(&(patota->tareas), inicio + desplazamiento, sizeof(patota->tareas));
	desplazamiento += sizeof(patota->tareas);
	sem_post(mutex_copia);

	return patota;
}


tarea* encontrar_tarea(t_segmento* segmento, t_tabla_segmentos_patota* tabla_patota, int32_t id_tarea_buscada) {

	tarea* tarea_recuperada = malloc(sizeof(tarea));

	t_list* direccion_tarea = tabla_patota->direccion_tareas;

	bool mismo_id_tarea(void* dl_tarea) {
		return (((t_dl_tarea*)dl_tarea)->id_tarea == id_tarea_buscada);
	}

	t_dl_tarea* direccion_logica_tarea = list_find(direccion_tarea, mismo_id_tarea);

	sem_wait(mutex_copia);
	tarea_recuperada->tamanio_tarea = direccion_logica_tarea->tamanio_tarea;
	tarea_recuperada->tarea = malloc(tarea_recuperada->tamanio_tarea);
	int32_t inicio = segmento->inicio + direccion_logica_tarea->direccion_logica;
	memcpy(tarea_recuperada->tarea, memoria_principal + inicio, tarea_recuperada->tamanio_tarea);
	sem_post(mutex_copia);

	return tarea_recuperada;
}


t_tcb* encontrar_tripulante(t_segmento* segmento) {

	t_tcb* tripulante = malloc(sizeof(t_tcb));

	void* inicio = (void*) memoria_principal + segmento->inicio;
	uint32_t desplazamiento = 0;

	sem_wait(mutex_copia);
	memcpy(&(tripulante->id_tripulante), inicio + desplazamiento, sizeof(tripulante->id_tripulante));
	desplazamiento += sizeof(tripulante->id_tripulante);
	sem_post(mutex_copia);

	sem_wait(mutex_copia);
	memcpy(&(tripulante->estado_tripulante), inicio + desplazamiento, sizeof(tripulante->estado_tripulante));
	desplazamiento += sizeof(tripulante->estado_tripulante);
	sem_post(mutex_copia);

	sem_wait(mutex_copia);
	memcpy(&(tripulante->posicion_x), inicio + desplazamiento, sizeof(tripulante->posicion_x));
	desplazamiento += sizeof(tripulante->posicion_x);
	sem_post(mutex_copia);

	sem_wait(mutex_copia);
	memcpy(&(tripulante->posicion_y), inicio + desplazamiento, sizeof(tripulante->posicion_y));
	desplazamiento += sizeof(tripulante->posicion_y);
	sem_post(mutex_copia);

	sem_wait(mutex_copia);
	memcpy(&(tripulante->id_tarea_a_realizar), inicio + desplazamiento, sizeof(tripulante->id_tarea_a_realizar));
	desplazamiento += sizeof(tripulante->id_tarea_a_realizar);
	sem_post(mutex_copia);

	sem_wait(mutex_copia);
	memcpy(&(tripulante->puntero_PCB), inicio + desplazamiento, sizeof(tripulante->puntero_PCB));
	desplazamiento += sizeof(tripulante->puntero_PCB);
	sem_post(mutex_copia);

	return tripulante;
}


// FUNCIONES DE BÚSQUEDA
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


t_segmento* buscar_por_id(t_list* segmentos, tipo_estructura tipo_de_segmento, uint32_t valor) {

	bool mismo_segmento(void* segmento) {
		return (((t_segmento*)segmento)->tipo_segmento == tipo_de_segmento) && (((t_segmento*)segmento)->id_segmento == valor);
	}

	t_segmento* segmento_buscado = list_find(segmentos, mismo_segmento);

	return segmento_buscado;
}


t_tarea* buscar_proxima_tarea_del_tripulante_segmentacion(t_list* segmentos, tipo_estructura tipo_de_segmento, int32_t id_proxima_tarea_del_tripu, t_tabla_segmentos_patota* tabla_patota) {

	bool misma_tarea(void* segmento) {
		return ((t_segmento*)segmento)->tipo_segmento == tipo_de_segmento;
	}

	t_segmento* segmento_tareas = list_find(segmentos, misma_tarea);

	tarea* tarea_pedida = encontrar_tarea(segmento_tareas, tabla_patota, id_proxima_tarea_del_tripu);

	t_tarea* tarea_a_retornar = obtener_la_tarea(tarea_pedida->tarea);

	free(tarea_pedida->tarea);
	free(tarea_pedida);

	return tarea_a_retornar;
}

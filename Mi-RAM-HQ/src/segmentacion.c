#include "segmentacion.h"


t_tabla_segmentos_patota* crear_tabla_segmentos(t_pcb* nueva_patota){

	t_tabla_segmentos_patota* tabla = malloc(sizeof(t_tabla_segmentos_patota));
	tabla->patota = malloc(sizeof(t_pcb));
	tabla->patota = nueva_patota;
	tabla->segmentos = list_create();

	return tabla;
}


t_segmento* administrar_guardar_segmento(void* estructura, tipo_estructura tipo_segmento, uint32_t tamanio) {

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
		actualizar_segmento(estructura, tipo_segmento, segmento_libre_a_ser_ocupado);
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
				//memoria_libre_por_segmento -= tamanio;
				//memoria_libre_total = memoria_restante + memoria_libre_por_segmento;
				memoria_libre_total -= tamanio;
				segmento_compactado->tamanio_segmento = tamanio;
			}
			if(diferencia == 0) {
				//memoria_libre_por_segmento -= tamanio;
				//memoria_libre_total = memoria_restante + memoria_libre_por_segmento;
				memoria_libre_total -= tamanio;
			}
			actualizar_segmento(estructura, tipo_segmento, segmento_compactado);
			return segmento_compactado;
		}
	}
	else {
		log_error(logger, "No hay segmentos libres para poder guardar en memoria.\n");
	}
	return NULL;
}


t_segmento* crear_segmento(void* estructura, tipo_estructura tipo_segmento, uint32_t tamanio) {

	t_segmento* segmento = malloc(sizeof(t_segmento));

	segmento->numero_de_segmento = contador_segmento;
	segmento->tipo_segmento = tipo_segmento;

	segmento->inicio = base_segmento;

	switch(tipo_segmento){
		case PATOTA:
			segmento->tamanio_segmento = tamanio;
			segmento->id_segmento = ((t_pcb*)estructura)->pid;
			guardar_patota(estructura);
			break;
		case TAREAS:
			segmento->tamanio_segmento = tamanio;
			segmento->id_segmento = 0;
			guardar_tareas(estructura);
			break;
		case TRIPULANTE:
			segmento->tamanio_segmento = tamanio;
			segmento->id_segmento = ((t_tcb*)estructura)->id_tripulante;
			guardar_tripulante(estructura);
			break;
		default:
			break;
	}

	segmento->estado_segmento = OCUPADO;

	contador_segmento++;

	//memoria_restante -= segmento->tamanio_segmento;
	//memoria_libre_total = memoria_restante + memoria_libre_por_segmento;
	memoria_libre_total -= segmento->tamanio_segmento;

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


void verificar_compactacion(void) {

	if(esquema_elegido == 'S') {
		t_list* seg_vacio = segmentos_libres();
		if(seg_vacio > 0) {
				//free(seg_vacio);
				compactar();
		}
		else {
			//free(seg_vacio);
			log_warning(logger, "No hay segmentos libres para poder compactar.\n");
		}
	}
	else{
		log_error(logger, "Se ha elegido el esquema de Paginación. ¡Para Compactar hacelo en Segmentación!\n");
	}


}

/*
void compactar(void) {

	if(esquema_elegido == 'S') {
		log_info(logger, "Inicio de rutina de compactación de memoria...\n");
		t_list* segmentos_ordenados = list_sorted(segmentos, menor_a_mayor_segun_inicio);

		uint32_t num_segmento_anterior = 0;
		uint32_t inicio = 0;

		printf("Cantidad de Segmentos: %u\n", list_size(segmentos));

		for(int i=0; i<list_size(segmentos_ordenados); i++){

			t_segmento* segmento = list_get(segmentos_ordenados, i);

			printf("\n\nEstado del Segmento: %u\n\n", segmento->estado_segmento);

			 if(segmento->estado_segmento == OCUPADO) {

				void* aux = malloc(segmento->tamanio_segmento);

				printf("Numero de segmento ANTES de COMPACTAR: %u\n", segmento->numero_de_segmento);
				printf("Inicio de segmento ANTES de COMPACTAR: %u\n", segmento->inicio);
				printf("Tamaño del segmento ANTES de COMPACTAR: %u\n", segmento->tamanio_segmento);

				memcpy(aux, memoria_principal + segmento->inicio, segmento->tamanio_segmento);

				memcpy(memoria_principal + inicio, aux, segmento->tamanio_segmento);

				segmento->inicio = inicio;
				segmento->numero_de_segmento = num_segmento_anterior;

				printf("Numero de segmento DESPUES de COMPACTAR: %u\n", segmento->numero_de_segmento);
				printf("Inicio de segmento DESPUES de COMPACTAR: %u\n", segmento->inicio);
				printf("Tamaño del segmento DESPUES de COMPACTAR: %u\n", segmento->tamanio_segmento);

				free(aux);

				inicio += segmento->tamanio_segmento;

				num_segmento_anterior = segmento->numero_de_segmento;
			 }
			 else {
				 num_segmento_anterior = segmento->numero_de_segmento;

				 inicio = segmento->inicio;

				 int indice = obtener_indice(segmentos, segmento);
				 list_remove(segmentos, indice);
				 free(segmento);
				 contador_segmento--;
			 }

		}

		crear_segmento_libre(inicio, memoria_libre_total);

		printf("Cantidad de Segmentos Libres despues de compactar: %u\n", list_size(segmentos_libres()));
		printf("Cantidad de Segmentos despues de crear el segmento: %u\n", list_size(segmentos));

		printf("Incio de segmento libre compactado: %u\n", inicio);
		printf("Tamaño de segmento libre compactado: %u\n", memoria_libre_total);

		memoria_libre_por_segmento = memoria_libre_total; 						//toda la memoria libre esta dentro de un segmento gigante
		memoria_restante = 0; 													// la memoria suelta queda en cero por que esta toda en la memoria libre por segmento

		log_info(logger, "Se compactó la memoria.\n");
	}
	else if(esquema_elegido == 'P') {
		log_warning(logger, "Se ha elegido el esquema de Paginación. ¡Para Compactar hacelo en Segmentación!\n");
	}
}*/


void compactar(void) {

	log_info(logger, "Inicio de rutina de compactación de memoria...\n");

	t_segmento* segmento;
	uint32_t inicio = 0;
	t_list* ocupados_ordenados = list_sorted(segmentos_ocupados(), menor_a_mayor_segun_inicio);

	//printf("Cantidad de Segmentos Libres: %u\n", list_size(segmentos_libres()));
	//printf("Cantidad de Segmentos: %u\n\n", list_size(segmentos));

	for(int i=0; i<list_size(ocupados_ordenados); i++){

		segmento = (t_segmento*) list_get(ocupados_ordenados, i);

		void* aux = malloc(segmento->tamanio_segmento);

		printf("Numero de segmento ANTES de COMPACTAR: %u\n", segmento->numero_de_segmento);
		printf("Inicio de segmento ANTES de COMPACTAR: %u\n", segmento->inicio);
		printf("Tamaño del segmento ANTES de COMPACTAR: %u\n", segmento->tamanio_segmento);

		memcpy(aux, memoria_principal + segmento->inicio, segmento->tamanio_segmento);

		memcpy(memoria_principal + inicio, aux, segmento->tamanio_segmento);

		segmento->inicio = inicio;
		segmento->numero_de_segmento = i;

		printf("Numero de segmento DESPUES de COMPACTAR: %u\n", segmento->numero_de_segmento);
		printf("Inicio de segmento DESPUES de COMPACTAR: %u\n", segmento->inicio);
		printf("Tamaño del segmento DESPUES de COMPACTAR: %u\n\n", segmento->tamanio_segmento);

		free(aux);

		inicio += segmento->tamanio_segmento;
	}
	t_list* seg_vacios = segmentos_libres();

	for(int c=0; c<list_size(segmentos); c++) {
		t_segmento* segmento_buscado = list_get(segmentos, c);

		printf("\n\nEstado del Segmento: %u\n\n", segmento_buscado->estado_segmento);

		if(segmento_buscado->estado_segmento == LIBRE) {
			t_segmento* segmento_removido = list_remove(segmentos, c);
			free(segmento_removido);
			contador_segmento--;
		}
	}

	printf("Cantidad de Segmentos despues de compactar: %u\n", list_size(segmentos));
	printf("Cantidad de Segmentos Libres despues de compactar: %u\n\n", list_size(seg_vacios));

	crear_segmento_libre(inicio, memoria_libre_total);

	printf("Cantidad de Segmentos Libres despues de compactar: %u\n", list_size(seg_vacios));
	printf("Cantidad de Segmentos despues de crear el segmento: %u\n", list_size(segmentos));

	printf("Incio de segmento libre compactado: %u\n", inicio);
	printf("Tamaño de segmento libre compactado: %u\n", memoria_libre_total);
	free(seg_vacios);
	log_info(logger, "Se compactó la memoria.\n");
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
	return ((t_segmento*)segmento)->numero_de_segmento < ((t_segmento*)segmento_siguiente)->numero_de_segmento;
}


bool menor_a_mayor_por_pid(void* segmento, void* segmento_siguiente) {
	return ((t_tabla_segmentos_patota*)segmento)->patota->pid < ((t_tabla_segmentos_patota*)segmento_siguiente)->patota->pid;
}


bool menor_a_mayor_segun_inicio(void* segmento, void* segmento_siguiente) {
	return ((t_segmento*)segmento)->inicio < ((t_segmento*)segmento_siguiente)->inicio;
}


bool menor_a_mayor_segun_tamanio(void* segmento, void* segmento_siguiente) {
	return ((t_segmento*)segmento)->tamanio_segmento < ((t_segmento*)segmento_siguiente)->tamanio_segmento;
}


bool esta_libre(void* segmento) {
	return ((t_segmento*)segmento)->estado_segmento == LIBRE;
}


bool esta_ocupado(void* segmento) {
	return ((t_segmento*)segmento)->estado_segmento == OCUPADO;
}


t_list* segmentos_libres(void) {
	t_list* seg_vacios = list_filter(segmentos, (void*) esta_libre);
	return seg_vacios;
}


t_list* segmentos_ocupados(void) {
	return list_filter(segmentos, (void*) esta_ocupado);
}


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

			free(segmentos_vacios);
			free(segmentos_con_espacio);
			free(segmentos_con_espacio_ordenados);

			return mejor_segmento;
		}

	}
	else if(criterio_elegido == FIRST_FIT){
		if(list_size(segmentos_vacios) >= 1) {

			t_segmento* primer_segmento = list_find(segmentos_vacios, memoria_igual_o_mas_grande);

			free(segmentos_vacios);
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
	free(segmentos_vacios);
	return respuesta;
}




// FUNCIONES PARA GUARDAR EN MEMORIA (SEGMENTACION)
void guardar_patota(t_pcb* nueva_patota) {

	memcpy(memoria_principal + base_segmento, &(nueva_patota->pid), sizeof(nueva_patota->pid));
	base_segmento += sizeof(nueva_patota->pid);

	memcpy(memoria_principal + base_segmento, &(nueva_patota->tareas), sizeof(nueva_patota->tareas));
	base_segmento += sizeof(nueva_patota->tareas);
}


void guardar_tareas(tareas_patota* tareas_de_la_patota) {

	memcpy(memoria_principal + base_segmento, tareas_de_la_patota->tareas, tareas_de_la_patota->tamanio_tareas);
	base_segmento += tareas_de_la_patota->tamanio_tareas;

}


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




// ACTUALIZACIONES EN MEMORIA (SEGMENTACION)
void actualizar_patota(t_pcb* patota, uint32_t inicio_segmento) {

	void* inicio = (void*) memoria_principal + inicio_segmento;
	uint32_t desplazamiento = 0;

	memcpy(inicio + desplazamiento, &(patota->pid), sizeof(patota->pid));
	desplazamiento += sizeof(patota->pid);

	memcpy(inicio + desplazamiento, &(patota->tareas), sizeof(patota->tareas));
	desplazamiento += sizeof(patota->tareas);
}


void actualizar_tareas(tareas_patota* tareas_de_la_patota, uint32_t inicio_segmento) {

	void* inicio = (void*) memoria_principal + inicio_segmento;
	uint32_t desplazamiento = 0;

	memcpy(inicio + desplazamiento, tareas_de_la_patota->tareas, tareas_de_la_patota->tamanio_tareas);
	desplazamiento += tareas_de_la_patota->tamanio_tareas;
}


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


void actualizar_segmento(void* estructura_actualizar, tipo_estructura tipo_segmento_a_guardar, t_segmento* segmento_libre) {

	segmento_libre->tipo_segmento = tipo_segmento_a_guardar;

	segmento_libre->estado_segmento = OCUPADO;

	switch(tipo_segmento_a_guardar){
		case PATOTA:
			segmento_libre->id_segmento = ((t_pcb*)estructura_actualizar)->pid;
			actualizar_patota(estructura_actualizar, segmento_libre->inicio);
			break;
		case TAREAS:
			segmento_libre->id_segmento = 0;
			actualizar_tareas(estructura_actualizar, segmento_libre->inicio);
			break;
		case TRIPULANTE:
			segmento_libre->id_segmento = ((t_tcb*)estructura_actualizar)->id_tripulante;
			actualizar_tripulante(estructura_actualizar, segmento_libre->inicio);
			break;
		default:
			break;
	}
	sem_post(crear_segmento_sem);
}




// FUNCIONES PARA OBTENER DE MEMORIA
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


void* encontrar_tarea(t_segmento* segmento) {

	void* inicio = (void*) memoria_principal + segmento->inicio;
	uint32_t desplazamiento = 0;

	char* tareas = malloc(segmento->tamanio_segmento);

	memcpy(tareas, inicio + desplazamiento, segmento->tamanio_segmento);
	desplazamiento += segmento->tamanio_segmento;

	if(segmento->tamanio_segmento == desplazamiento) {
			return tareas;
	}
	else {
		free(tareas);
		return NULL;
	}
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


t_tarea* buscar_proxima_tarea_del_tripulante_segmentacion(t_list* segmentos, tipo_estructura tipo_de_segmento, int32_t id_proxima_tarea_del_tripu, uint32_t tamanio_tareas) {

	bool misma_tarea(void* segmento) {
		return ((t_segmento*)segmento)->tipo_segmento == tipo_de_segmento;
	}

	t_segmento* segmento_tareas = list_find(segmentos, misma_tarea);

	char* tareas = obtener_contenido_de_segmento(segmento_tareas);

	t_list* tareas_de_la_patota = obtener_las_tareas(tareas, tamanio_tareas);

	if(list_size(tareas_de_la_patota)-1 < id_proxima_tarea_del_tripu) {
		list_destroy_and_destroy_elements(tareas_de_la_patota, free);
		free(tareas);
		return NULL;
	}
	else{
		t_tarea* tarea_buscada = list_get(tareas_de_la_patota, id_proxima_tarea_del_tripu);
		list_remove(tareas_de_la_patota, id_proxima_tarea_del_tripu);
		list_destroy_and_destroy_elements(tareas_de_la_patota, free);
		free(tareas);
		return tarea_buscada;
	}
}

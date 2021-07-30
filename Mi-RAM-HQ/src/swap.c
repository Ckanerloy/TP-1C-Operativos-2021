#include "swap.h"


void inicializar_swap(void) {

	if((TAMANIO_SWAP % TAMANIO_PAGINA) != 0) {
		log_error(logger, "No se puede iniciar la memoria, ya que no es múltiplo del Tamaño de las páginas.\n");
		abort();
	}
	cantidad_paginas_swap =	TAMANIO_SWAP / TAMANIO_PAGINA;

	log_info(logger, "Entran %d páginas en Memoria Swap.\n", cantidad_paginas_swap);

	paginas_swap = list_create();

	frames_swap = calloc(cantidad_paginas_swap, sizeof(frame_swap));
	for(int i=0; i<cantidad_paginas_swap; i++){
		frames_swap[i] = malloc(sizeof(frame_swap));
		liberar_frame_swap(i);
	}

	sem_swap = malloc(sizeof(sem_t));
	sem_init(sem_swap, 0, 1);

	sem_lru = malloc(sizeof(sem_t));
	sem_init(sem_lru, 0, 1);

	sem_clock = malloc(sizeof(sem_t));
	sem_init(sem_clock, 0, 1);

	area_swap = iniciar_area_swap();
}


void* iniciar_area_swap(void) {

	log_info(logger, "Se inició el Área de Swap con un tamaño de %u bytes.\n", TAMANIO_SWAP);

	archivo_swap = open(PATH_SWAP, O_RDWR | O_CREAT,S_IRUSR | S_IWUSR);
	ftruncate(archivo_swap, cantidad_paginas_swap * TAMANIO_PAGINA);

	log_info(logger, "La dirección del Archivo de Swap es %s\n", PATH_SWAP);

	if(archivo_swap == -1) {

		log_error(logger,"Error al abrir el archivo Swap.\n");

		free(contenido_swap);
		close(archivo_swap);
		return NULL;
	}

	else {
		contenido_swap = mmap(NULL, cantidad_paginas_swap * TAMANIO_PAGINA +1, PROT_READ | PROT_WRITE,MAP_SHARED, archivo_swap, 0);

		if(contenido_swap == MAP_FAILED){

			log_error(logger,"Error mapeando memoria Swap.\n");
		}
		return contenido_swap;
	}
}


// Devuelvo el numero de frame
int32_t buscar_frame_por_pagina(int32_t numero_pagina) {

	for(int c=0; c<cantidad_paginas_swap; c++) {
		if(frames_swap[c]->pagina == numero_pagina) {
			return c;
		}
	}
	return -1;
}


t_pagina* buscar_pagina_en_swap(int32_t numero_pagina) {

	bool misma_pagina(void* pagina) {
		return ((t_pagina*)pagina)->numero_de_pagina == numero_pagina;
	}

	t_pagina* pagina_buscada = list_find(paginas_swap, misma_pagina);

	int indice = obtener_indice(paginas_swap, pagina_buscada);

	list_remove(paginas_swap, indice);

	return pagina_buscada;
}


int32_t obtener_frame_libre_swap(void) {

	for(int frame_disponible=0; frame_disponible < cantidad_paginas_swap; frame_disponible++) {
		if(frames_swap[frame_disponible]->estado == LIBRE) {
			//log_info(logger, "El frame a asignar en Swap es el %d.\n", frame_disponible);
			return frame_disponible;
		}
	}
	log_error(logger, "No hay frames disponibles en Swap.\n");
	return -1;
}

void liberar_frame_swap(int32_t numero_frame) {

	frames_swap[numero_frame]->espacio_libre = TAMANIO_PAGINA;
	frames_swap[numero_frame]->estado = LIBRE;
	frames_swap[numero_frame]->pagina = -1;
}


int32_t frame_disponible_segun_algoritmo(void) {

	int32_t frame;

	if(algoritmo_elegido == LRU) {
		frame = aplicar_LRU();

		//log_info(logger,"El frame a asignar en memoria mediante LRU es el: %d.\n", frame);
		return frame;
	}
	else if(algoritmo_elegido == CLOCK) {
		frame = aplicar_CLOCK();

		//log_info(logger,"El frame a asignar en memoria mediante CLOCK es el: %d.\n", frame);
		return frame;
	}
	else {
		log_error(logger, "No se eligió ningún algoritmo de reemplazo.\n");
		return -1;
	}

}


int32_t aplicar_LRU(void) {

	int32_t frame_libre;

	uint32_t tiempo_mas_viejo = get_timestamp();
	int posicion;

	t_tabla_paginas_patota* tabla = NULL;
	t_tabla_paginas_patota* tabla_a_devolver = NULL;
	t_pagina* pagina_obtenida = NULL;

	sem_wait(sem_lru);
	for(int i=0; i<list_size(tablas_paginas); i++){
		tabla = list_get(tablas_paginas, i);

		for(int j=0; j<list_size(tabla->paginas); j++){
			pagina_obtenida = list_get(tabla->paginas, j);

			if(pagina_obtenida->tiempo_referencia < tiempo_mas_viejo){
				if(pagina_obtenida->P == 1){
					tiempo_mas_viejo = pagina_obtenida->tiempo_referencia;
					tabla_a_devolver = tabla;
					posicion = j;
				}

			}
		}
	}
	sem_post(sem_lru);

	if(tabla_a_devolver == NULL) {
		log_error(logger, "No hay nada cargado en Swap, no jodas que se rompe todo.\n");
		free(tabla);
		free(tabla_a_devolver);
		free(pagina_obtenida);
		exit(EXIT_FAILURE);
	}

	t_pagina* pagina_victima = list_get(tabla_a_devolver->paginas, posicion);

	uint32_t frame_a_buscar = pagina_victima->numero_de_frame;

	log_info(logger, "Se desalojará la Página %u del Frame %u.\n", pagina_victima->numero_de_pagina, frame_a_buscar);

	uint32_t inicio_frame = frame_a_buscar * TAMANIO_PAGINA;

	int32_t espacio_ocupado = frames[frame_a_buscar]->puntero_frame;

	void* buffer = malloc(espacio_ocupado);
	memcpy(buffer, memoria_principal + inicio_frame, espacio_ocupado);

	// Guardo la Página Víctima en Memoria Virtual
	sem_wait(sem_lru);
	guardar_pagina_en_swap(buffer, pagina_victima, espacio_ocupado);
	sem_post(sem_lru);

	memoria_libre_total += (TAMANIO_PAGINA - frames[frame_a_buscar]->espacio_libre);

	liberar_frame(frame_a_buscar);

	frame_libre = frame_a_buscar;

	return frame_libre;
}



int32_t aplicar_CLOCK() {

	int32_t frame_libre;

	t_tabla_paginas_patota* tabla = NULL;
	t_pagina* pagina_victima = NULL;
	t_pagina* pagina_aux = NULL;

	t_list* lista_a_iterar = list_create();

	sem_wait(sem_clock);
	for(int i=0; i<list_size(tablas_paginas); i++){
		tabla = list_get(tablas_paginas, i);

		for(int j=0; j<list_size(tabla->paginas); j++){
			t_pagina* pagina = list_get(tabla->paginas, j);

			list_add(lista_a_iterar, pagina);
		}
	}
	sem_post(sem_clock);

	if(list_is_empty(lista_a_iterar)) {
		log_error(logger, "No hay nada cargado en Swap, no jodas que se rompe todo.\n");
		free(tabla);
		free(pagina_victima);
		free(pagina_aux);
		free(lista_a_iterar);
		exit(EXIT_FAILURE);
	}

	bool menor_a_mayor_por_frame_en_clock(void* primero, void* siguiente) {
		return ((t_pagina*)primero)->numero_de_frame < ((t_pagina*)siguiente)->numero_de_frame;
	}


	bool esta_en_memoria(void* pagina) {
		return ((t_pagina*)pagina)->P == 1;
	}

	sem_wait(sem_clock);
	t_list* lista_en_memoria = list_filter(lista_a_iterar, esta_en_memoria);
	list_sort(lista_en_memoria, menor_a_mayor_por_frame_en_clock);

	int salir = 1;
	while(salir == 1){
		for(int i=0; i<list_size(lista_en_memoria); i++){

			pagina_aux = list_get(lista_en_memoria, i);

			if(pagina_aux->U == 0){
				pagina_victima = pagina_aux;
				salir = 0;
				break;
			}
		}

		if(salir == 0){
			break;
		}

		for(int j=0; j<list_size(lista_en_memoria); j++){
			pagina_aux = list_get(lista_en_memoria, j);

			if(pagina_aux->U == 0){
				pagina_victima = pagina_aux;
				salir = 0;
				break;

			}else if(pagina_aux->U == 1){
				pagina_aux->U = 0;
				log_info(logger, "FRAME %d, BIT DE USO: %d, PAGINA: %d",pagina_aux->numero_de_frame, pagina_aux->U, pagina_aux->numero_de_pagina);
			}
		}

		if(salir == 0){
			break;
		}
	}
	sem_post(sem_clock);

	// Obtengo todos los datos que se encuentran en el frame de dicha pagina, para pasarlos a SWAP
	uint32_t frame_a_buscar = pagina_victima->numero_de_frame;

	log_info(logger, "Se desalojará la Página %u del Frame %u.\n", pagina_victima->numero_de_pagina, frame_a_buscar);

	uint32_t inicio_frame = frame_a_buscar * TAMANIO_PAGINA;

	int32_t espacio_ocupado = frames[frame_a_buscar]->puntero_frame;

	void* buffer = malloc(espacio_ocupado);
	memcpy(buffer, memoria_principal + inicio_frame, espacio_ocupado);

	sem_wait(sem_clock);
	// Guardo la Página Víctima en Memoria Virtual
	guardar_pagina_en_swap(buffer, pagina_victima, espacio_ocupado);
	sem_post(sem_clock);

	memoria_libre_total += (TAMANIO_PAGINA - frames[frame_a_buscar]->espacio_libre);

	liberar_frame(frame_a_buscar);

	frame_libre = frame_a_buscar;

	return frame_libre;
}


int guardar_pagina_en_swap(void* buffer, t_pagina* pagina, int32_t espacio_ocupado) {

	int32_t frame_libre = obtener_frame_libre_swap();
	pagina->P = 0;
	pagina->numero_de_frame = -1;

	void* inicio_swap = (void*)area_swap + (frame_libre * TAMANIO_PAGINA);
	memcpy(inicio_swap, buffer, espacio_ocupado);
	inicio_swap += espacio_ocupado;
	memset(inicio_swap, '\0', TAMANIO_PAGINA - espacio_ocupado);

	frames_swap[frame_libre]->pagina = pagina->numero_de_pagina;

	frames_swap[frame_libre]->estado = OCUPADO;

	frames_swap[frame_libre]->espacio_libre = TAMANIO_PAGINA - espacio_ocupado;

	list_add(paginas_swap, pagina);

	//log_info(logger, "Se asignó la Página %u en el Frame %u de Swap.\n", pagina->numero_de_pagina, frame_libre);

	if(msync(area_swap, TAMANIO_SWAP, MS_SYNC) < 0) {
		log_error(logger,"[msync]Error al volcar los cambios a Swap.\n");
		return -1;

	}else {

		log_info(logger,"[msync]Se agregó la Pagina %u en el Frame %u de Swap exitosamente.\n", frames_swap[frame_libre]->pagina, frame_libre);
	}

	free(buffer);
	return 0;
}


void* recuperar_en_swap(int32_t numero_pagina, int32_t *espacio_usado) {

	t_pagina* pagina_recuperada = buscar_pagina_en_swap(numero_pagina);

	int32_t numero_frame = buscar_frame_por_pagina(numero_pagina);

	if(numero_frame == -1) {
		log_error(logger, "La Página %u no está cargada en ningún frame de la Memoria Swap.\n", numero_pagina);
		return 0;
	}

	int32_t espacio_ocupado = TAMANIO_PAGINA - frames_swap[numero_frame]->espacio_libre;

	void* buffer = leer_frame_en_swap(numero_frame, espacio_ocupado);

	*espacio_usado = espacio_ocupado;

	return buffer;
}


void* leer_frame_en_swap(int32_t numero_frame, int32_t espacio_ocupado) {

	void* buffer = malloc(espacio_ocupado);

	void* inicio_area_swap = (void*) area_swap + (numero_frame * TAMANIO_PAGINA);

	memcpy(buffer, inicio_area_swap, espacio_ocupado);

	liberar_frame_swap(numero_frame);

	return buffer;
}


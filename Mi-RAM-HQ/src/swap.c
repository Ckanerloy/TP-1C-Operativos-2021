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
		frames_swap[i]->estado = LIBRE;
		frames_swap[i]->pagina = -1;
	}

	sem_swap = malloc(sizeof(sem_t));
	sem_init(sem_swap, 0, 1);

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


// Necesitamos ayuda por file system TODO
void escribir_en_swap(char* buffer){
	sem_wait(sem_swap);
	FILE* archivo = fopen(dump_memoria->path_dump, "r+");
	//archivo_swap;
	fseek(archivo, 0, SEEK_END);

	fwrite(buffer, strlen(buffer), 1, archivo);

	close(archivo_swap);

	sem_post(sem_swap);
	free(buffer);
}


// Devuelvo el numero de frame
int32_t buscar_por_pagina(int32_t numero_pagina) {

	for(int c=0; c<cantidad_paginas_swap; c++) {
		if(frames_swap[c]->pagina == numero_pagina) {
			return c;
		}
	}
	return -1;
}


int32_t obtener_frame_libre_swap(void) {

	for(int frame_disponible=0; frame_disponible < cantidad_paginas_swap; frame_disponible++) {
		if(frames_swap[frame_disponible]->estado == LIBRE) {
			log_info(logger, "El frame a asignar en Swap es el %d.\n", frame_disponible);
			return frame_disponible;
		}
	}
	log_error(logger, "No hay frames disponibles en Swap.\n");
	return -1;
}




int32_t frame_disponible_segun_algoritmo(void) {

	int32_t frame;

	if(algoritmo_elegido == LRU) {
		frame = aplicar_LRU();

		log_info(logger,"El frame a asignar en memoria mediante LRU es el: %d", frame);
		return frame;
	}
	else if(algoritmo_elegido == CLOCK) {
		frame = aplicar_CLOCK();

		log_info(logger,"El frame a asignar en memoria mediante CLOCK es el: %d", frame);
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

	t_tabla_paginas_patota* tabla;
	t_tabla_paginas_patota* tabla_a_devolver;
	t_pagina* pagina_obtenida;

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

	log_info(logger,"Se reemplazará una página por el algoritmo de LRU.\n");
	t_pagina* pagina_a_remover = list_get(tabla_a_devolver->paginas, posicion);

	// Obtengo todos los datos que se encuentran en el frame de dicha pagina, para pasarlos a SWAP

	uint32_t frame_a_buscar = pagina_a_remover->numero_de_frame;
	printf("NUMERO DE PAGINA: %u\n", pagina_a_remover->numero_de_pagina);

	printf("FRAME VICTIMA: %u\n", frame_a_buscar);
	pagina_a_remover->P = 0;
	pagina_a_remover->numero_de_frame = -1;
	pagina_a_remover->tiempo_referencia = get_timestamp();

	printf("El Frame de la Página %u es %u\n", pagina_a_remover->numero_de_pagina, frame_a_buscar);

	void* buffer = malloc(TAMANIO_PAGINA);
	uint32_t inicio_frame = frame_a_buscar * TAMANIO_PAGINA;
	printf("INICIO FRAME EN LRU: %u\n", inicio_frame);

	int32_t espacio_ocupado = frames[frame_a_buscar]->puntero_frame;

	memcpy(buffer, memoria_principal + inicio_frame, espacio_ocupado);

	uint32_t desplazamiento = inicio_frame + espacio_ocupado;
	printf("DESPLAZAMIENTO LRU: %u\n", desplazamiento);

	uint32_t offset = TAMANIO_PAGINA - espacio_ocupado;
	printf("OFFSET LRU: %u\n", offset);

	log_debug(logger,"Se reemplazó la página por LRU en el Frame donde comienza en: %u", offset);

	/*
	 * 	Este buffer lo mando a SWAP
	 *
	 *	Guardo en SWAP solamente el buffer (void*) y el número de página, asi que cuando tenga que retornarlo, lo
	 *		busco por el número de página (obtenido por la Dirección Lógica) y retorno el buffer (void*) para guardarlo en un frame disponible
	 */
	 guardar_pagina_en_swap(buffer, pagina_a_remover->numero_de_pagina, espacio_ocupado);


	memoria_libre_total += (TAMANIO_PAGINA - frames[frame_a_buscar]->espacio_libre);

	liberar_frame(frame_a_buscar);

	frame_libre = frame_a_buscar;

	printf("Se libero el Frame %u\n", frame_libre);

	return frame_libre;
}



int32_t aplicar_CLOCK() {

	int32_t frame_libre;

	t_tabla_paginas_patota* tabla;
	t_pagina* pagina_obtenida;
	t_pagina* pagina_aux;

	t_list* lista_a_iterar = list_create();

	for(int i=0; i<list_size(tablas_paginas); i++){
		tabla = list_get(tablas_paginas, i);

		for(int j=0; j<list_size(tabla->paginas); j++){
			t_pagina* pagina = list_get(tabla->paginas, j);

			list_add(lista_a_iterar, pagina);
		}
	}

	bool menor_a_mayor_por_frame_en_clock(void* primero, void* siguiente) {
		return ((t_pagina*)primero)->numero_de_frame < ((t_pagina*)siguiente)->numero_de_frame;
	}


	bool esta_en_memoria(void* pagina) {
		return ((t_pagina*)pagina)->P == 1;
	}

	t_list* lista_en_memoria = list_filter(lista_a_iterar, esta_en_memoria);
	list_sort(lista_en_memoria, menor_a_mayor_por_frame_en_clock);
	printf("LA LISTA TIENE %d frames\n", list_size(lista_en_memoria));


	int salir = 1;
	while(salir == 1){
		for(int i=0; i<list_size(lista_en_memoria); i++){

			pagina_aux = list_get(lista_en_memoria, i);
			printf("FRAME %d, BIT DE USO: %d\n",pagina_aux->numero_de_frame, pagina_aux->U);

			if(pagina_aux->U == 0){
				pagina_obtenida = pagina_aux;
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
				pagina_obtenida = pagina_aux;
				salir = 0;
				break;

			}else if(pagina_aux->U == 1){
				pagina_aux->U = 0;
				printf("FRAME %d, BIT DE USO: %d\n",pagina_aux->numero_de_frame, pagina_aux->U);
			}
		}

		if(salir == 0){
			break;
		}
	}

	log_info(logger,"Se reemplazará una página por el algoritmo de CLOCK.\n");

	// Obtengo todos los datos que se encuentran en el frame de dicha pagina, para pasarlos a SWAP

	uint32_t frame_a_buscar = pagina_obtenida->numero_de_frame;
	printf("NUMERO DE PAGINA: %u\n", pagina_obtenida->numero_de_pagina);

	printf("FRAME VICTIMA: %u\n", frame_a_buscar);
	pagina_obtenida->P = 0;
	pagina_obtenida->numero_de_frame = -1;
	pagina_obtenida->tiempo_referencia = get_timestamp();

	printf("El Frame de la Página %u es %u\n", pagina_obtenida->numero_de_pagina, frame_a_buscar);

	void* buffer = malloc(TAMANIO_PAGINA);
	uint32_t inicio_frame = frame_a_buscar * TAMANIO_PAGINA;
	printf("INICIO FRAME EN CLOCK: %u\n", inicio_frame);

	int32_t espacio_ocupado = frames[frame_a_buscar]->puntero_frame;

	memcpy(buffer, memoria_principal + inicio_frame, espacio_ocupado);

	uint32_t desplazamiento = inicio_frame + espacio_ocupado;
	printf("DESPLAZAMIENTO CLOCK: %u\n", desplazamiento);

	uint32_t offset = TAMANIO_PAGINA - espacio_ocupado;
	printf("OFFSET CLOCK: %u\n", offset);

	log_debug(logger,"Se reemplazó la página por CLOCK en el Frame donde comienza en: %u", offset);

	/*
	 * 	Este buffer lo mando a SWAP
	 *
	 *	Guardo en SWAP solamente el buffer (void*) y el número de página, asi que cuando tenga que retornarlo, lo
	 *		busco por el número de página (obtenido por la Dirección Lógica) y retorno el buffer (void*) para guardarlo en un frame disponible
	 */
	guardar_pagina_en_swap(buffer, pagina_obtenida->numero_de_pagina, espacio_ocupado);


	memoria_libre_total += (TAMANIO_PAGINA - frames[frame_a_buscar]->espacio_libre);

	liberar_frame(frame_a_buscar);

	frame_libre = frame_a_buscar;

	printf("Se libero el Frame %u\n", frame_libre);

	return frame_libre;

}


// TODO
int guardar_pagina_en_swap(void* buffer, int32_t numero_pagina, int32_t espacio_ocupado) {

	int32_t frame_libre = obtener_frame_libre_swap();
	t_pagina* pagina_swap = malloc(sizeof(t_pagina));
	pagina_swap->P = 0;
	pagina_swap->U = 1;
	pagina_swap->numero_de_pagina = numero_pagina;
	pagina_swap->numero_de_frame = frame_libre;
	pagina_swap->tiempo_referencia = get_timestamp();

	void* stream = (void*) area_swap + (frame_libre * TAMANIO_PAGINA);

	memcpy(stream, buffer, espacio_ocupado);
	stream += espacio_ocupado;
	//memset(stream, '\0', TAMANIO_PAGINA - espacio_ocupado -1 );

	printf("FRAME ELEGIDO: %u\n", frame_libre);
	frames_swap[frame_libre]->pagina = pagina_swap->numero_de_pagina;
	printf("PAGINA GUARDADA: %u\n", frames_swap[frame_libre]->pagina);


	frames_swap[frame_libre]->estado = OCUPADO;
	frames_swap[frame_libre]->espacio_libre = TAMANIO_PAGINA - espacio_ocupado;
	printf("ESPACIO LIBRE FRAME SWAP: %u\n", frames_swap[frame_libre]->espacio_libre);

	list_add(paginas_swap, pagina_swap);

	if(msync(area_swap, TAMANIO_SWAP, MS_SYNC) < 0) {
		log_error(logger,"[msync]Error al volcar los cambios a Swap.\n");
		return -1;

	}else {

		log_info(logger,"[msync]Se agregó la pagina en Swap exitosamente.\n");
	}

	return 0;
}


void* recuperar_en_swap(int32_t numero_pagina, int32_t *espacio_usado) {

	int32_t numero_frame = buscar_por_pagina(numero_pagina);

	if(numero_frame == -1) {
		log_error(logger, "La página no está cargada en ningún frame de la Memoria Swap.\n");
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

	log_info(logger, "Se recuperó los datos de Swap.\n");
	return buffer;
}


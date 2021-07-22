#include "swap.h"

void inicializar_swap(void) {

	if((TAMANIO_SWAP % TAMANIO_PAGINA) != 0) {
		log_error(logger, "No se puede iniciar la memoria, ya que no es múltiplo del Tamaño de las páginas.\n");
		abort();
	}
	cantidad_paginas_swap =	TAMANIO_SWAP / TAMANIO_PAGINA;

	log_info(logger, "Entran %d páginas en Memoria Swap.\n", cantidad_paginas_swap);

	paginas_swap = list_create();

	frames_swap = malloc(sizeof(frame) * cantidad_paginas_swap);
	for(int i=0; i<cantidad_paginas_swap; i++){
		frames_swap[i] = LIBRE;
	}

	area_swap = iniciar_area_swap();
}


void* iniciar_area_swap(void) {

	log_info(logger, "Se inició el Área de Swap con un tamaño de %u bytes.\n", TAMANIO_SWAP);

	archivo_swap = open(PATH_SWAP, O_RDWR | O_CREAT,S_IRUSR | S_IWUSR);
	ftruncate(archivo_swap, cantidad_paginas_swap * TAMANIO_PAGINA);

	printf("La dirección del Archivo de Swap es %s\n", PATH_SWAP);

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


int obtener_marco_libre(void) {

	for(int marco_disponible=0; marco_disponible < cantidad_paginas_swap; marco_disponible++) {
		if(frames_swap[marco_disponible] == LIBRE) {
			log_info(logger, "El marco a asignar en Swap es el %d.\n", marco_disponible);
			return marco_disponible;
		}
	}
	log_info(logger, "No hay marcos disponibles en Swap.\n");
	return -1;
}




int32_t frame_disponible_segun_algoritmo(void) {

	if(algoritmo_elegido == LRU) {
		log_info(logger, "Se eligió el algoritmo de LRU.\n");
		return aplicar_LRU();
	}
	else if(algoritmo_elegido == CLOCK) {
		log_info(logger, "Se eligió el algoritmo de CLOCK.\n");
		return aplicar_CLOCK();
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
				tiempo_mas_viejo = pagina_obtenida->tiempo_referencia;
				tabla_a_devolver = tabla;
				posicion = j;
			}

		}
	}

	log_info(logger,"Se eliminará una página por LRU.\n");
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

	memcpy(buffer, memoria_principal + inicio_frame, TAMANIO_PAGINA);

	uint32_t desplazamiento = inicio_frame + TAMANIO_PAGINA;
	printf("DESPLAZAMIENTO LRU: %u\n", desplazamiento);

	uint32_t offset = (desplazamiento - inicio_frame) - TAMANIO_PAGINA;
	printf("OFFSET LRU: %u\n", offset);

	log_debug(logger,"Se eliminó por LRU el Frame donde comenzaba en: %u", offset);

	/*
	 * 	Este buffer lo mando a SWAP
	 *
	 */

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
	t_tabla_paginas_patota* tabla_aux;

	t_list* lista_a_iterar = list_create();

	for(int i=0; i<list_size(tablas_paginas); i++){
		tabla = list_get(tablas_paginas, i);

		for(int j=0; j<list_size(tabla->paginas); j++){
			pagina_obtenida = list_get(tabla->paginas, j);
			estructura_CLOCK* estructura_clock = malloc(sizeof(estructura_CLOCK));
			estructura_clock->pagina = pagina_obtenida;
			estructura_clock->tabla = tabla;

			list_add(lista_a_iterar, estructura_clock);
		}
	}

	bool menor_a_mayor_por_frame_en_clock(void* primero, void* siguiente) {
		return ((estructura_CLOCK*)primero)->pagina->numero_de_frame < ((estructura_CLOCK*)siguiente)->pagina->numero_de_frame;
	}


	bool esta_en_memoria(void* estructura) {
		return ((estructura_CLOCK*)estructura)->pagina->P == 1;
	}

	t_list* lista_en_memoria = list_filter(lista_a_iterar, esta_en_memoria);
	list_sort(lista_en_memoria, menor_a_mayor_por_frame_en_clock);
	printf("LA LISTA TIENE %d frames\n", list_size(lista_en_memoria));


	estructura_CLOCK* estructura_aux;

	int salir = 1;
	while(salir == 1){
		for(int i=0; i<list_size(lista_en_memoria); i++){

			estructura_aux = list_get(lista_en_memoria, i);
			printf("FRAME %d, BIT DE USO: %d\n",estructura_aux->pagina->numero_de_frame, estructura_aux->pagina->U);

			if(estructura_aux->pagina->U == 0){
				tabla_aux = estructura_aux->tabla;
				pagina_aux = estructura_aux->pagina;
				salir = 0;
				break;
			}
		}

		if(salir == 0){
			break;
		}

		for(int j=0; j<list_size(lista_en_memoria); j++){
			estructura_aux = list_get(lista_en_memoria, j);

			if(estructura_aux->pagina->U == 0){
				tabla_aux = estructura_aux->tabla;
				pagina_aux = estructura_aux->pagina;
				salir = 0;
				break;

			}else if(estructura_aux->pagina->U == 1){
				estructura_aux->pagina->U = 0;
				printf("FRAME %d, BIT DE USO: %d\n",estructura_aux->pagina->numero_de_frame, estructura_aux->pagina->U);
			}
		}

		if(salir == 0){
			break;
		}
	}

	log_info(logger,"Se eliminará una página por CLOCK.\n");
	//t_pagina* pagina_a_remover = list_get(tabla_aux->paginas, posicion);

	// Obtengo todos los datos que se encuentran en el frame de dicha pagina, para pasarlos a SWAP

	uint32_t frame_a_buscar = pagina_aux->numero_de_frame;
	printf("NUMERO DE PAGINA: %u\n", pagina_aux->numero_de_pagina);

	printf("FRAME VICTIMA: %u\n", frame_a_buscar);
	pagina_aux->P = 0;
	pagina_aux->numero_de_frame = -1;
	pagina_aux->tiempo_referencia = get_timestamp();

	printf("El Frame de la Página %u es %u\n", pagina_aux->numero_de_pagina, frame_a_buscar);

	void* buffer = malloc(TAMANIO_PAGINA);
	uint32_t inicio_frame = frame_a_buscar * TAMANIO_PAGINA;
	printf("INICIO FRAME EN CLOCK: %u\n", inicio_frame);

	memcpy(buffer, memoria_principal + inicio_frame, TAMANIO_PAGINA);

	uint32_t desplazamiento = inicio_frame + TAMANIO_PAGINA;
	printf("DESPLAZAMIENTO CLOCK: %u\n", desplazamiento);

	uint32_t offset = (desplazamiento - inicio_frame) - TAMANIO_PAGINA;
	printf("OFFSET CLOCK: %u\n", offset);

	log_debug(logger,"Se eliminó por CLOCK el Frame donde comenzaba en: %u", offset);

	/*
	 * 	Este buffer lo mando a SWAP
	 *
	 */

	memoria_libre_total += (TAMANIO_PAGINA - frames[frame_a_buscar]->espacio_libre);

	liberar_frame(frame_a_buscar);


	frame_libre = frame_a_buscar;

	printf("Se libero el Frame %u\n", frame_libre);

	return frame_libre;

}


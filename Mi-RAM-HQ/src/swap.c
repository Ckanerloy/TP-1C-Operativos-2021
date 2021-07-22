#include "swap.h"

void inicializar_swap(void) {

	if((TAMANIO_SWAP % TAMANIO_PAGINA) != 0) {
		log_error(logger, "No se puede iniciar la memoria, ya que no es múltiplo del Tamaño de las páginas.\n");
		abort();
	}
	cantidad_paginas_swap =	TAMANIO_SWAP / TAMANIO_PAGINA;

	log_info(logger, "Entran %d páginas en Memoria Swap.\n", cantidad_paginas_swap);

	paginas_swap = list_create();

	//estado_frames_en_swap = calloc(cantidad_paginas_swap, sizeof(frame));
/*	for(int i=0;i<cantidad_paginas_swap;i++){
		estado_frames_en_swap[i] = malloc(sizeof(frame));
		estado_frames_en_swap[i] = LIBRE;
		estado_frames_en_swap[i]->espacio_libre = TAMANIO_PAGINA;
		estado_frames_en_swap[i]->pagina = -1;
		estado_frames_en_swap[i]->proceso = -1;
	}*/

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
		if(estado_frames_en_swap[marco_disponible] == LIBRE) {
			log_info(logger, "El marco a asignar en Swap es el %d.\n", marco_disponible);
			return marco_disponible;
		}
	}
	log_info(logger, "No hay marcos disponibles en Swap.\n");
	return -1;
}




int32_t aplicar_LRU(void) {

	int32_t frame_libre;

	uint32_t tiempo_mas_viejo = get_timestamp();
	int posicion;

	t_tabla_paginas_patota* tabla;
	t_tabla_paginas_patota* tabla_a_devolver;
	t_pagina* pagina_obtenida;

	for(int i=0;i<list_size(tablas_paginas);i++){
		tabla = list_get(tablas_paginas, i);

		for(int j=0;j<list_size(tabla->paginas);j++){
			pagina_obtenida = list_get(tabla->paginas, j);

			if(pagina_obtenida->tiempo_referencia < tiempo_mas_viejo){
				tiempo_mas_viejo = pagina_obtenida->tiempo_referencia;
				tabla_a_devolver = tabla;
				posicion = j;
			}

		}
	}

	log_info(logger,"Se eliminará una página por LRU");
	t_pagina* pagina_a_remover = list_get(tabla_a_devolver->paginas, posicion);

	// Obtengo todos los datos que se encuentran en el frame de dicha pagina, para pasarlos a SWAP

	uint32_t frame_a_buscar = pagina_a_remover->numero_de_frame;
	pagina_a_remover->P = 0;
	pagina_a_remover->numero_de_frame = -1;

	printf("El Frame de la Página %u es %u\n", pagina_a_remover->numero_de_pagina, frame_a_buscar);

	void* buffer = malloc(TAMANIO_PAGINA);
	uint32_t inicio_frame = frame_a_buscar * TAMANIO_PAGINA;

	memcpy(buffer, memoria_principal + inicio_frame, TAMANIO_PAGINA);

	uint32_t desplazamiento = inicio_frame + TAMANIO_PAGINA;

	uint32_t offset = (desplazamiento - inicio_frame) - TAMANIO_PAGINA;

	log_debug(logger,"Se eliminó por LRU el Frame donde comenzaba en: %u", offset);

	/*
	 * 	Este buffer lo mando a SWAP
	 *
	 */

	memoria_libre_total += (TAMANIO_PAGINA - frames[frame_a_buscar]->espacio_libre);
	// Liberar Frame
	frames[frame_a_buscar]->espacio_libre = TAMANIO_PAGINA;
	frames[frame_a_buscar]->estado = LIBRE;
	frames[frame_a_buscar]->pagina = -1;
	frames[frame_a_buscar]->proceso = -1;



	frame_libre = frame_a_buscar;

	printf("Se libero el Frame %u\n", frame_libre);

	return frame_libre;
}



int32_t aplicar_CLOCK() {

	int32_t frame_libre;

	uint32_t tiempo_mas_viejo = get_timestamp();
	int posicion;

}


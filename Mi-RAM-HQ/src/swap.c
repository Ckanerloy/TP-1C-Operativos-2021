#include "swap.h"

void inicializar_swap(void) {

	cantidad_paginas_swap =	TAMANIO_SWAP / TAMANIO_PAGINA;

	log_info(logger, "Entran %d páginas en Memoria Swap.\n", cantidad_paginas_swap);

	paginas_swap = list_create();

	estado_frames_en_swap = malloc(sizeof(estado) * cantidad_paginas_swap);

	for(int i=0;i<cantidad_paginas_swap;i++){
		estado_frames_en_swap[i] = LIBRE;
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
		if(estado_frames_en_swap[marco_disponible] == LIBRE) {
			log_info(logger, "El marco a asignar en Swap es el %d.\n", marco_disponible);
			return marco_disponible;
		}
	}
	log_info(logger, "No hay marcos disponibles en Swap.\n");
	return -1;
}


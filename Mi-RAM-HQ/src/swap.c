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

	int32_t frame;

	if(algoritmo_elegido == LRU) {
		frame = aplicar_LRU();

		log_info(logger,"El frame a asignar en memoria es el: %d", frame);
		return frame;
	}
	else if(algoritmo_elegido == CLOCK) {
		frame = aplicar_CLOCK();

		log_info(logger,"El frame a asignar en memoria es el: %d", frame);
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
				tiempo_mas_viejo = pagina_obtenida->tiempo_referencia;
				tabla_a_devolver = tabla;
				posicion = j;
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

	memcpy(buffer, memoria_principal + inicio_frame, TAMANIO_PAGINA);

	uint32_t desplazamiento = inicio_frame + TAMANIO_PAGINA;
	printf("DESPLAZAMIENTO LRU: %u\n", desplazamiento);

	uint32_t offset = (desplazamiento - inicio_frame) - TAMANIO_PAGINA;
	printf("OFFSET LRU: %u\n", offset);

	log_debug(logger,"Se reemplazó la página por LRU en el Frame donde comienza en: %u", offset);

	/*
	 * 	Este buffer lo mando a SWAP
	 *
	 *	Guardo en SWAP solamente el buffer (void*) y el número de página, asi que cuando tenga que retornarlo, lo
	 *		busco por el número de página (obtenido por la Dirección Lógica) y retorno el buffer (void*) para guardarlo en un frame disponible
	 *
	 *	actualizar_pagina_en_swap(buffer, pagina_obtenida->numero_de_pagina)
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

	memcpy(buffer, memoria_principal + inicio_frame, TAMANIO_PAGINA);

	uint32_t desplazamiento = inicio_frame + TAMANIO_PAGINA;
	printf("DESPLAZAMIENTO CLOCK: %u\n", desplazamiento);

	uint32_t offset = (desplazamiento - inicio_frame) - TAMANIO_PAGINA;
	printf("OFFSET CLOCK: %u\n", offset);

	log_debug(logger,"Se reemplazó la página por CLOCK en el Frame donde comienza en: %u", offset);

	/*
	 * 	Este buffer lo mando a SWAP
	 *
	 *	Guardo en SWAP solamente el buffer (void*) y el número de página, asi que cuando tenga que retornarlo, lo
	 *		busco por el número de página (obtenido por la Dirección Lógica) y retorno el buffer (void*) para guardarlo en un frame disponible
	 *
	 *	actualizar_pagina_en_swap(buffer, pagina_obtenida->numero_de_pagina)
	 */

	memoria_libre_total += (TAMANIO_PAGINA - frames[frame_a_buscar]->espacio_libre);

	liberar_frame(frame_a_buscar);

	frame_libre = frame_a_buscar;

	printf("Se libero el Frame %u\n", frame_libre);

	return frame_libre;

}



void actualizar_pagina_en_swap() {

}

/*
int actualizarPaginaEnSwapVieja(nombre* nombreRestaurante,uint32_t idPedido,uint32_t cantTotal,uint32_t cantLista,char* nombrePlato){
	//buscar esta pagina en el frame guardado y actualizar la informacion si la pag
	//tiene el bit de modificado en 1, si no queda como estaba

	PedidoSwap* pedido = buscarPedidoSwap(idPedido,nombreRestaurante);
	ContenidoPaginaSwap* pagina = buscarPaginaEnPedidoSwap(pedido,nombrePlato);
	Pagina* paginaPosible;
	for(int i=0;i<paginasEnSwap->elements_count;i++){
		paginaPosible = list_get(paginasEnSwap,i);
		if(paginaPosible->nroPagina == pagina->nroPagina){
			paginaPosible->P = 0;
			paginaPosible->U = 0;
			char** paginaALeer = leerPaginaGuardadaEnSwap(paginaPosible->frame);
			char* total = string_itoa(cantTotal);
			char* lista = string_itoa(cantLista);
			strcpy(paginaALeer[0],total);
			strcpy(paginaALeer[1],lista);

			char* paginaActualizada = string_new();
			string_append(&paginaActualizada,paginaALeer[0]);
			string_append(&paginaActualizada,",");
			string_append(&paginaActualizada,paginaALeer[1]);
			string_append(&paginaActualizada,",");
			string_append(&paginaActualizada,nombrePlato);

			pthread_mutex_lock(&swapPagina);
			void* stream = mapeo + (tamanioPagina*paginaPosible->frame);
			memcpy(stream,paginaActualizada,strlen(paginaActualizada));
			stream += strlen(paginaActualizada);
			memset(stream, '\0', tamanioPagina - strlen(paginaActualizada));
			pthread_mutex_unlock(&swapPagina);

			//char** paginaALeerDespues = leerPaginaGuardadaEnSwap(paginaPosible->frame);

			if(msync(mapeo,strlen(paginaActualizada), MS_SYNC) < 0) {

				if(logger->is_active_console == false){
					logger->is_active_console = true;
				}
				log_error(logger,"[msync]Error al volcar los cambios a SWAP");
				return -1;
			}else{

				if(logger->is_active_console == false){
					logger->is_active_console = true;
				}
				log_info(logger,"[msync]Se guardaron los cambios de la página %s en SWAP exitosamente",nombrePlato);
			}

			liberadorDeArray(paginaALeer);
			free(total);
			free(lista);
			free(paginaActualizada);
			//free(paginaALeer);
		}
	}

	return 0;
}*/



/*
char** leerPaginaGuardadaEnSwap(int frame){
	pthread_mutex_lock(&leerPagina);
	char* pagina = malloc(tamanioPagina + 1);
	memset(pagina, '\0', tamanioPagina + 1);

	void* stream = mapeo + (tamanioPagina*frame);
	memcpy(pagina,stream,tamanioPagina);
	pthread_mutex_unlock(&leerPagina);

	char** separarPagina = string_split(pagina,",");

	if(logger->is_active_console == false){
		logger->is_active_console = true;
	}
	log_info(logger,"Se procede a leer en SWAP la página en el frame %d",frame);
	//printf("Cantidad total: %s\n Cantidad lista: %s\n Nombre plato: %s\n\n",separarPagina[0],separarPagina[1],separarPagina[2]);

	free(pagina);
	return separarPagina;
}*/


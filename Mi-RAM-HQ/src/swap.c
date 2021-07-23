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


int32_t obtener_frame_libre_swap(void) {

	for(int frame_disponible=0; frame_disponible < cantidad_paginas_swap; frame_disponible++) {
		if(frames_swap[frame_disponible]->estado == LIBRE) {
			log_info(logger, "El frame a asignar en Swap es el %d.\n", frame_disponible);
			return frame_disponible;
		}
	}
	log_info(logger, "No hay frames disponibles en Swap.\n");
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

	int32_t espacio_ocupado = TAMANIO_PAGINA - frames[frame_a_buscar]->espacio_libre;

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
	 guardar_pagina_en_swap(buffer, pagina_obtenida->numero_de_pagina, espacio_ocupado);


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

	int32_t espacio_ocupado = TAMANIO_PAGINA - frames[frame_a_buscar]->espacio_libre;

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

	int32_t inicio = frame_libre * TAMANIO_PAGINA;
	memcpy(area_swap + inicio, buffer, espacio_ocupado);
	area_swap += espacio_ocupado;
	memset(area_swap, '\n', TAMANIO_PAGINA - espacio_ocupado);

	frames_swap[frame_libre]->pagina = pagina_swap->numero_de_pagina;
	frames_swap[frame_libre]->estado = OCUPADO;
	frames_swap[frame_libre]->espacio_libre = TAMANIO_PAGINA - espacio_ocupado;

	list_add(paginas_swap, pagina_swap);

	if(msync(area_swap, espacio_ocupado, MS_SYNC) < 0) {

		if(logger->is_active_console == false){
			logger->is_active_console = true;
		}
		log_error(logger,"[msync]Error al volcar los cambios a Swap.\n");
		return -1;

	}else {

		if(logger->is_active_console == false){
			logger->is_active_console = true;
		}
		log_info(logger,"[msync]Se agregó la pagina en Swap exitosamente.\n");
	}
	return 0;
}



/*
int guardarPaginaEnSwapNueva(nombre* nombreRestaurante,uint32_t idPedido,nombre* nombrePlato,int cantidad){
	PedidoSwap* pedido = verificarPedidoSwap(idPedido,nombreRestaurante);
	ContenidoPaginaSwap* contenido = buscarPaginaEnPedidoSwap(pedido,nombrePlato->nombre);
	if(contenido){

		if(logger->is_active_console == false){
			logger->is_active_console = true;
		}
		log_error(logger,"La pagina %s ya se encuentra cargada en memoria",contenido->nombrePlato);
	}else{
		int marco = buscarMarcoLibre();
		Pagina* pagina = malloc(sizeof(Pagina));
		pagina->frame = marco;
		pagina->P = 0;
		pagina->U = 1;
		pagina->M = 0;
		pagina->nroPagina = contadorPagina;

		ContenidoPaginaSwap* contenidoPag = malloc(sizeof(ContenidoPaginaSwap));
		contenidoPag->cantTotal = cantidad;
		contenidoPag->cantLista = 0;
		contenidoPag->nroPagina = contadorPagina;
		char* nombre = string_duplicate(nombrePlato->nombre);
		strcpy(contenidoPag->nombrePlato,nombrePlato->nombre);

		char* cantTotal = string_itoa(cantidad);
		char* cantLista = string_itoa(0);
		char* paginaACopiar = string_new();

		string_append(&paginaACopiar,cantTotal);
		string_append(&paginaACopiar,",");
		string_append(&paginaACopiar,cantLista);
		string_append(&paginaACopiar,",");
		string_append(&paginaACopiar,nombre);

		pthread_mutex_lock(&swapPagina);
		void* stream = mapeo + (tamanioPagina*marco);
		memcpy(stream,paginaACopiar,strlen(paginaACopiar));
		stream += strlen(paginaACopiar);
		memset(stream, '\0', tamanioPagina - strlen(paginaACopiar));

		framesEnSwap[marco] = OCUPADO;
		list_add(paginasEnSwap,pagina);
		pthread_mutex_unlock(&swapPagina);

		pthread_mutex_lock(&agregarPedidoSwap);
		list_add(pedido->platos,contenidoPag);
		pthread_mutex_unlock(&agregarPedidoSwap);


		if(logger->is_active_console == false){
			logger->is_active_console = true;
		}
		uint32_t posicionRelativa = (stream - mapeo) - strlen(paginaACopiar);
		log_info(logger,"Se inserto en SWAP el plato: %s en el frame: %d, donde comienza en: %u",nombrePlato->nombre,marco,posicionRelativa);

		if(msync(mapeo,strlen(paginaACopiar), MS_SYNC) < 0) {

			if(logger->is_active_console == false){
				logger->is_active_console = true;
			}
			log_error(logger,"[msync]Error al volcar los cambios a SWAP");
			return -1;
		}else{

			if(logger->is_active_console == false){
				logger->is_active_console = true;
			}
			log_info(logger,"[msync]Se agregó la pagina en SWAP exitosamente");
		}

		free(cantTotal);
		free(cantLista);
		free(nombre);
		free(paginaACopiar);
	}

	pthread_mutex_lock(&modifPagina);
	contadorPagina++;
	pthread_mutex_unlock(&modifPagina);

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




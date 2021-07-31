#include "paginacion.h"


uint32_t get_timestamp(void) {
	struct timeval tv;
	gettimeofday(&tv,NULL);
	uint32_t  timestamp = (uint32_t)((tv.tv_sec)*1000 + (tv.tv_usec)/1000);
	return timestamp;
}


void liberar_frame(uint32_t num_frame) {
	frames[num_frame]->estado = LIBRE;
	frames[num_frame]->espacio_libre = TAMANIO_PAGINA;
	frames[num_frame]->puntero_frame = 0;
	frames[num_frame]->pagina = -1;
	frames[num_frame]->proceso = -1;
}


int32_t cantidad_paginas_usadas(int32_t tamanio) {

    int32_t cantidad_paginas = 0;

    while((cantidad_paginas * TAMANIO_PAGINA) < tamanio){
    	cantidad_paginas++;
    }

    return cantidad_paginas;
}


void inicializar_frames(void) {
	cantidad_frames = TAMANIO_MEMORIA / TAMANIO_PAGINA;
	log_info(logger, "Se inician %u frames en Memoria Principal.\n", cantidad_frames);
	frames = calloc(cantidad_frames, sizeof(frame));
	for(int i=0; i<cantidad_frames; i++) {
		frames[i] = malloc(sizeof(frame));
		liberar_frame(i);
	}
}


bool hay_frame_libre(void) {

	for(int i=0; i<cantidad_frames; i++) {
		if(frames[i]->estado == LIBRE) {
			return true;
		}
	}
	return false;
}


int32_t obtener_frame_libre(void) {
	for(int i=0; i<cantidad_frames; i++) {
		if(frames[i]->estado == LIBRE) {
			return i;
		}
	}
	return -1;
}


int32_t obtener_frame_disponible(void) {
	int32_t num_frame;

	if(hay_frame_libre()) {
		num_frame = obtener_frame_libre();

		return num_frame;
	}

	else {

		log_info(logger,"Todos los frames están ocupados, por lo que se aplicará el algoritmo de reemplazo dado por el archivo de Configuración.\n");

		return frame_disponible_segun_algoritmo();
	}
}


t_pagina* obtener_pagina_disponible(t_list* paginas) {

	bool esta_libre(void* pagina) {
		return ((t_pagina*)pagina)->estado == LIBRE;
	}
	list_sort(paginas, menor_a_mayor_segun_num);
	t_pagina* pagina_buscada = list_find(paginas, esta_libre);
	return pagina_buscada;
}


t_tabla_paginas_patota* crear_tabla_paginas(t_pcb* nueva_patota, int32_t tamanio_total, int32_t cantidad_tripulantes) {

	t_tabla_paginas_patota* tabla = malloc(sizeof(t_tabla_paginas_patota));
	tabla->patota = malloc(sizeof(t_pcb));
	tabla->patota->pid = nueva_patota->pid;
	tabla->patota->tareas = nueva_patota->tareas;
	tabla->paginas = list_create();
	tabla->direccion_tripulantes = list_create();
	tabla->direccion_tareas = list_create();
	tabla->cantidad_tripulantes = cantidad_tripulantes;

	int32_t cantidad_paginas = cantidad_paginas_usadas(tamanio_total);
	for(int i=0; i<cantidad_paginas; i++) {
		t_pagina* pagina = malloc(sizeof(t_pagina));
		pagina->numero_de_pagina = contador_pagina;
		pagina->P = 0;
		pagina->numero_de_frame = -1;
		pagina->U = 0;
		pagina->estado = LIBRE;

		list_add_in_index(tabla->paginas, i, pagina);

		contador_pagina++;
	}

	return tabla;
}




void guardar_estructura_en_memoria(void* estructura, tipo_estructura tipo, t_tabla_paginas_patota* tabla_patota, int32_t tamanio_estructura) {

	void* buffer;

	int32_t pid = tabla_patota->patota->pid;

	switch(tipo) {

		case PATOTA:
			sem_wait(mutex_serializacion);
			buffer = serializar_patota(estructura, tamanio_estructura);
			tabla_patota->direccion_patota = puntero_inicio;
			sem_post(mutex_serializacion);

			break;

		case TAREAS:
			sem_wait(mutex_serializacion);
			buffer = serializar_tareas(estructura, tamanio_estructura, tabla_patota);
			tabla_patota->patota->tareas = puntero_inicio;
			sem_post(mutex_serializacion);

			break;

		case TRIPULANTE:
			sem_wait(mutex_serializacion);
			buffer = serializar_tripulante(estructura, tamanio_estructura);
			t_dl_tripulante* direccion_tripulante = malloc(sizeof(t_dl_tripulante));
			direccion_tripulante->direccion_logica = puntero_inicio;
			direccion_tripulante->id_tripulante = ((t_tcb*)estructura)->id_tripulante;
			list_add(tabla_patota->direccion_tripulantes, direccion_tripulante);
			sem_post(mutex_serializacion);

			break;

		default:
			break;
	}

	memoria_libre_total -= tamanio_estructura;

	int32_t paginas_necesarias = cantidad_paginas_usadas(tamanio_estructura);

	// Mas que Sobrante es lo que se guarda en la última página, sin la necesidad de tener que ocuparla por completo
	int32_t sobrante = tamanio_estructura - (paginas_necesarias-1) * TAMANIO_PAGINA;
	int32_t offset = 0;
	int32_t pagina_a_usar = 0;
	while(paginas_necesarias > 0) {

		t_pagina* pagina_buscada = obtener_pagina_disponible(tabla_patota->paginas);
		pagina_buscada->P = 1;
		pagina_buscada->U = 1;
		asignar_frame_disponible(pagina_buscada, pid);

		int32_t num_frame = pagina_buscada->numero_de_frame;
		uint32_t offset_frame = TAMANIO_PAGINA - frames[num_frame]->espacio_libre;
		int32_t inicio_frame = num_frame * TAMANIO_PAGINA + offset_frame;

		if(paginas_necesarias == 1) {

			if(frames[num_frame]->espacio_libre >= sobrante) {

				memcpy(memoria_principal + inicio_frame, buffer + offset, sobrante);
				offset += sobrante;
				puntero_inicio += sobrante;

				frames[num_frame]->espacio_libre -= sobrante;
				frames[num_frame]->puntero_frame += sobrante;

				if(frames[num_frame]->espacio_libre == 0) {
					frames[num_frame]->estado = OCUPADO;
					pagina_buscada->estado = OCUPADO;
				}
			}
			else {

				memcpy(memoria_principal + inicio_frame, buffer + offset, frames[num_frame]->espacio_libre);
				offset += frames[num_frame]->espacio_libre;
				puntero_inicio += frames[num_frame]->espacio_libre;
				frames[num_frame]->estado = OCUPADO;
				pagina_buscada->estado = OCUPADO;

				int32_t resto = sobrante - frames[num_frame]->espacio_libre;
				frames[num_frame]->espacio_libre = 0;
				frames[num_frame]->puntero_frame = TAMANIO_PAGINA;

				t_pagina* pagina_buscada = obtener_pagina_disponible(tabla_patota->paginas);
				pagina_buscada->P = 1;
				pagina_buscada->U = 1;
				asignar_frame_disponible(pagina_buscada, pid);

				num_frame = pagina_buscada->numero_de_frame;

				offset_frame = TAMANIO_PAGINA - frames[num_frame]->espacio_libre;

				inicio_frame = num_frame * TAMANIO_PAGINA + offset_frame;

				memcpy(memoria_principal + inicio_frame, buffer + offset, resto);
				offset += resto;
				puntero_inicio += resto;

				frames[num_frame]->espacio_libre -= resto;
				frames[num_frame]->puntero_frame += resto;

				if(frames[num_frame]->espacio_libre == 0) {
					frames[num_frame]->estado = OCUPADO;
					pagina_buscada->estado = OCUPADO;
				}
			}
		}

		// En este caso, como necesito mas de 1 página, voy a guardar las demás páginas por completo
		else {

			if(frames[num_frame]->espacio_libre == TAMANIO_PAGINA) {

				memcpy(memoria_principal + inicio_frame, buffer + offset, TAMANIO_PAGINA);
				offset += TAMANIO_PAGINA;
				puntero_inicio += TAMANIO_PAGINA;
				frames[num_frame]->espacio_libre = 0;
				frames[num_frame]->puntero_frame = TAMANIO_PAGINA;
				frames[num_frame]->estado = OCUPADO;
				pagina_buscada->estado = OCUPADO;

			}

			// En el caso que el frame este ocupado por una parte, tengo que guardar una parte en ese frame, y el resto en el sig
				// tambien, si entro por aca, es porque voy a guardar tantos bytes como el TAMAÑO de una página
			else {

				memcpy(memoria_principal + inicio_frame, buffer + offset, frames[num_frame]->espacio_libre);
				offset += frames[num_frame]->espacio_libre;
				puntero_inicio += frames[num_frame]->espacio_libre;
				frames[num_frame]->estado = OCUPADO;
				pagina_buscada->estado = OCUPADO;

				int32_t resto = TAMANIO_PAGINA - frames[num_frame]->espacio_libre;
				frames[num_frame]->espacio_libre = 0;
				frames[num_frame]->puntero_frame = TAMANIO_PAGINA;

				t_pagina* pagina_buscada = obtener_pagina_disponible(tabla_patota->paginas);
				pagina_buscada->P = 1;
				pagina_buscada->U = 1;
				asignar_frame_disponible(pagina_buscada, pid);

				num_frame = pagina_buscada->numero_de_frame;
				offset_frame = TAMANIO_PAGINA - frames[num_frame]->espacio_libre;
				inicio_frame = num_frame * TAMANIO_PAGINA + offset_frame;

				memcpy(memoria_principal + inicio_frame, buffer + offset, resto);
				offset += resto;
				puntero_inicio += resto;

				frames[num_frame]->espacio_libre -= resto;
				frames[num_frame]->puntero_frame += resto;

				if(frames[num_frame]->espacio_libre == 0) {
					frames[num_frame]->estado = OCUPADO;
					pagina_buscada->estado = OCUPADO;
				}
			}
		}

		pagina_a_usar++;
		paginas_necesarias--;
	}

	free(buffer);
	sem_post(crear_pagina_sem);
}



//TODO
void asignar_frame_disponible(t_pagina* pagina, uint32_t pid) {

	pagina->numero_de_frame = obtener_frame_disponible();

	pagina->P = 1;
	pagina->U = 1;
	pagina->tiempo_referencia = get_timestamp();

	frames[pagina->numero_de_frame]->pagina = pagina->numero_de_pagina;

	frames[pagina->numero_de_frame]->proceso = pid;

	log_info(logger, "Se asignó la Página %u en el Frame %u.\n", pagina->numero_de_pagina, pagina->numero_de_frame);
}


t_tabla_paginas_patota* buscar_tabla_patota(uint32_t id_patota) {

	bool se_encuentra_patota(void* tabla){
		return ((t_tabla_paginas_patota*)tabla)->patota->pid == id_patota;
	}

	t_tabla_paginas_patota* tabla_buscada = list_find(tablas_paginas, se_encuentra_patota);

	return tabla_buscada;
}


int32_t buscar_direccion_tripu_por_id(t_tabla_paginas_patota* tabla_patota_buscada, uint32_t id_tripulante_a_buscar) {

	t_list* direccion_tripulantes = tabla_patota_buscada->direccion_tripulantes;

	bool mismo_id_tripulante(void* dl_tripulante) {
		return (((t_dl_tripulante*)dl_tripulante)->id_tripulante == id_tripulante_a_buscar);
	}

	t_dl_tripulante* direc_logica_tripulante = list_find(direccion_tripulantes, mismo_id_tripulante);

	return direc_logica_tripulante->direccion_logica;
}


int32_t buscar_direccion_tarea_por_id(t_tabla_paginas_patota* tabla_patota_buscada, uint32_t id_tarea_a_buscar) {

	t_list* direccion_tarea = tabla_patota_buscada->direccion_tareas;

	bool mismo_id_tarea(void* dl_tarea) {
		return (((t_dl_tarea*)dl_tarea)->id_tarea == id_tarea_a_buscar);
	}

	t_dl_tarea* direccion_logica_tarea = list_find(direccion_tarea, mismo_id_tarea);

	return direccion_logica_tarea->direccion_logica;
}


int32_t obtener_direc_fisica_con_direccion_logica(int32_t direccion_logica, t_tabla_paginas_patota* tabla_patota_buscada) {

	int32_t numero_pag = direccion_logica / TAMANIO_PAGINA;

	int32_t resto = direccion_logica % TAMANIO_PAGINA;

	t_pagina* pagina_buscada = buscar_pagina(numero_pag, tabla_patota_buscada->paginas);

	if(pagina_buscada->P == 1) {

		int32_t inicio = pagina_buscada->numero_de_frame * TAMANIO_PAGINA;

		return inicio + resto;
	}
	else {
		//TODO
		log_info(logger, "La Página %u no se encuentra cargada en Memoria Principal.\n", numero_pag);

		sem_wait(mutex_swap);
		asignar_frame_disponible(pagina_buscada, tabla_patota_buscada->patota->pid);

		int32_t espacio_ocupado;
		void* buffer_recuperado = recuperar_en_swap(pagina_buscada->numero_de_pagina, &espacio_ocupado);
		int32_t frame_disponible = pagina_buscada->numero_de_frame;

		mem_hexdump(buffer_recuperado, espacio_ocupado);

		printf("FRAME USADO: %u\n", frame_disponible);
		printf("ESPACIO OCUPADO DEL FRAME: %u\n", espacio_ocupado);

		printf("NUMERO DE PAGINA: %u\n", frames[frame_disponible]->pagina);
		printf("PROCESO: %u\n", frames[frame_disponible]->proceso);

		frames[frame_disponible]->espacio_libre = TAMANIO_PAGINA - espacio_ocupado;
		frames[frame_disponible]->estado = OCUPADO;
		frames[frame_disponible]->puntero_frame = espacio_ocupado;
		sem_post(mutex_swap);

		int32_t inicio_frame = frame_disponible * TAMANIO_PAGINA;

		sem_wait(mutex_swap);
		memcpy(memoria_principal + inicio_frame, buffer_recuperado, espacio_ocupado);
		memoria_libre_total -= espacio_ocupado;
		sem_post(mutex_swap);

		log_info(logger, "Se recuperó la Página %u desde Memoria Virtual y se asignó al Frame %u.\n", pagina_buscada->numero_de_pagina, frame_disponible);

		free(buffer_recuperado);
		return inicio_frame + resto;
	}
}


t_pagina* buscar_pagina(int32_t nro_pagina, t_list* paginas) {

	bool se_encuentra_pagina(void* pagina){
		return ((t_pagina*)pagina)->numero_de_pagina == nro_pagina;
	}

	t_pagina* pagina = list_find(paginas, se_encuentra_pagina);
	return pagina;
}




// Funciones de serialización en Paginación
void* serializar_patota(t_pcb* patota, uint32_t tamanio) {

	void* buffer = malloc(tamanio);

	int32_t desplazamiento = 0;

	memcpy(buffer + desplazamiento, &(patota->pid), sizeof(patota->pid));
	desplazamiento += sizeof(patota->pid);

	memcpy(buffer + desplazamiento, &(patota->tareas), sizeof(patota->tareas));
	desplazamiento += sizeof(patota->tareas);

	return buffer;
}


void* serializar_tareas(t_list* tareas_de_la_patota, uint32_t tamanio, t_tabla_paginas_patota* tabla_patota) {

	void* buffer = malloc(tamanio);
	int32_t desplazamiento = 0;
	int32_t cantidad_tareas = list_size(tareas_de_la_patota);

	for(int i=0; i<cantidad_tareas; i++) {
		tarea* tarea_a_guardar = list_get(tareas_de_la_patota, i);
		t_dl_tarea* direccion_tarea = malloc(sizeof(t_dl_tarea));

		direccion_tarea->id_tarea = i;
		direccion_tarea->direccion_logica = puntero_inicio + desplazamiento;

		memcpy(buffer + desplazamiento, tarea_a_guardar->tarea, tarea_a_guardar->tamanio_tarea);
		desplazamiento += tarea_a_guardar->tamanio_tarea;
		direccion_tarea->tamanio_tarea = tarea_a_guardar->tamanio_tarea;

		list_add_in_index(tabla_patota->direccion_tareas, i, direccion_tarea);
		free(tarea_a_guardar);
	}
	return buffer;
}


void* serializar_tripulante(t_tcb* tripulante, uint32_t tamanio) {

	void* buffer = malloc(tamanio);

	int32_t desplazamiento = 0;

	memcpy(buffer + desplazamiento, &(tripulante->id_tripulante), sizeof(tripulante->id_tripulante));
	desplazamiento += sizeof(tripulante->id_tripulante);

	memcpy(buffer + desplazamiento, &(tripulante->estado_tripulante), sizeof(tripulante->estado_tripulante));
	desplazamiento += sizeof(tripulante->estado_tripulante);

	memcpy(buffer + desplazamiento, &(tripulante->posicion_x), sizeof(tripulante->posicion_x));
	desplazamiento += sizeof(tripulante->posicion_x);

	memcpy(buffer + desplazamiento, &(tripulante->posicion_y), sizeof(tripulante->posicion_y));
	desplazamiento += sizeof(tripulante->posicion_y);

	memcpy(buffer + desplazamiento, &(tripulante->id_tarea_a_realizar), sizeof(tripulante->id_tarea_a_realizar));
	desplazamiento += sizeof(tripulante->id_tarea_a_realizar);

	memcpy(buffer + desplazamiento, &(tripulante->puntero_PCB), sizeof(tripulante->puntero_PCB));
	desplazamiento += sizeof(tripulante->puntero_PCB);

	return buffer;
}


void* obtener_tripulante_de_memoria(uint32_t direccion_fisica, uint32_t direccion_logica, t_tabla_paginas_patota* tabla_patota) {

	void* buffer = malloc(sizeof(t_tcb));

	void* inicio;

	int32_t lectura_tripulante = sizeof(t_tcb);
	int32_t direccion_logica_final = direccion_logica + sizeof(t_tcb);
	int32_t offset_frame = direccion_fisica % TAMANIO_PAGINA;
	int32_t faltante;
	int32_t direccion_logica_tripulante = direccion_logica;
	int32_t offset_buffer = 0;
	int32_t direccion_fisica_nueva = direccion_fisica;

	sem_wait(mutex_tripulante_swap);
	while(direccion_logica_tripulante != direccion_logica_final) {

		faltante = TAMANIO_PAGINA - offset_frame;
		inicio = (void*)memoria_principal + direccion_fisica_nueva;

		if(lectura_tripulante >= faltante){
			lectura_tripulante -= faltante;
			memcpy(buffer + offset_buffer, inicio, faltante);
			direccion_logica_tripulante += faltante;
			offset_buffer += faltante;
			direccion_fisica_nueva = obtener_direc_fisica_con_direccion_logica(direccion_logica_tripulante, tabla_patota);
			offset_frame = direccion_fisica_nueva % TAMANIO_PAGINA;
		}
		else{
			memcpy(buffer + offset_buffer, inicio, lectura_tripulante);
			direccion_logica_tripulante += lectura_tripulante;
			offset_buffer += lectura_tripulante;
		}
	}
	sem_post(mutex_tripulante_swap);
	return buffer;
}


void* obtener_tareas_de_memoria(uint32_t direccion_fisica, uint32_t id_tarea_buscada, t_tabla_paginas_patota* tabla_patota) {

	t_list* direccion_tarea = tabla_patota->direccion_tareas;

	bool mismo_id_tarea(void* dl_tarea) {
		return (((t_dl_tarea*)dl_tarea)->id_tarea == id_tarea_buscada);
	}

	t_dl_tarea* dl_tarea = list_find(direccion_tarea, mismo_id_tarea);

	void* buffer = malloc(dl_tarea->tamanio_tarea);

	void* inicio;

	int32_t lectura_tarea = dl_tarea->tamanio_tarea;
	int32_t direccion_logica_tarea = dl_tarea->direccion_logica;
	int32_t direccion_logica_final = direccion_logica_tarea + dl_tarea->tamanio_tarea;
	int32_t offset_frame = direccion_fisica % TAMANIO_PAGINA;
	int32_t faltante;
	int32_t offset_buffer = 0;
	int32_t direccion_fisica_nueva = direccion_fisica;

	printf("DIRECCION FISICA ACTUAL: %u\n", direccion_fisica);
	printf("DIRECCION LOGICA INICIO: %u   DIRECCION LOGICA FIN: %u\n", direccion_logica_tarea, direccion_logica_final);
	printf("OFFSET FRAME: %u\n\n", offset_frame);


	sem_wait(mutex_tripulante_swap);
	while(direccion_logica_tarea != direccion_logica_final) {

		faltante = TAMANIO_PAGINA - offset_frame;
		inicio = (void*)memoria_principal + direccion_fisica_nueva;

		printf("\nFALTANTE: %u\n", faltante);
		printf("LECTURA TAREA: %u\n\n", lectura_tarea);

		if(lectura_tarea >= faltante){
			lectura_tarea -= faltante;
			memcpy(buffer + offset_buffer, inicio, faltante);
			direccion_logica_tarea += faltante;
			offset_buffer += faltante;
			direccion_fisica_nueva = obtener_direc_fisica_con_direccion_logica(direccion_logica_tarea, tabla_patota);
			offset_frame = direccion_fisica_nueva % TAMANIO_PAGINA;
		}
		else{
			memcpy(buffer + offset_buffer, inicio, lectura_tarea);
			direccion_logica_tarea += lectura_tarea;
			offset_buffer += lectura_tarea;
			printf("DIRECCION LOGICA TAREA LEIDA: %u\n", direccion_logica_tarea);
			printf("OFFSET BUFFER: %u\n", offset_buffer);
		}
	}

	sem_post(mutex_tripulante_swap);

	return buffer;
}


t_tcb* encontrar_tripulante_memoria(uint32_t direccion_fisica, uint32_t direccion_logica, t_tabla_paginas_patota* tabla_patota) {

	t_tcb* tripulante = malloc(sizeof(t_tcb));

	printf("                             ENCONTRAR TRIPULANTE\n");

	void* buffer = obtener_tripulante_de_memoria(direccion_fisica, direccion_logica, tabla_patota);
	uint32_t desplazamiento = 0;

	memcpy(&(tripulante->id_tripulante), buffer + desplazamiento, sizeof(tripulante->id_tripulante));
	desplazamiento += sizeof(tripulante->id_tripulante);

	printf("ID TRIPULANTE: %u\n", tripulante->id_tripulante);

	memcpy(&(tripulante->estado_tripulante), buffer + desplazamiento, sizeof(tripulante->estado_tripulante));
	desplazamiento += sizeof(tripulante->estado_tripulante);

	printf("ESTADO TRIPULANTE: %c\n", tripulante->estado_tripulante);

	memcpy(&(tripulante->posicion_x), buffer + desplazamiento, sizeof(tripulante->posicion_x));
	desplazamiento += sizeof(tripulante->posicion_x);

	printf("POSICION X: %u\n", tripulante->posicion_x);

	memcpy(&(tripulante->posicion_y), buffer + desplazamiento, sizeof(tripulante->posicion_y));
	desplazamiento += sizeof(tripulante->posicion_y);

	printf("POSICION Y: %u\n", tripulante->posicion_y);

	memcpy(&(tripulante->id_tarea_a_realizar), buffer + desplazamiento, sizeof(tripulante->id_tarea_a_realizar));
	desplazamiento += sizeof(tripulante->id_tarea_a_realizar);

	printf("ID TAREA A REALIZAR: %u\n", tripulante->id_tarea_a_realizar);

	memcpy(&(tripulante->puntero_PCB), buffer + desplazamiento, sizeof(tripulante->puntero_PCB));
	desplazamiento += sizeof(tripulante->puntero_PCB);

	printf("PUNTERO PCB: %u\n", tripulante->puntero_PCB);

	free(buffer);
	return tripulante;
}


tarea* encontrar_tarea_en_memoria(uint32_t direccion_fisica, uint32_t id_tarea_buscada, t_tabla_paginas_patota* tabla_patota_buscada) {

	tarea* tarea_recuperada = malloc(sizeof(tarea));

	t_list* direccion_tarea = tabla_patota_buscada->direccion_tareas;

	bool mismo_id_tarea(void* dl_tarea) {
		return (((t_dl_tarea*)dl_tarea)->id_tarea == id_tarea_buscada);
	}

	t_dl_tarea* direccion_logica_tarea = list_find(direccion_tarea, mismo_id_tarea);

	void* buffer = obtener_tareas_de_memoria(direccion_fisica, id_tarea_buscada, tabla_patota_buscada);

	tarea_recuperada->tamanio_tarea = direccion_logica_tarea->tamanio_tarea;
	tarea_recuperada->tarea = malloc(tarea_recuperada->tamanio_tarea);

	memcpy(tarea_recuperada->tarea, buffer, tarea_recuperada->tamanio_tarea);

	mem_hexdump(buffer, tarea_recuperada->tamanio_tarea);

	printf("TAREA RECUPERADA: %s\n", tarea_recuperada->tarea);

	free(buffer);
	return tarea_recuperada;
}


void actualizar_tripulante_memoria(t_tcb* tripulante, uint32_t direccion_fisica, t_tabla_paginas_patota* tabla_patota) {

	void* inicio;
	void* buffer = serializar_tripulante(tripulante, sizeof(t_tcb));

	int32_t lectura_tripulante = sizeof(t_tcb);
	int32_t direccion_logica_tripulante = buscar_direccion_tripu_por_id(tabla_patota, tripulante->id_tripulante);
	int32_t direccion_logica_final = direccion_logica_tripulante + sizeof(t_tcb);
	int32_t offset_frame = direccion_fisica % TAMANIO_PAGINA;
	int32_t faltante;
	int32_t offset_buffer = 0;
	int32_t direccion_fisica_nueva = direccion_fisica;

	sem_wait(mutex_tripulante_swap);
	while(direccion_logica_tripulante != direccion_logica_final) {

		faltante = TAMANIO_PAGINA - offset_frame;
		inicio = (void*)memoria_principal + direccion_fisica_nueva;

		if(lectura_tripulante >= faltante){
			lectura_tripulante -= faltante;
			memcpy(inicio, buffer + offset_buffer, faltante);
			direccion_logica_tripulante += faltante;
			offset_buffer += faltante;
			direccion_fisica_nueva = obtener_direc_fisica_con_direccion_logica(direccion_logica_tripulante, tabla_patota);
			offset_frame = direccion_fisica_nueva % TAMANIO_PAGINA;
		}
		else{
			memcpy(inicio, buffer + offset_buffer, lectura_tripulante);
			direccion_logica_tripulante += lectura_tripulante;
			offset_buffer += lectura_tripulante;
		}
	}
	sem_post(mutex_tripulante_swap);
}


void actualizar_referencia(t_list* paginas, uint32_t direccion_logica) {

	int32_t numero = direccion_logica / TAMANIO_PAGINA;
	int32_t entero = (int32_t) numero;

	bool misma_pagina(void* pagina) {
		return ((t_pagina*)pagina)->numero_de_pagina == entero;
	}

	t_pagina* pagina_a_actualizar = list_find(paginas, misma_pagina);
	pagina_a_actualizar->tiempo_referencia = get_timestamp();
	pagina_a_actualizar->U = 1;
}


t_tarea* buscar_proxima_tarea_del_tripulante_paginacion(uint32_t direccion_fisica, uint32_t id_tarea_buscada, t_tabla_paginas_patota* tabla_patota_buscada) {

	tarea* tarea_encontrada = encontrar_tarea_en_memoria(direccion_fisica, id_tarea_buscada, tabla_patota_buscada);

	printf("TAREA ENCONTRADA CON ID: %u Y ES: %s\n", id_tarea_buscada, tarea_encontrada->tarea);

	t_tarea* tarea_a_retornar = obtener_la_tarea(tarea_encontrada->tarea);

	return tarea_a_retornar;
}



// FUNCIONES PARA ORDENAR O USAR EN LISTAS
bool menor_a_mayor_por_frame(void* pagina, void* pagina_siguiente) {
	return ((t_pagina*)pagina)->numero_de_frame < ((t_pagina*)pagina_siguiente)->numero_de_frame;
}

bool menor_a_mayor_segun_num(void* pagina, void* pagina_siguiente) {
	return ((t_pagina*)pagina)->numero_de_pagina < ((t_pagina*)pagina_siguiente)->numero_de_pagina;
}

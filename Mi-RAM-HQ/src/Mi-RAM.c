#include "Mi-RAM.h"

int main(void) {

	config = crear_config(CONFIG_PATH);
	obtener_datos_de_config(config);
	logger = crear_log("mi-ram-hq.log", "Mi-RAM HQ");

	// Recibe la señal para hacer el Dump de Memoria
	signal(SIGUSR1, (void*)iniciar_dump_memoria);

	// Recibe la señal para compactar la memoria
	signal(SIGUSR2, (void*)verificar_compactacion);

	// Muestra en pantalla la cantidad de Memoria Libre
	signal(SIGALRM, (void*)chequear_memoria);

	iniciar_variables_y_semaforos();
	inicializar_memoria();
	elegir_esquema_de_memoria(ESQUEMA_MEMORIA);
	criterio_elegido = elegir_criterio_seleccion(CRITERIO_SELECCION);
	algoritmo_elegido = elegir_algoritmo_reemplazo(ALGORITMO_REEMPLAZO);

	iniciar_mapa();
	iniciar_archivo_dump();
	iniciar_comunicacion();

	return EXIT_SUCCESS;
}

void iniciar_variables_y_semaforos(void) {

	memoria_principal = NULL;
	area_swap = NULL;

	contador_id_tripu = 1;
	contador_id_patota = 1;
	contador_segmento = 0;
	contador_pagina = 0;

	ids = list_create();

	mutex_segmentos = malloc(sizeof(sem_t));
	sem_init(mutex_segmentos, 0, 1);

	crear_segmento_sem = malloc(sizeof(sem_t));
	sem_init(crear_segmento_sem, 0, 0);

	crear_pagina_sem = malloc(sizeof(sem_t));
	sem_init(crear_pagina_sem, 0, 0);
}


void inicializar_memoria(void) {
	log_info(logger, "Se utilizará %s como esquema de memoria.\n", ESQUEMA_MEMORIA);

	memoria_principal = malloc(TAMANIO_MEMORIA);
	memoria_libre_total = TAMANIO_MEMORIA;
	memoria_virtual_total = TAMANIO_SWAP;

	if(memoria_principal != NULL){
		log_info(logger, "Se inició la Memoria Principal con un tamaño de %u bytes.\n", TAMANIO_MEMORIA);
	}
	else {
		log_error(logger, "Error al iniciar la Memoria Principal.\n");
		sleep(1);
		abort();
	}
}


void iniciar_mapa(void) {

	//nivel_gui_inicializar();

	//nivel_gui_get_area_nivel(&columnas, &filas);

	//amongOs = nivel_crear("A-MongOs");
}


void obtener_datos_de_config(t_config* config) {

	PUERTO = config_get_string_value(config, "PUERTO");
	TAMANIO_MEMORIA = config_get_int_value(config, "TAMANIO_MEMORIA");
	ESQUEMA_MEMORIA = config_get_string_value(config, "ESQUEMA_MEMORIA");
	TAMANIO_PAGINA = config_get_int_value(config, "TAMANIO_PAGINA");
	TAMANIO_SWAP = config_get_int_value(config, "TAMANIO_SWAP");
	PATH_SWAP = config_get_string_value(config, "PATH_SWAP");
	ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	CRITERIO_SELECCION = config_get_string_value(config, "CRITERIO_SELECCION");
}


void iniciar_comunicacion() {

	int32_t conexion_servidor = iniciar_servidor(IP, PUERTO);

	while(1) {

		int32_t conexion_cliente = esperar_conexion(conexion_servidor);

		pthread_create(&hilo_recibir_mensajes, NULL, (void*)escuchar_conexion, (int32_t*)conexion_cliente);

		pthread_detach(hilo_recibir_mensajes);

	}
}



void procesar_mensajes(codigo_operacion operacion, int32_t conexion) {

	// INICIAR_PATOTA
	t_iniciar_patota* patota_recibida;
	t_respuesta_iniciar_patota* respuesta_iniciar_patota;
	int32_t tamanio_total;
	tareas_patota* tareas_de_la_patota;

	// ACTUALIZAR_UBICACION_TRIPULANTE
	t_tripulante_ubicacion* tripulante_por_ubicacion;
	t_respuesta_tripulante* respuesta_ok_ubicacion;

	// PEDIR_UBICACION_TRIPULANTE
	t_tripulante* tripulante_para_ubicacion;
	t_respuesta_tripulante_ubicacion* respuesta_con_ubicacion;

	// ACTUALIZAR_ESTADO_TRIPULANTE
	t_tripulante_estado* tripulante_por_estado;
	t_respuesta_tripulante* respuesta_por_estado;

	// PEDIDO_TAREA
	t_tripulante* tripulante_para_tarea;
	t_respuesta_tarea_tripulante* respuesta_con_tarea_tripulante;

	// EXPULSAR_TRIPULANTE
	t_tripulante* tripulante_a_eliminar;
	t_respuesta_tripulante* respuesta_tripulante_eliminado;


	switch(operacion)
{
			case INICIAR_PATOTA:
				patota_recibida = malloc(sizeof(t_iniciar_patota));
				respuesta_iniciar_patota = malloc(sizeof(t_respuesta_iniciar_patota));
				recibir_mensaje(patota_recibida, operacion, conexion);

				tareas_de_la_patota = malloc(sizeof(tareas_patota));

				parser_posiciones = string_split(patota_recibida->posiciones, "|");

				// Junta los IDs de los tripulantes de la Patota
				char* ids_enviar = string_new();

				string_trim(&patota_recibida->tareas_de_patota);
				strcat(patota_recibida->tareas_de_patota, "\0");

				tareas_de_la_patota->tamanio_tareas = patota_recibida->tamanio_tareas+1;
				tareas_de_la_patota->tareas = malloc(tareas_de_la_patota->tamanio_tareas);
				strcpy(tareas_de_la_patota->tareas, patota_recibida->tareas_de_patota);

				tamanio_tripulante = sizeof(uint32_t) + sizeof(char) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
				tamanio_tripulantes = tamanio_tripulante * patota_recibida->cantidad_tripulantes;
				tamanio_patota = sizeof(t_pcb);
				tamanio_tareas = tareas_de_la_patota->tamanio_tareas;
				tamanio_total = tamanio_patota + tamanio_tareas + tamanio_tripulantes;

//													   SEGMENTACION
				if(esquema_elegido == 'S') {
					t_tabla_segmentos_patota* tabla_patota;
					t_pcb* nueva_patota;

					// Verifica si hay espacio para guardar en memoria
					if(validar_espacio_por_patota_segmentacion(tamanio_total) == 0) {
						respuesta_iniciar_patota->numero_de_patota = 0;
						respuesta_iniciar_patota->respuesta = 0;
						respuesta_iniciar_patota->tamanio_ids = 0;
						respuesta_iniciar_patota->ids_tripu = malloc(respuesta_iniciar_patota->tamanio_ids+1);
						strcpy(respuesta_iniciar_patota->ids_tripu, "");

						log_error(logger, "No hay espacio suficiente para guardar la patota y su/s tripulante/s. \n");
					}
					else { // Hay suficiente espacio en memoria, puedo guardarlo y envio una respuesta a Discordiador

						nueva_patota = crear_pcb();

						tabla_patota = crear_tabla_segmentos(nueva_patota);

						sem_wait(mutex_segmentos);
						t_segmento* segmento_patota = administrar_guardar_segmento(nueva_patota, PATOTA, tamanio_patota);
						list_add(tabla_patota->segmentos, segmento_patota);
						sem_post(mutex_segmentos);

						uint32_t direccion_pcb = segmento_patota->inicio;

						log_info(logger, "Se inició la Patota %u con %u tripulante/s.\n", nueva_patota->pid, patota_recibida->cantidad_tripulantes);

						sem_wait(crear_segmento_sem);

						sem_wait(mutex_segmentos);
						t_segmento* segmento_tareas = administrar_guardar_segmento(tareas_de_la_patota, TAREAS, tamanio_tareas);
						list_add(tabla_patota->segmentos, segmento_tareas);
						sem_post(mutex_segmentos);

						tabla_patota->patota->tareas = segmento_tareas->inicio;
						tabla_patota->tamanio_tareas = tamanio_tareas;

						log_info(logger, "Se guardaron las tareas de la Patota %u, las cuales son: \n%s\n", nueva_patota->pid, patota_recibida->tareas_de_patota);

						sem_wait(crear_segmento_sem);

						int posicion = 0;
						for(int i=0;i<patota_recibida->cantidad_tripulantes;i++){

							t_tcb* nuevo_tripulante = crear_tcb(direccion_pcb, atoi(parser_posiciones[posicion]), atoi(parser_posiciones[posicion+1]));

							sem_wait(mutex_segmentos);
							t_segmento* segmento_tripulante = administrar_guardar_segmento(nuevo_tripulante, TRIPULANTE, tamanio_tripulante);
							list_add(tabla_patota->segmentos, segmento_tripulante);
							sem_post(mutex_segmentos);

							log_info(logger, "Se inició el Tripulante %u en el estado %c, con una posición en X: %u y una posición en Y: %u.\n", nuevo_tripulante->id_tripulante, nuevo_tripulante->estado_tripulante, nuevo_tripulante->posicion_x, nuevo_tripulante->posicion_y);

							//personaje_crear(amongOs, nuevo_tripulante->id_tripulante, nuevo_tripulante->posicion_x, nuevo_tripulante->posicion_y);

							sem_wait(crear_segmento_sem);

							string_append_with_format(&ids_enviar, "%u|", contador_id_tripu);
							contador_id_tripu++;
							free(nuevo_tripulante);
							posicion += 2;
						}

						list_add(tablas_segmentos, tabla_patota);

						strcat(ids_enviar,"\0");

						respuesta_iniciar_patota->numero_de_patota = nueva_patota->pid;
						respuesta_iniciar_patota->respuesta = 1;
						respuesta_iniciar_patota->tamanio_ids = strlen(ids_enviar);
						respuesta_iniciar_patota->ids_tripu = malloc(respuesta_iniciar_patota->tamanio_ids+1);
						strcpy(respuesta_iniciar_patota->ids_tripu,ids_enviar);

						contador_id_patota++;

						log_info(logger, "La patota iniciada tiene un peso de %d bytes.\n", tamanio_total);
						log_info(logger, "Se ha guardado en memoria la Patota %u y su/s %u Tripulante/s.\n", nueva_patota->pid, patota_recibida->cantidad_tripulantes);
					}


					enviar_mensaje(respuesta_iniciar_patota, RESPUESTA_INICIAR_PATOTA, conexion);
					//free(nueva_patota);
				}

//													   PAGINACION
				else if(esquema_elegido  == 'P') {
					t_pcb* nueva_patota;
					t_tabla_paginas_patota* tabla_patota;

					// Verifica si hay espacio para guardar en memoria
					if(validar_espacio_por_patota_paginacion(tamanio_total) == 0) {
						respuesta_iniciar_patota->numero_de_patota = 0;
						respuesta_iniciar_patota->respuesta = 0;
						respuesta_iniciar_patota->tamanio_ids = 0;
						respuesta_iniciar_patota->ids_tripu = malloc(respuesta_iniciar_patota->tamanio_ids+1);
						strcpy(respuesta_iniciar_patota->ids_tripu, "");

						log_error(logger, "No hay espacio suficiente para guardar la patota y su/s tripulante/s. \n");
					}
					else {

						nueva_patota = crear_pcb();

						tabla_patota = crear_tabla_paginas(nueva_patota, tamanio_total);

						guardar_estructura_en_memoria(nueva_patota, PATOTA, tabla_patota, tamanio_patota);
						uint32_t direccion_pcb = tabla_patota->direccion_patota;

						log_info(logger, "Se inició la Patota %u con %u tripulante/s.\n", nueva_patota->pid, patota_recibida->cantidad_tripulantes);

						sem_wait(crear_pagina_sem);

						guardar_estructura_en_memoria(tareas_de_la_patota, TAREAS, tabla_patota, tareas_de_la_patota->tamanio_tareas);
						tabla_patota->tamanio_tareas = tareas_de_la_patota->tamanio_tareas;

						log_info(logger, "Se guardaron las tareas de la Patota %u, las cuales son: \n%s\n", nueva_patota->pid, patota_recibida->tareas_de_patota);

						sem_wait(crear_pagina_sem);


						int posicion = 0;
						for(int i=0;i<patota_recibida->cantidad_tripulantes;i++){

							t_tcb* nuevo_tripulante = crear_tcb(direccion_pcb, atoi(parser_posiciones[posicion]), atoi(parser_posiciones[posicion+1]));

							guardar_estructura_en_memoria(nuevo_tripulante, TRIPULANTE, tabla_patota, tamanio_tripulante);

							log_info(logger, "Se inició el Tripulante %u en el estado %c, con una posición en X: %u y una posición en Y: %u.\n", nuevo_tripulante->id_tripulante, nuevo_tripulante->estado_tripulante, nuevo_tripulante->posicion_x, nuevo_tripulante->posicion_y);

							//personaje_crear(amongOs, nuevo_tripulante->id_tripulante, nuevo_tripulante->posicion_x, nuevo_tripulante->posicion_y);

							sem_wait(crear_pagina_sem);

							string_append_with_format(&ids_enviar, "%u|", contador_id_tripu);
							contador_id_tripu++;
							free(nuevo_tripulante);
							posicion += 2;
						}

						printf("Dirección lógica del PCB: %u\n", tabla_patota->direccion_patota);
						printf("Dirección lógica de las Tareas: %u\n", tabla_patota->patota->tareas);

						for(int c=0; c<patota_recibida->cantidad_tripulantes; c++) {
							t_dl_tripulante* direccion_tripu = list_get(tabla_patota->direccion_tripulantes, c);
							printf("Dirección lógica del Tripulante %d: %u\n", direccion_tripu->id_tripulante, direccion_tripu->direccion_logica);
						}

						printf("Cantidad de páginas usadas: %u\n", list_size(tabla_patota->paginas));

						t_pagina* ultima_pagina = list_get(tabla_patota->paginas, list_size(tabla_patota->paginas)-1);
						printf("Espacio del ultimo Frame usado por esta patota: %u\n", frames[ultima_pagina->numero_de_frame]->espacio_libre);

						frames[ultima_pagina->numero_de_frame]->estado = OCUPADO;
						ultima_pagina->estado = OCUPADO;
						printf("\nULTIMA PAGINA: %u\n", ultima_pagina->numero_de_pagina);
						printf("ULTIMO FRAME USADO: %u \n", ultima_pagina->numero_de_frame);

						puntero_inicio += frames[ultima_pagina->numero_de_frame]->espacio_libre;

						printf("\n\n                     PUNTERO INICIO: %u\n\n", puntero_inicio);


						list_add(tablas_paginas, tabla_patota);

						strcat(ids_enviar,"\0");

						respuesta_iniciar_patota->numero_de_patota = nueva_patota->pid;
						respuesta_iniciar_patota->respuesta = 1;
						respuesta_iniciar_patota->tamanio_ids = strlen(ids_enviar);
						respuesta_iniciar_patota->ids_tripu = malloc(respuesta_iniciar_patota->tamanio_ids+1);
						strcpy(respuesta_iniciar_patota->ids_tripu,ids_enviar);

						contador_id_patota++;

						log_info(logger, "La patota iniciada tiene un peso de %d bytes.\n", tamanio_total);
						log_info(logger, "Se ha guardado en memoria la Patota %u y su/s %u Tripulante/s.\n", nueva_patota->pid, patota_recibida->cantidad_tripulantes);

					}

					enviar_mensaje(respuesta_iniciar_patota, RESPUESTA_INICIAR_PATOTA, conexion);
					//free(nueva_patota);
				}

				cerrar_conexion(logger, conexion);

				free(ids_enviar);
				free(parser_posiciones);

				free(respuesta_iniciar_patota->ids_tripu);
				free(respuesta_iniciar_patota);
				free(patota_recibida->tareas_de_patota);
				free(patota_recibida->posiciones);
				free(patota_recibida);
				break;

			case ACTUALIZAR_UBICACION_TRIPULANTE:
				tripulante_por_ubicacion = malloc(sizeof(t_tripulante_ubicacion));
				respuesta_ok_ubicacion = malloc(sizeof(t_respuesta_tripulante));

				recibir_mensaje(tripulante_por_ubicacion, operacion, conexion);

				t_tcb* tripulante_buscado_por_ubicacion;

				if(esquema_elegido == 'S') {

					int indice = 0;

					sem_wait(mutex_segmentos);
					t_tabla_segmentos_patota* patota_buscada = buscar_tabla_de_patota(tripulante_por_ubicacion->id_patota);
					t_segmento* segmento_buscado = buscar_por_id(patota_buscada->segmentos, TRIPULANTE, tripulante_por_ubicacion->id_tripulante);
					sem_post(mutex_segmentos);

					indice = obtener_indice(patota_buscada->segmentos, segmento_buscado);

					sem_wait(mutex_segmentos);
					tripulante_buscado_por_ubicacion = obtener_contenido_de_segmento(segmento_buscado);
					sem_post(mutex_segmentos);

					tripulante_buscado_por_ubicacion->posicion_x = tripulante_por_ubicacion->posicion_x;
					tripulante_buscado_por_ubicacion->posicion_y = tripulante_por_ubicacion->posicion_y;

					//TODO: actualizar la posicion del tripulante en el MAPA
					sem_wait(mutex_segmentos);
					actualizar_segmento(tripulante_buscado_por_ubicacion, TRIPULANTE, segmento_buscado);
					list_replace(patota_buscada->segmentos, indice, segmento_buscado);
					sem_post(mutex_segmentos);

				}
				else if(esquema_elegido  == 'P') {
					//int indice = 0;

					t_tabla_paginas_patota* tabla_patota_buscada = buscar_tabla_patota(tripulante_por_ubicacion->id_patota);

					int32_t direccion_logica = buscar_pagina_por_id(tabla_patota_buscada, tripulante_por_ubicacion->id_tripulante);

					int32_t direccion_fisica = obtener_direc_fisica_con_direccion_logica(direccion_logica, tabla_patota_buscada);

					printf("Dirección lógica: %u\n", direccion_logica);
					printf("Dirección física: %u\n", direccion_fisica);

					tripulante_buscado_por_ubicacion = encontrar_tripulante_memoria(direccion_fisica);

					tripulante_buscado_por_ubicacion->posicion_x = tripulante_por_ubicacion->posicion_x;
					tripulante_buscado_por_ubicacion->posicion_y = tripulante_por_ubicacion->posicion_y;

					//TODO: actualizar la posicion del tripulante en el MAPA

					actualizar_tripulante_memoria(tripulante_buscado_por_ubicacion, direccion_fisica);
					actualizar_referencia(tabla_patota_buscada->paginas, direccion_logica);

				}

				respuesta_ok_ubicacion->id_tripulante = tripulante_por_ubicacion->id_tripulante;
				respuesta_ok_ubicacion->respuesta = 1;

				log_info(logger, "Se actualizaron las posiciones del Tripulante %u de la Patota %u, siendo estas la posición X: %u y la posición Y: %u.\n", tripulante_por_ubicacion->id_tripulante, tripulante_por_ubicacion->id_patota, tripulante_por_ubicacion->posicion_x, tripulante_por_ubicacion->posicion_y);

				enviar_mensaje(respuesta_ok_ubicacion, RESPUESTA_OK_UBICACION, conexion);

				cerrar_conexion(logger, conexion);

				free(tripulante_buscado_por_ubicacion);
				free(tripulante_por_ubicacion);
				free(respuesta_ok_ubicacion);
				break;

			case PEDIR_UBICACION_TRIPULANTE:
				tripulante_para_ubicacion = malloc(sizeof(t_tripulante));
				respuesta_con_ubicacion = malloc(sizeof(t_respuesta_tripulante_ubicacion));

				recibir_mensaje(tripulante_para_ubicacion, operacion, conexion);

				t_tcb* tripulante_con_ubicacion;

				if(esquema_elegido == 'S') {

					sem_wait(mutex_segmentos);
					t_tabla_segmentos_patota* patota_buscada = buscar_tabla_de_patota(tripulante_para_ubicacion->id_patota);
					t_segmento* segmento_buscado = buscar_por_id(patota_buscada->segmentos, TRIPULANTE, tripulante_para_ubicacion->id_tripulante);
					tripulante_con_ubicacion = obtener_contenido_de_segmento(segmento_buscado);
					sem_post(mutex_segmentos);

					respuesta_con_ubicacion->posicion_x = tripulante_con_ubicacion->posicion_x;
					respuesta_con_ubicacion->posicion_y = tripulante_con_ubicacion->posicion_y;

				}
				else if(esquema_elegido  == 'P') {

					t_tabla_paginas_patota* tabla_patota_buscada = buscar_tabla_patota(tripulante_para_ubicacion->id_patota);

					int32_t direccion_logica = buscar_pagina_por_id(tabla_patota_buscada, tripulante_para_ubicacion->id_tripulante);

					int32_t direccion_fisica = obtener_direc_fisica_con_direccion_logica(direccion_logica, tabla_patota_buscada);

					printf("Dirección lógica: %u\n", direccion_logica);
					printf("Dirección física: %u\n", direccion_fisica);

					tripulante_con_ubicacion = encontrar_tripulante_memoria(direccion_fisica);

					respuesta_con_ubicacion->posicion_x = tripulante_con_ubicacion->posicion_x;
					respuesta_con_ubicacion->posicion_y = tripulante_con_ubicacion->posicion_y;
					actualizar_referencia(tabla_patota_buscada->paginas, direccion_logica);
				}

				respuesta_con_ubicacion->id_tripulante = tripulante_para_ubicacion->id_tripulante;
				respuesta_con_ubicacion->respuesta = 1;

				log_info(logger, "Se enviaron las posiciones X: %u e Y: %u del Tripulante %u de la Patota %u.\n", respuesta_con_ubicacion->posicion_x, respuesta_con_ubicacion->posicion_y, respuesta_con_ubicacion->id_tripulante, tripulante_para_ubicacion->id_patota);

				enviar_mensaje(respuesta_con_ubicacion, RESPUESTA_NUEVA_UBICACION, conexion);

				cerrar_conexion(logger, conexion);

				free(tripulante_con_ubicacion);
				free(tripulante_para_ubicacion);
				free(respuesta_con_ubicacion);
				break;

			case ACTUALIZAR_ESTADO_TRIPULANTE:
				tripulante_por_estado = malloc(sizeof(t_tripulante_estado));
				respuesta_por_estado = malloc(sizeof(t_respuesta_tripulante));

				recibir_mensaje(tripulante_por_estado, operacion, conexion);

				char estado_anterior;
				t_tcb* tripulante_buscado_por_estado;

				if(esquema_elegido == 'S') {

					int indice = 0;

					sem_wait(mutex_segmentos);
					t_tabla_segmentos_patota* patota_buscada = buscar_tabla_de_patota(tripulante_por_estado->id_patota);
					t_segmento* segmento_buscado = buscar_por_id(patota_buscada->segmentos, TRIPULANTE, tripulante_por_estado->id_tripulante);
					sem_post(mutex_segmentos);

					indice = obtener_indice(patota_buscada->segmentos, segmento_buscado);

					sem_wait(mutex_segmentos);
					tripulante_buscado_por_estado = obtener_contenido_de_segmento(segmento_buscado);
					sem_post(mutex_segmentos);

					estado_anterior = tripulante_buscado_por_estado->estado_tripulante;

					tripulante_buscado_por_estado->estado_tripulante = tripulante_por_estado->estado;

					sem_wait(mutex_segmentos);
					actualizar_segmento(tripulante_buscado_por_estado, TRIPULANTE, segmento_buscado);

					list_replace(patota_buscada->segmentos, indice, segmento_buscado);
					sem_post(mutex_segmentos);

				}
				else if(esquema_elegido  == 'P') {

					t_tabla_paginas_patota* tabla_patota_buscada = buscar_tabla_patota(tripulante_por_estado->id_patota);

					int32_t direccion_logica = buscar_pagina_por_id(tabla_patota_buscada, tripulante_por_estado->id_tripulante);

					int32_t direccion_fisica = obtener_direc_fisica_con_direccion_logica(direccion_logica, tabla_patota_buscada);

					printf("Dirección lógica: %u\n", direccion_logica);
					printf("Dirección física: %u\n", direccion_fisica);

					tripulante_buscado_por_estado = encontrar_tripulante_memoria(direccion_fisica);

					estado_anterior = tripulante_buscado_por_estado->estado_tripulante;

					tripulante_buscado_por_estado->estado_tripulante = tripulante_por_estado->estado;

					//TODO: actualizar la posicion del tripulante en el MAPA

					actualizar_tripulante_memoria(tripulante_buscado_por_estado, direccion_fisica);
					actualizar_referencia(tabla_patota_buscada->paginas, direccion_logica);
				}

				respuesta_por_estado->id_tripulante = tripulante_por_estado->id_tripulante;
				respuesta_por_estado->respuesta = 1;

				log_info(logger, "El tripulante %u de la Patota %u cambió del Estado %c al Estado %c.\n", tripulante_buscado_por_estado->id_tripulante, tripulante_por_estado->id_patota, estado_anterior, tripulante_buscado_por_estado->estado_tripulante);

				enviar_mensaje(respuesta_por_estado, RESPUESTA_OK_ESTADO, conexion);

				cerrar_conexion(logger, conexion);

				free(tripulante_buscado_por_estado);
				free(tripulante_por_estado);
				free(respuesta_por_estado);
				break;

			case PEDIDO_TAREA:
				tripulante_para_tarea = malloc(sizeof(t_tripulante));
				respuesta_con_tarea_tripulante = malloc(sizeof(t_respuesta_tarea_tripulante));
				respuesta_con_tarea_tripulante->tarea = malloc(sizeof(t_tarea));

				recibir_mensaje(tripulante_para_tarea, operacion, conexion);

				t_tcb* tripulante_con_tarea;

				if(esquema_elegido == 'S') {

					int indice = 0;

					sem_wait(mutex_segmentos);
					t_tabla_segmentos_patota* patota_buscada = buscar_tabla_de_patota(tripulante_para_tarea->id_patota);
					t_segmento* segmento_buscado = buscar_por_id(patota_buscada->segmentos, TRIPULANTE, tripulante_para_tarea->id_tripulante);
					sem_post(mutex_segmentos);

					indice = obtener_indice(patota_buscada->segmentos, segmento_buscado);

					sem_wait(mutex_segmentos);
					tripulante_con_tarea = obtener_contenido_de_segmento(segmento_buscado);
					sem_post(mutex_segmentos);
					int32_t id_tarea_a_buscar_del_tripu = tripulante_con_tarea->id_tarea_a_realizar;

					sem_wait(mutex_segmentos);
					t_tarea* tarea_buscada = buscar_proxima_tarea_del_tripulante_segmentacion(patota_buscada->segmentos, TAREAS, id_tarea_a_buscar_del_tripu, patota_buscada->tamanio_tareas);
					sem_post(mutex_segmentos);

					respuesta_con_tarea_tripulante->id_tripulante = tripulante_para_tarea->id_tripulante;
					respuesta_con_tarea_tripulante->respuesta = 1;

					if(tarea_buscada != NULL) {
						respuesta_con_tarea_tripulante->tarea->operacion = tarea_buscada->operacion;
						respuesta_con_tarea_tripulante->tarea->cantidad = tarea_buscada->cantidad;
						respuesta_con_tarea_tripulante->tarea->posicion_x = tarea_buscada->posicion_x;
						respuesta_con_tarea_tripulante->tarea->posicion_y = tarea_buscada->posicion_y;
						respuesta_con_tarea_tripulante->tarea->tiempo = tarea_buscada->tiempo;
						tripulante_con_tarea->id_tarea_a_realizar++;
					}
					else {
						respuesta_con_tarea_tripulante->tarea->operacion = TAREA_VACIA;
						respuesta_con_tarea_tripulante->tarea->cantidad = 0;
						respuesta_con_tarea_tripulante->tarea->posicion_x = 0;
						respuesta_con_tarea_tripulante->tarea->posicion_y = 0;
						respuesta_con_tarea_tripulante->tarea->tiempo = 0;
					}

					tarea_buscada = NULL;
					free(tarea_buscada);

					sem_wait(mutex_segmentos);
					actualizar_segmento(tripulante_con_tarea, TRIPULANTE, segmento_buscado);

					list_replace(patota_buscada->segmentos, indice, segmento_buscado);
					sem_post(mutex_segmentos);

				}
				else if(esquema_elegido  == 'P') {

					t_tabla_paginas_patota* tabla_patota_buscada = buscar_tabla_patota(tripulante_para_tarea->id_patota);

					int32_t direccion_logica = buscar_pagina_por_id(tabla_patota_buscada, tripulante_para_tarea->id_tripulante);

					int32_t direccion_fisica = obtener_direc_fisica_con_direccion_logica(direccion_logica, tabla_patota_buscada);

					printf("Dirección lógica: %u\n", direccion_logica);
					printf("Dirección física: %u\n", direccion_fisica);

					printf("Dirección física de las tareas: %u\n", tabla_patota_buscada->patota->tareas);

					tripulante_con_tarea = encontrar_tripulante_memoria(direccion_fisica);
					int32_t id_tarea_a_buscar_del_tripu = tripulante_con_tarea->id_tarea_a_realizar;

					t_tarea* tarea_buscada = buscar_proxima_tarea_del_tripulante_paginacion(tabla_patota_buscada->patota->tareas, id_tarea_a_buscar_del_tripu, tabla_patota_buscada->tamanio_tareas);

					respuesta_con_tarea_tripulante->id_tripulante = tripulante_para_tarea->id_tripulante;
					respuesta_con_tarea_tripulante->respuesta = 1;

					if(tarea_buscada != NULL) {
						respuesta_con_tarea_tripulante->tarea->operacion = tarea_buscada->operacion;
						respuesta_con_tarea_tripulante->tarea->cantidad = tarea_buscada->cantidad;
						respuesta_con_tarea_tripulante->tarea->posicion_x = tarea_buscada->posicion_x;
						respuesta_con_tarea_tripulante->tarea->posicion_y = tarea_buscada->posicion_y;
						respuesta_con_tarea_tripulante->tarea->tiempo = tarea_buscada->tiempo;
						tripulante_con_tarea->id_tarea_a_realizar++;
					}
					else {
						respuesta_con_tarea_tripulante->tarea->operacion = TAREA_VACIA;
						respuesta_con_tarea_tripulante->tarea->cantidad = 0;
						respuesta_con_tarea_tripulante->tarea->posicion_x = 0;
						respuesta_con_tarea_tripulante->tarea->posicion_y = 0;
						respuesta_con_tarea_tripulante->tarea->tiempo = 0;
					}

					tarea_buscada = NULL;
					free(tarea_buscada);

					actualizar_tripulante_memoria(tripulante_con_tarea, direccion_fisica);
					actualizar_referencia(tabla_patota_buscada->paginas, direccion_logica);
				}

				log_info(logger, "Se envió la próxima tarea que tendrá que ejecutar el Tripulante %u de la Patota %u.\n", respuesta_con_tarea_tripulante->id_tripulante, tripulante_para_tarea->id_patota);

				enviar_mensaje(respuesta_con_tarea_tripulante, RESPUESTA_NUEVA_TAREA, conexion);

				cerrar_conexion(logger, conexion);

				free(tripulante_con_tarea);
				free(tripulante_para_tarea);
				free(respuesta_con_tarea_tripulante->tarea);
				free(respuesta_con_tarea_tripulante);
				break;

			case EXPULSAR_TRIPULANTE:
				tripulante_a_eliminar = malloc(sizeof(t_tripulante));
				respuesta_tripulante_eliminado = malloc(sizeof(t_respuesta_tripulante));

				recibir_mensaje(tripulante_a_eliminar, operacion, conexion);

				if(esquema_elegido == 'S') {

					int indice = 0;

					sem_wait(mutex_segmentos);
					t_tabla_segmentos_patota* patota_buscada = buscar_tabla_de_patota(tripulante_a_eliminar->id_patota);
					t_segmento* segmento_buscado = buscar_por_id(patota_buscada->segmentos, TRIPULANTE, tripulante_a_eliminar->id_tripulante);
					sem_post(mutex_segmentos);

					indice = obtener_indice(patota_buscada->segmentos, segmento_buscado);

					sem_wait(mutex_segmentos);
					list_remove(patota_buscada->segmentos, indice);
					sem_post(mutex_segmentos);

					liberar_segmento(segmento_buscado);


					// Tienen que haber 2 segmentos en una patota, los cuales son el PCB y las TAREAS
					if(list_size(patota_buscada->segmentos) == 2) {

						list_clean_and_destroy_elements(patota_buscada->segmentos, (void*)liberar_segmento);

						/*for(int i=0; i<list_size(patota_buscada->segmentos); i++) {

							list_remove(patota_buscada->segmentos, i);
							liberar_segmento(list_get(patota_buscada->segmentos, i));

						}*/


						int indice_patota = obtener_indice(tablas_segmentos, patota_buscada);
						list_remove(tablas_segmentos, indice_patota);
						//list_destroy(patota_buscada->segmentos);
						free(patota_buscada->patota);
						free(patota_buscada);
					}




				}
				else if(esquema_elegido  == 'P') {
					t_tabla_paginas_patota* tabla_patota_buscada = buscar_tabla_patota(tripulante_a_eliminar->id_patota);

					int32_t direccion_logica = buscar_pagina_por_id(tabla_patota_buscada, tripulante_a_eliminar->id_tripulante);

					int32_t direccion_fisica = obtener_direc_fisica_con_direccion_logica(direccion_logica, tabla_patota_buscada);

					printf("Dirección lógica: %u\n", direccion_logica);
					printf("Dirección física: %u\n", direccion_fisica);

					//int indice = 0;

					/*
					 * Implementar lógica para eliminar la memoria que involucra a este tripulante
					 *
					 *
					 * - De haber eliminado a todos los tripulantes, tambien eliminar a la patota con sus tareas
					 * 			 Esto hace que los frames que ocupa esta páginas se liberen,
					 * 			 lo mismo con las paginas del proceso, van a quedar libres
					 */

				}

				respuesta_tripulante_eliminado->respuesta = 1;
				respuesta_tripulante_eliminado->id_tripulante = tripulante_a_eliminar->id_tripulante;

				log_info(logger, "Se ha expulsado al Tripulante %u de la Patota %u de la nave.\n", tripulante_a_eliminar->id_tripulante, tripulante_a_eliminar->id_patota);

				enviar_mensaje(respuesta_tripulante_eliminado, RESPUESTA_TRIPULANTE_ELIMINADO, conexion);

				cerrar_conexion(logger, conexion);

				free(tripulante_a_eliminar);
				free(respuesta_tripulante_eliminado);
				break;

			case CERRAR_MODULO:
				cerrar_conexion(logger, conexion);

				printf("Terminando programa... \n");
				sleep(1);
				printf("-------------------------------------------------------------------------------------------------------------------------------------------------- \n");
				free(crear_segmento_sem);
				free(memoria_principal);
				printf("Memoria Principal liberada...\n");

				if(esquema_elegido == 'P') {
					//free(area_swap);
					//printf("Area de Swap liberada...\n\n");
					list_destroy(tablas_paginas);
					free(frames);
				}

				log_info(logger, "Se ha cerrado el programa de forma exitosa.\n");
				terminar_programa(config, logger);
				exit(0);

			default:
				log_warning(logger, "Operacion desconocida. No quieras meter la pata");
				break;
			}
}


t_pcb* crear_pcb() {
	t_pcb* proceso_patota =  malloc(sizeof(t_pcb));
	proceso_patota->pid = contador_id_patota;
	proceso_patota->tareas = 0; //Direccion de memoria de las tareas
	return proceso_patota;
}


t_tcb* crear_tcb(uint32_t dir_logica_pcb, uint32_t posicion_x, uint32_t posicion_y) {

	t_tcb* tripulante = malloc(sizeof(t_tcb));
	tripulante->id_tripulante = contador_id_tripu;
	tripulante->estado_tripulante = 'N';
	tripulante->posicion_x = posicion_x;
	tripulante->posicion_y = posicion_y;
	tripulante->id_tarea_a_realizar = 0;
	tripulante->puntero_PCB = dir_logica_pcb;

	return tripulante;
}


uint32_t cantidad_tareas(char** parser_tarea) {

	int cantidad = 0;
	while(parser_tarea[cantidad] != NULL){
		cantidad++;
	}
	return cantidad;
}


void chequear_memoria(void) {
	log_info(logger, "La memoria libre total es de %i.\n", memoria_libre_total);
}


codigo_memoria mapeo_esquema_memoria(char* ESQUEMA)
{
	codigo_memoria esquema_memoria;

	if(strcmp(ESQUEMA, "PAGINACION") == 0) {
		esquema_memoria = PAGINACION;
	}

	if(strcmp(ESQUEMA, "SEGMENTACION") == 0) {
		esquema_memoria = SEGMENTACION;
	}

	return esquema_memoria;
}


// Elige el esquema de memoria a utilizar e inicializa la misma
void elegir_esquema_de_memoria(char* ESQUEMA)
{
	codigo_memoria cod_mem;

	cod_mem = mapeo_esquema_memoria(ESQUEMA);

	switch(cod_mem) {

		case PAGINACION:

			esquema_elegido = 'P';
			tablas_paginas = list_create();
			puntero_inicio = 0;
			log_info(logger, "Las páginas tendran un tamaño de %u bytes cada una.\n", TAMANIO_PAGINA);
			log_info(logger, "Se utilizará el algoritmo de %s para reemplazar las páginas.\n", ALGORITMO_REEMPLAZO);
			inicializar_frames();
			inicializar_swap();

			break;

		case SEGMENTACION:

			esquema_elegido = 'S';
			log_info(logger, "Se utilizará el criterio de %s para colocar el segmento en memoria.\n", CRITERIO_SELECCION);
			tablas_segmentos = list_create();
			segmentos = list_create();
			base_segmento = 0;
			crear_segmento_libre(0, TAMANIO_MEMORIA);
			break;

		default:
			log_error(logger, "No se eligió ningún esquema de memoria, por lo que no se puede seguir con el programa.");
			abort();
			break;
	}
}


bool validar_espacio_por_patota_segmentacion(uint32_t tamanio) {
	int32_t restante = memoria_libre_total - tamanio;
	return (restante >= 0);
}

bool validar_espacio_por_patota_paginacion(uint32_t tamanio) {
	int32_t restante = memoria_libre_total - tamanio;
	if(restante < 0) {
		int32_t restante_memoria_virtual = memoria_virtual_total - tamanio;
		return restante_memoria_virtual >= 0;
	}
	return (restante >= 0);
}

algoritmo_reemplazo elegir_algoritmo_reemplazo(char* algoritmo){
	algoritmo_reemplazo algoritmo_reemplazo;

	if(strcmp(algoritmo, "LRU") == 0) {
		algoritmo_reemplazo = LRU;
	}
	if(strcmp(algoritmo, "CLOCK") == 0) {
		algoritmo_reemplazo = CLOCK;
	}

	return algoritmo_reemplazo;
}


criterio_seleccion elegir_criterio_seleccion(char* criterio){
	criterio_seleccion criterio_seleccionado;

	if(strcmp(criterio, "BEST_FIT") == 0 || strcmp(criterio, "BF") == 0) {
		criterio_seleccionado = BEST_FIT;
	}
	if(strcmp(criterio, "FIRST_FIT") == 0 || strcmp(criterio, "FF") == 0) {
		criterio_seleccionado = FIRST_FIT;
	}

	return criterio_seleccionado;
}

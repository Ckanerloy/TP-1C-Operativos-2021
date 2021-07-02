#include "Mi-RAM.h"

int main(void) {

	config = crear_config(CONFIG_PATH);
	obtener_datos_de_config(config);
	logger = crear_log("mi-ram-hq.log", "Mi-RAM HQ");

	iniciar_variables_y_semaforos();

	signal(SIGUSR1, iniciar_dump_memoria);

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
	ids = list_create();

	crear_segmento_sem = malloc(sizeof(sem_t));
	sem_init(crear_segmento_sem, 0, 0);
}

void inicializar_memoria(void) {
	log_info(logger, "Se utilizará %s como esquema de memoria.\n", ESQUEMA_MEMORIA);

	memoria_principal = malloc(TAMANIO_MEMORIA);
	memoria_restante = TAMANIO_MEMORIA;

	if(memoria_principal != NULL){
		log_info(logger, "Se inició la Memoria Principal con un tamaño de %u bytes.\n", TAMANIO_MEMORIA);
	}
	else {
		log_error(logger, "Error al iniciar la Memoria Principal.\n");
		sleep(1);
		abort();
	}

	area_swap = malloc(TAMANIO_SWAP);
	if(area_swap != NULL){
		log_info(logger, "Se inició el Área de Swap con un tamaño de %u bytes.\n", TAMANIO_SWAP);
	}
	else{
		log_error(logger, "Error al iniciar el Area de Swap.\n");
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

		int32_t* conexion_cliente = esperar_conexion(conexion_servidor);

		pthread_create(&hilo_recibir_mensajes, NULL,(void*)escuchar_conexion, conexion_cliente);

		pthread_detach(hilo_recibir_mensajes);

	}
}



void procesar_mensajes(codigo_operacion operacion, int32_t conexion) {

	// INICIAR_PATOTA
	t_iniciar_patota* patota_recibida;
	t_respuesta_iniciar_patota* respuesta_iniciar_patota;
	int32_t tamanio_total;
	t_list* tareas_de_la_patota;

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
				log_trace(logger, "SE INICIARÁ UNA PATOTA CON SUS TRIPULANTES.");
				patota_recibida = malloc(sizeof(t_iniciar_patota));
				respuesta_iniciar_patota = malloc(sizeof(t_respuesta_iniciar_patota));
				t_tabla_segmentos_patota* tabla_patota;
				recibir_mensaje(patota_recibida, operacion, conexion);

				//printf("Cantidad de tripulantes: %d \n" , patota_recibida->cantidad_tripulantes);
				//printf("Contenido de tareas: %s \n", patota_recibida->tareas_de_patota);
				//printf("Posiciones de los tripulantes: %s \n", patota_recibida->posiciones);
				parser_posiciones = string_split(patota_recibida->posiciones, "|");

				char* ids_enviar = string_new();

				// Tareas de UNA Patota
				string_trim(&patota_recibida->tareas_de_patota);
				char** parser_tarea = obtener_tareas(patota_recibida->tareas_de_patota);

				uint32_t cant_tareas = cantidad_tareas(parser_tarea);

				tareas_de_la_patota = list_create();

				for(uint32_t i=0; i<cant_tareas; i++){
					list_add_in_index(tareas_de_la_patota, i, obtener_la_tarea(parser_tarea[i]));
				}

				tamanio_tripulante = sizeof(uint32_t) + sizeof(char) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
				tamanio_tripulantes = tamanio_tripulante * patota_recibida->cantidad_tripulantes;
				tamanio_patota = sizeof(t_pcb);
				tamanio_tareas = sizeof(t_tarea) * list_size(tareas_de_la_patota);

				tamanio_total = tamanio_patota + tamanio_tareas + tamanio_tripulantes;

				if(esquema_elegido == 'S') {
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

						t_segmento* segmento_patota = administrar_guardar_segmento(nueva_patota, PATOTA, tamanio_patota);
						list_add(tabla_patota->segmentos, segmento_patota);
						uint32_t direccion_pcb = segmento_patota->inicio;

						log_info(logger, "Se inició la Patota %u con %u tripulante/s.\n", nueva_patota->pid, patota_recibida->cantidad_tripulantes);

						sem_wait(crear_segmento_sem);

						t_segmento* segmento_tareas = administrar_guardar_segmento(tareas_de_la_patota, TAREAS, tamanio_tareas);
						list_add(tabla_patota->segmentos, segmento_tareas);
						tabla_patota->patota->tareas = segmento_tareas->inicio;

						log_info(logger, "Se guardaron las tareas de la Patota %u, las cuales son: %s\n",nueva_patota->pid, patota_recibida->tareas_de_patota);

						sem_wait(crear_segmento_sem);

						int posicion = 0;
						for(int i=0;i<patota_recibida->cantidad_tripulantes;i++){

							t_tcb* nuevo_tripulante = crear_tcb(direccion_pcb, atoi(parser_posiciones[posicion]), atoi(parser_posiciones[posicion+1]));

							t_segmento* segmento_tripulante = administrar_guardar_segmento(nuevo_tripulante, TRIPULANTE, tamanio_tripulante);
							list_add(tabla_patota->segmentos, segmento_tripulante);

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

					log_info(logger, "El tamaño de la memoria restante es de %i.\n", memoria_restante);

					printf("Inicio de proximo segmento: %d \n", base_segmento);
					printf("Numero de proximo segmento: %d \n\n\n", contador_segmento);

					enviar_mensaje(respuesta_iniciar_patota, RESPUESTA_INICIAR_PATOTA, conexion);
				}

				else if(esquema_elegido  == 'P') {
					//crear_pagina(estructura, tipo_estructura);


				}

				else {
					log_error(logger, "No se puede guardar la estructura en Memoria");
				}

				free(ids_enviar);
				free(parser_tarea);
				free(tareas_de_la_patota);
				free(respuesta_iniciar_patota->ids_tripu);
				free(respuesta_iniciar_patota);
				free(patota_recibida->tareas_de_patota);
				free(patota_recibida->posiciones);
				free(patota_recibida);
				break;

			case ACTUALIZAR_UBICACION_TRIPULANTE:
				log_trace(logger, "SE ACTUALIZARÁ LA POSICIÓN DE UN TRIPULANTE.");
				tripulante_por_ubicacion = malloc(sizeof(t_tripulante_ubicacion));
				respuesta_ok_ubicacion = malloc(sizeof(t_respuesta_tripulante));

				recibir_mensaje(tripulante_por_ubicacion, operacion, conexion);

				t_tcb* tripulante_buscado_por_ubicacion;

				if(esquema_elegido == 'S') {

					int indice = 0;

					t_tabla_segmentos_patota* patota_buscada = buscar_tabla_de_patota(tripulante_por_ubicacion->id_patota);

					t_segmento* segmento_buscado = buscar_por_id(patota_buscada->segmentos, TRIPULANTE, tripulante_por_ubicacion->id_tripulante);

					indice = obtener_indice(patota_buscada->segmentos, segmento_buscado);

					tripulante_buscado_por_ubicacion = obtener_contenido_de_segmento(segmento_buscado);

					tripulante_buscado_por_ubicacion->posicion_x = tripulante_por_ubicacion->posicion_x;
					tripulante_buscado_por_ubicacion->posicion_y = tripulante_por_ubicacion->posicion_y;

					//falta actualizar la posicion del tripulante en el MAPA

					actualizar_segmento(tripulante_buscado_por_ubicacion, TRIPULANTE, segmento_buscado);

					list_replace(patota_buscada->segmentos, indice, segmento_buscado);

				}
				else if(esquema_elegido  == 'P') {
					//crear_pagina(estructura, tipo_estructura);

				}
				else {
					log_error(logger, "No se puede guardar la estructura en Memoria");
				}

				respuesta_ok_ubicacion->id_tripulante = tripulante_por_ubicacion->id_tripulante;
				respuesta_ok_ubicacion->respuesta = 1;

				log_info(logger, "Se actualizaron las posiciones del Tripulante %u de la Patota %u, siendo estas la posición X: %u y la posición Y: %u.\n", tripulante_por_ubicacion->id_tripulante, tripulante_por_ubicacion->id_patota, tripulante_por_ubicacion->posicion_x, tripulante_por_ubicacion->posicion_y);

				enviar_mensaje(respuesta_ok_ubicacion, RESPUESTA_OK_UBICACION, conexion);

				free(tripulante_por_ubicacion);
				free(respuesta_ok_ubicacion);
				break;

			case PEDIR_UBICACION_TRIPULANTE:
				log_trace(logger, "SE RETORNARÁ LA UBICACIÓN DE UN TRIPULANTE.");
				tripulante_para_ubicacion = malloc(sizeof(t_tripulante));
				respuesta_con_ubicacion = malloc(sizeof(t_respuesta_tripulante_ubicacion));

				recibir_mensaje(tripulante_para_ubicacion, operacion, conexion);

				if(esquema_elegido == 'S') {

					t_tabla_segmentos_patota* patota_buscada = buscar_tabla_de_patota(tripulante_para_ubicacion->id_patota);

					t_segmento* segmento_buscado = buscar_por_id(patota_buscada->segmentos, TRIPULANTE, tripulante_para_ubicacion->id_tripulante);

					t_tcb* tripulante_con_ubicacion = obtener_contenido_de_segmento(segmento_buscado);

					respuesta_con_ubicacion->posicion_x = tripulante_con_ubicacion->posicion_x;
					respuesta_con_ubicacion->posicion_y = tripulante_con_ubicacion->posicion_y;

				}
				else if(esquema_elegido  == 'P') {
					//crear_pagina(estructura, tipo_estructura);

				}
				else {
					log_error(logger, "No se puede guardar la estructura en Memoria");
				}

				respuesta_con_ubicacion->id_tripulante = tripulante_para_ubicacion->id_tripulante;
				respuesta_con_ubicacion->respuesta = 1;

				log_info(logger, "Se enviaron las posiciones X: %u e Y: %u del Tripulante %u de la Patota %u.\n", respuesta_con_ubicacion->posicion_x, respuesta_con_ubicacion->posicion_y, respuesta_con_ubicacion->id_tripulante, tripulante_para_ubicacion->id_patota);

				enviar_mensaje(respuesta_con_ubicacion, RESPUESTA_NUEVA_UBICACION, conexion);

				free(tripulante_para_ubicacion);
				free(respuesta_con_ubicacion);
				break;

			case ACTUALIZAR_ESTADO_TRIPULANTE:
				log_trace(logger, "SE ACTUALIZARÁ EL ESTADO DE UN TRIPULANTE.");
				tripulante_por_estado = malloc(sizeof(t_tripulante_estado));
				respuesta_por_estado = malloc(sizeof(t_respuesta_tripulante));

				recibir_mensaje(tripulante_por_estado, operacion, conexion);

				char estado_anterior;
				t_tcb* tripulante_buscado_por_estado;

				if(esquema_elegido == 'S') {

					int indice = 0;

					t_tabla_segmentos_patota* patota_buscada = buscar_tabla_de_patota(tripulante_por_estado->id_patota);

					t_segmento* segmento_buscado = buscar_por_id(patota_buscada->segmentos, TRIPULANTE, tripulante_por_estado->id_tripulante);

					indice = obtener_indice(patota_buscada->segmentos, segmento_buscado);

					tripulante_buscado_por_estado = obtener_contenido_de_segmento(segmento_buscado);

					estado_anterior = tripulante_buscado_por_estado->estado_tripulante;

					tripulante_buscado_por_estado->estado_tripulante = tripulante_por_estado->estado;

					actualizar_segmento(tripulante_buscado_por_estado, TRIPULANTE, segmento_buscado);

					list_replace(patota_buscada->segmentos, indice, segmento_buscado);

				}
				else if(esquema_elegido  == 'P') {
					//crear_pagina(estructura, tipo_estructura);

				}
				else {
					log_error(logger, "No se puede guardar la estructura en Memoria");
				}

				respuesta_por_estado->id_tripulante = tripulante_por_estado->id_tripulante;
				respuesta_por_estado->respuesta = 1;

				log_info(logger, "El tripulante %u de la Patota %u cambió del Estado %c al Estado %c.\n", tripulante_buscado_por_estado->id_tripulante, tripulante_por_estado->id_patota, estado_anterior, tripulante_buscado_por_estado->estado_tripulante);

				enviar_mensaje(respuesta_por_estado, RESPUESTA_OK_ESTADO, conexion);

				free(respuesta_por_estado);
				break;

			case PEDIDO_TAREA:
				log_trace(logger, "SE RETORNARÁ LA PRÓXIMA TAREA A REALIZAR DE UN TRIPULANTE.");
				tripulante_para_tarea = malloc(sizeof(t_tripulante));
				respuesta_con_tarea_tripulante = malloc(sizeof(t_respuesta_tarea_tripulante));
				respuesta_con_tarea_tripulante->tarea = malloc(sizeof(t_tarea));

				recibir_mensaje(tripulante_para_tarea, operacion, conexion);

				if(esquema_elegido == 'S') {

					int indice = 0;

					t_tabla_segmentos_patota* patota_buscada = buscar_tabla_de_patota(tripulante_para_tarea->id_patota);

					t_segmento* segmento_buscado = buscar_por_id(patota_buscada->segmentos, TRIPULANTE, tripulante_para_tarea->id_tripulante);

					indice = obtener_indice(patota_buscada->segmentos, segmento_buscado);

					t_tcb* tripulante_con_tarea = obtener_contenido_de_segmento(segmento_buscado);
					int id_tarea_a_buscar_del_tripu = tripulante_con_tarea->id_tarea_a_realizar;

					t_tarea* tarea_buscada = buscar_proxima_tarea_del_tripulante(patota_buscada->segmentos, TAREAS, id_tarea_a_buscar_del_tripu);

					respuesta_con_tarea_tripulante->id_tripulante = tripulante_para_tarea->id_tripulante;
					respuesta_con_tarea_tripulante->respuesta = 1;
					respuesta_con_tarea_tripulante->tarea->operacion = tarea_buscada->operacion;
					respuesta_con_tarea_tripulante->tarea->cantidad = tarea_buscada->cantidad;
					respuesta_con_tarea_tripulante->tarea->posicion_x = tarea_buscada->posicion_x;
					respuesta_con_tarea_tripulante->tarea->posicion_y = tarea_buscada->posicion_y;
					respuesta_con_tarea_tripulante->tarea->tiempo = tarea_buscada->tiempo;

					tripulante_con_tarea->id_tarea_a_realizar++;

					actualizar_segmento(tripulante_con_tarea, TRIPULANTE, segmento_buscado);

					list_replace(patota_buscada->segmentos, indice, segmento_buscado);

				}
				else if(esquema_elegido  == 'P') {
					//crear_pagina(estructura, tipo_estructura);

				}
				else {
					log_error(logger, "No se puede guardar la estructura en Memoria");
				}

				log_info(logger, "Se envió la próxima tarea que tendrá que ejecutar el Tripulante %u de la Patota %u.\n", respuesta_con_tarea_tripulante->id_tripulante, tripulante_para_tarea->id_patota);

				enviar_mensaje(respuesta_con_tarea_tripulante, RESPUESTA_NUEVA_TAREA, conexion);

				free(tripulante_para_tarea);
				free(respuesta_con_tarea_tripulante);
				break;

			case EXPULSAR_TRIPULANTE:
				log_trace(logger, "SE EXPULSARÁ UN TRIPULANTE.");
				tripulante_a_eliminar = malloc(sizeof(t_tripulante));
				respuesta_tripulante_eliminado = malloc(sizeof(t_respuesta_tripulante));

				recibir_mensaje(tripulante_a_eliminar, operacion, conexion);

				printf("Tripulante a Expulsar: %u \n", tripulante_a_eliminar->id_tripulante);
				printf("Patota del tripulante: %u \n", tripulante_a_eliminar->id_patota);


				if(esquema_elegido == 'S') {

					int indice = 0;

					t_tabla_segmentos_patota* patota_buscada = buscar_tabla_de_patota(tripulante_a_eliminar->id_patota);

					t_segmento* segmento_buscado = buscar_por_id(patota_buscada->segmentos, TRIPULANTE, tripulante_a_eliminar->id_tripulante);

					indice = obtener_indice(patota_buscada->segmentos, segmento_buscado);

					log_info(logger, "Memoria Total antes de expulsar tripulante: %u.\n\n\n", memoria_libre_total);

					list_remove(patota_buscada->segmentos, indice);

					liberar_segmento(segmento_buscado);

					log_info(logger, "Memoria Total despues de expulsar tripulante: %u.\n\n\n", memoria_libre_total);

// Verificar si es correcto eliminar el segmento dentro de los segmentos de patota
//	y dejarlo dentro de los segmentos generales del sistema marcado como LIBRE


				}
				else if(esquema_elegido  == 'P') {
					//crear_pagina(estructura, tipo_estructura);

				}
				else {
					log_error(logger, "No se puede guardar la estructura en Memoria");
				}

				respuesta_tripulante_eliminado->respuesta = 1;
				respuesta_tripulante_eliminado->id_tripulante = tripulante_a_eliminar->id_tripulante;

				log_info(logger, "Se ha expulsado al Tripulante %u de la Patota %u de la nave.\n", tripulante_a_eliminar->id_tripulante, tripulante_a_eliminar->id_patota);

				enviar_mensaje(respuesta_tripulante_eliminado, RESPUESTA_TRIPULANTE_ELIMINADO, conexion);

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
				free(area_swap);
				printf("Area de Swap liberada...\n\n");
				log_info(logger, "Se ha cerrado el programa de forma exitosa.\n");
				terminar_programa(config, logger);
				exit(0);

			default:
				log_warning(logger, "Operacion desconocida. No quieras meter la pata");
				break;
			}
}


bool validar_espacio_por_patota_segmentacion(uint32_t tamanio) {
	int32_t restante = memoria_libre_total - tamanio;
	return (restante >= 0);
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





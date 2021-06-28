#include "Mi-RAM.h"

int main(void) {

	config = crear_config(CONFIG_PATH);
	obtener_datos_de_config(config);
	logger = crear_log("mi-ram-hq.log", "Mi-RAM HQ");

	iniciar_variables_y_semaforos();

	inicializar_memoria();
	elegir_esquema_de_memoria(ESQUEMA_MEMORIA);
	criterio_elegido = elegir_criterio_seleccion(CRITERIO_SELECCION);
	algoritmo_elegido = elegir_algoritmo_reemplazo(ALGORITMO_REEMPLAZO);

	iniciar_mapa();

	iniciar_dump_memoria();

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
	printf("Tamanio de Memoria Principal: %u \n", TAMANIO_MEMORIA);
	printf("Esquema de Memoria utilizado: %s \n", ESQUEMA_MEMORIA);
	printf("Tamanio de Pagina: %u \n", TAMANIO_PAGINA);
	printf("Tamanio Area Swap: %u \n", TAMANIO_SWAP);
	printf("Algoritmo de Reemplazo: %s \n", ALGORITMO_REEMPLAZO);
	printf("Criterio de Seleccion para meter en Segmentacion: %s \n", CRITERIO_SELECCION);

	memoria_principal = malloc(TAMANIO_MEMORIA);
	memoria_restante = TAMANIO_MEMORIA;

	if(memoria_principal != NULL){
		printf("Memoria Principal iniciada... \n");
	}
	else {
		log_error(logger, "Error al iniciar la Memoria Principal.\n");
		sleep(1);
		abort();
	}

	area_swap = malloc(TAMANIO_SWAP);
	if(area_swap != NULL){
		printf("Area de Swap iniciada...\n");
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

void iniciar_dump_memoria(void) {

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
	t_respuesta_tripulante_ubicacion* respuesta_por_ubicacion;

	// ACTUALIZAR_ESTADO_TRIPULANTE
	t_tripulante_estado* tripulante_por_estado;
	t_respuesta_tripulante* respuesta_por_estado;

	// PEDIDO_TAREA
	t_tripulante* tripulante_para_tarea;
	t_respuesta_tarea_tripulante* respuesta_tarea_tripulante;

	// EXPULSAR_TRIPULANTE
	t_tripulante* tripulante_a_eliminar;
	t_respuesta_tripulante* respuesta_tripulante_eliminado;


	switch(operacion)
{
			case INICIAR_PATOTA:
				patota_recibida = malloc(sizeof(t_iniciar_patota));
				respuesta_iniciar_patota = malloc(sizeof(t_respuesta_iniciar_patota));

				recibir_mensaje(patota_recibida, operacion, conexion);

				t_tabla_segmentos_patota* tabla_patota;


				printf("Cantidad de tripulantes: %d \n" , patota_recibida->cantidad_tripulantes);
				printf("Contenido de tareas: %s \n", patota_recibida->tareas_de_patota);
				printf("Posiciones de los tripulantes: %s \n", patota_recibida->posiciones);
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


				for(uint32_t c=0; c<cant_tareas; c++) {
					t_tarea* tarea_ejemplo = list_get(tareas_de_la_patota, c);

					printf("Operacion: %u\n", tarea_ejemplo->operacion);
					printf(" - Cantidad: %u\n", tarea_ejemplo->cantidad);
					printf(" - Posicion X: %u\n", tarea_ejemplo->posicion_x);
					printf(" - Posicion Y: %u\n", tarea_ejemplo->posicion_y);
					printf(" - Tiempo: %u\n", tarea_ejemplo->tiempo);

					free(tarea_ejemplo);
				}

				tamanio_tripulante = sizeof(uint32_t) + sizeof(char) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t);
				tamanio_tripulantes = tamanio_tripulante * patota_recibida->cantidad_tripulantes;
				tamanio_patota = sizeof(t_pcb);
				tamanio_tareas = sizeof(t_tarea) * list_size(tareas_de_la_patota);

				tamanio_total = tamanio_patota + tamanio_tareas + tamanio_tripulantes;

				printf("Tamanio de un PCB: %d\n", sizeof(t_pcb));
				printf("Tamanio de un TCB: %d\n", tamanio_tripulante);
				printf("Tamanio de una tarea: %d\n", sizeof(t_tarea));


				if(esquema_elegido == 'S') {
					t_pcb* nueva_patota;

					// Verifica si hay espacio para guardar en memoria
					if(validar_espacio_por_patota_segmentacion(tamanio_total) == 0) {
						respuesta_iniciar_patota->numero_de_patota = 0;
						respuesta_iniciar_patota->respuesta = 0;
						respuesta_iniciar_patota->tamanio_ids = 0;
						respuesta_iniciar_patota->ids_tripu = malloc(respuesta_iniciar_patota->tamanio_ids+1);
						strcpy(respuesta_iniciar_patota->ids_tripu, "");

						log_error(logger, "No hay espacio suficiente para guardar la patota y sus tripulantes. \n");
					}
					else { // Hay suficiente espacio en memoria, puedo guardarlo y envio una respuesta a Discordiador

						nueva_patota = crear_pcb();

						tabla_patota = crear_tabla_segmentos(nueva_patota);



						t_segmento* segmento_patota = administrar_guardar_segmento(nueva_patota, PATOTA, tamanio_patota);
						list_add(tabla_patota->segmentos, segmento_patota);
						uint32_t direccion_pcb = segmento_patota->inicio;

						sem_wait(crear_segmento_sem);

						t_segmento* segmento_tareas = administrar_guardar_segmento(tareas_de_la_patota, TAREAS, tamanio_tareas);
						list_add(tabla_patota->segmentos, segmento_tareas);

						tabla_patota->patota->tareas = segmento_tareas->inicio;
						sem_wait(crear_segmento_sem);

						for(int i=0;i<patota_recibida->cantidad_tripulantes;i++){

							//uint32_t direccion_pcb = segmento_patota->inicio;
							t_tcb* nuevo_tripulante = crear_tcb(direccion_pcb, atoi(parser_posiciones[i]), atoi(parser_posiciones[i+1]));

							t_segmento* segmento_tripulante = administrar_guardar_segmento(nuevo_tripulante, TRIPULANTE, tamanio_tripulante);
							list_add(tabla_patota->segmentos, segmento_tripulante);


							//personaje_crear(amongOs, nuevo_tripulante->id_tripulante, nuevo_tripulante->posicion_x, nuevo_tripulante->posicion_y);


							sem_wait(crear_segmento_sem);

							string_append_with_format(&ids_enviar, "%u|", contador_id_tripu);
							contador_id_tripu++;
							free(nuevo_tripulante);
						}

						list_add(tablas_segmentos, tabla_patota);

						strcat(ids_enviar,"\0");


						respuesta_iniciar_patota->numero_de_patota = nueva_patota->pid;
						respuesta_iniciar_patota->respuesta = 1;
						respuesta_iniciar_patota->tamanio_ids = strlen(ids_enviar);
						respuesta_iniciar_patota->ids_tripu = malloc(respuesta_iniciar_patota->tamanio_ids+1);
						strcpy(respuesta_iniciar_patota->ids_tripu,ids_enviar);

						contador_id_patota++;

						log_info(logger, "Se ha guardado en memoria la patota y sus tripulantes. \n");
					}

					printf("Tamanio Patota %d \n", tamanio_total);
					printf("Tamanio Memoria Principal %d \n", TAMANIO_MEMORIA);
					printf("Memoria Restante: %d \n", memoria_restante);

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
				tripulante_por_ubicacion = malloc(sizeof(t_tripulante_ubicacion));
				respuesta_ok_ubicacion = malloc(sizeof(t_respuesta_tripulante));

				recibir_mensaje(tripulante_por_ubicacion, operacion, conexion);

				/*
				 * - BUSCAR TRIPULANTE EN LA MEMORIA (COINCIDIENDO POR ID_TRIPULANTE E ID_PATOTA)
				 * - CAMBIAR LAS POSICIONES X E Y DEL TRIPULANTE GUARDADO EN MEMORIA
				 * - GUARDARLO DE NUEVO
				 * - ACTUALIZAR LA POSICION DEL TRIPULANTE EN EL MAPA
				 * - ENVIAR UNA RESPUESTA DE CONFIRMACION DE ACTUALIZACION DE POSICION?
				 */
				respuesta_ok_ubicacion->id_tripulante = tripulante_por_ubicacion->id_tripulante;
				respuesta_ok_ubicacion->respuesta = 1;

				enviar_mensaje(respuesta_ok_ubicacion, RESPUESTA_OK_UBICACION, conexion);
				// LE CONFIRMO A DISCORDIADOR QUE SE ACTUALIZO EXITOSAMENTE LA UBICACION DEL TRIPULANTE

				free(tripulante_por_ubicacion);
				free(respuesta_ok_ubicacion);
				break;

			case PEDIR_UBICACION_TRIPULANTE:
				tripulante_para_ubicacion = malloc(sizeof(t_tripulante));
				respuesta_por_ubicacion = malloc(sizeof(t_respuesta_tripulante_ubicacion));

				recibir_mensaje(tripulante_para_ubicacion, operacion, conexion);

				/*
				 *  - BUSCAR TRIPULANTE EN LA MEMORIA (COINCIDIENDO POR ID_TRIPULANTE E ID_PATOTA)
				 *  - OBTENER LAS POSICIONES X E Y DEL TRIPULANTE GUARDADO
				 *  - GUARDARLAS EN LA ESTRUCTURA respuesta_por_ubicacion
				 *  - ENVIAR ESTA RESPUESTA AL DISCORDIADOR
				 */
				enviar_mensaje(respuesta_por_ubicacion, RESPUESTA_NUEVA_UBICACION, conexion);
				// LE ENVIA A DISCORDIADOR LA UBICACION ACTUAL DEL TRIPULANTE

				free(tripulante_para_ubicacion);
				free(respuesta_por_ubicacion);
				break;

			case ACTUALIZAR_ESTADO_TRIPULANTE:
				tripulante_por_estado = malloc(sizeof(t_tripulante_estado));
				respuesta_por_estado = malloc(sizeof(t_respuesta_tripulante));

				recibir_mensaje(tripulante_por_estado, operacion, conexion);

				/*
				 *  - BUSCAR TRIPULANTE EN LA MEMORIA (COINCIDIENDO POR ID_TRIPULANTE E ID_PATOTA)
				 *  - CAMBIAR EL ESTADO DEL TRIPULANTE POR EL QUE ME ENVIAN DESDE DISCORDIADOR
				 *  - ACTUALIZAR EL ESTADO DEL TRIPULANTE EN MEMORIA
				 *  - ENVIAR UNA RESPUESTA DE CONFIRMACION A DISCORDIADOR?
				 */


				if(esquema_elegido == 'S') {

					t_tabla_segmentos_patota* patota_buscada = buscar_tabla_de_patota(tripulante_por_estado->id_patota);

					printf("Tamanio patota buscada: %u \n", sizeof(patota_buscada));
// todO hay problemas de memoria cuando lee estos printf, pero aun asi los lee bien...
					printf("Id patota buscada: %u\n", patota_buscada->patota->pid);
					printf("Tareas de la patota buscada: %u\n\n", patota_buscada->patota->tareas);

					t_segmento* segmento_buscado = buscar_por_id_tripulante(patota_buscada->segmentos, TRIPULANTE, tripulante_por_estado->id_tripulante);

					printf("Id de segmento buscado: %u\n", segmento_buscado->id_segmento);
					printf("Inicio segmento buscado %u\n", segmento_buscado->inicio);
					printf("Tamanio segmento buscado %u\n\n", segmento_buscado->tamanio_segmento);

					t_tcb* tripulante_buscado = obtener_contenido_de_segmento(segmento_buscado);

					printf("Estado tripulante antes: %c\n", tripulante_buscado->estado_tripulante);

					tripulante_buscado->estado_tripulante = tripulante_por_estado->estado;

					printf("ID tripulante buscado: %u\n", tripulante_buscado->id_tripulante);
					printf("Estado tripulante despues buscado: %c\n", tripulante_buscado->estado_tripulante);
					printf("Posicion X tripulante buscado: %u\n", tripulante_buscado->posicion_x);
					printf("Posicion Y tripulante buscado: %u\n", tripulante_buscado->posicion_y);
					printf("ID tarea del tripulante buscado: %u\n", tripulante_buscado->id_tarea_a_realizar);
					printf("PCB puntero tripulante buscado: %u\n\n", tripulante_buscado->puntero_PCB);

					actualizar_segmento(tripulante_buscado, TRIPULANTE, segmento_buscado);
				}
				else if(esquema_elegido  == 'P') {
					//crear_pagina(estructura, tipo_estructura);
				}
				else {
					log_error(logger, "No se puede guardar la estructura en Memoria");
				}

				respuesta_por_estado->id_tripulante = tripulante_por_estado->id_tripulante;
				respuesta_por_estado->respuesta = 1;

				printf("Tripulante: %u\n", respuesta_por_estado->id_tripulante);
				printf("Estado: %c\n", tripulante_por_estado->estado);

				enviar_mensaje(respuesta_por_estado, RESPUESTA_OK_ESTADO, conexion);
				// LE CONFIRMA A DISCORDIADOR QUE SE REALIZO EXITOSAMENTE LA ACTUALIZACION DEL ESTADO DEL TRIPULANTE

				free(respuesta_por_estado);
				break;

			case PEDIDO_TAREA:
				tripulante_para_tarea = malloc(sizeof(t_tripulante));
				respuesta_tarea_tripulante = malloc(sizeof(t_respuesta_tarea_tripulante));

				recibir_mensaje(tripulante_para_tarea, operacion, conexion);

				/*
				 * - BUSCAR TRIPULANTE EN LA MEMORIA (COINCIDIENDO POR ID_TRIPULANTE E ID_PATOTA)
				 * - BUSCAR LA TAREA POR EL ID_PROXIMA_TAREA DEL TRIPULANTE EN MEMORIA
				 * - GUARDAR LA TAREA EN LA ESTRUCTURA t_respuesta_tarea_tripulante
				 * - ACTUALIZAR EL ID_TAREA_A_REALIZAR DEL TRIPULANTE Y GUARDARLO EN MEMORIA
				 * - ENVIAR LA RESPUESTA CON EL TRIPULANTE Y LA TAREA A REALIZAR
				 */

				enviar_mensaje(respuesta_tarea_tripulante, RESPUESTA_NUEVA_TAREA, conexion);
				// LE ENVIA A DISCORDIADOR LA TAREA A REALIZAR POR DICHO TRIPULANTE

				if(esquema_elegido == 'S') {

				}
				else if(esquema_elegido == 'P') {

				}

				free(tripulante_para_tarea);
				free(respuesta_tarea_tripulante);
				break;

			case EXPULSAR_TRIPULANTE:
				tripulante_a_eliminar = malloc(sizeof(t_tripulante));
				respuesta_tripulante_eliminado = malloc(sizeof(t_respuesta_tripulante));

				recibir_mensaje(tripulante_a_eliminar, operacion, conexion);

				printf("Tripulante a Expulsar: %u \n", tripulante_a_eliminar->id_tripulante);
				printf("Patota del tripulante: %u \n", tripulante_a_eliminar->id_patota);

				/*
				 * - BUSCAR TRIPULANTE EN LA MEMORIA (COINCIDIENDO POR ID_TRIPULANTE E ID_PATOTA)
				 * - LIBERAR EN LA MEMORIA EL SEGMENTO QUE HACE REFERENCIA A ESTE TRIPULANTE
				 * - ELIMINARLO DEL MAPA
				 * - ENVIAR UNA RESPUESTA DE CONFIRMACION AL DISCORDIADOR
				 */

				enviar_mensaje(respuesta_tripulante_eliminado, RESPUESTA_TRIPULANTE_ELIMINADO, conexion);
				// LE AVISA A DISCORDIADOR QUE EL TRIPULANTE FUE ELIMINADO EXITOSAMENTE ASI LO ELIMINA DE SUS ESTRUCTURAS

				if(esquema_elegido == 'S') {

				}
				else if(esquema_elegido == 'P') {

				}


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
				terminar_programa(config, logger);
				exit(0);

			default:
				log_warning(logger, "Operacion desconocida. No quieras meter la pata");
				break;
			}
}


bool validar_espacio_por_patota_segmentacion(uint32_t tamanio) {
	int32_t restante = memoria_compactada - tamanio;
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


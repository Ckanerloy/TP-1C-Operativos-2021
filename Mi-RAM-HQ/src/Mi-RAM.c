#include "Mi-RAM.h"

int main(void)
{
	memoria_principal = NULL;
	area_swap = NULL;
	config = crear_config(CONFIG_PATH);
	contador_id_tripu=1;
	contador_id_patota=1;
	ids = list_create();
	inicio = 0;
	contador_segmento = 0;

	crear_segmento_sem = malloc(sizeof(sem_t));
	sem_init(crear_segmento_sem, 0, 0);

	obtener_datos_de_config(config);
	logger = crear_log("mi-ram-hq.log", "Mi-RAM HQ");

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

	elegir_esquema_de_memoria(ESQUEMA_MEMORIA);
	criterio_elegido = elegir_criterio_seleccion(CRITERIO_SELECCION);
	algoritmo_elegido = elegir_algoritmo_reemplazo(ALGORITMO_REEMPLAZO);

	pthread_mutex_init(&mutexTablasDeSegmentos,NULL);

	iniciar_comunicacion();

	return EXIT_SUCCESS;
}



void obtener_datos_de_config(t_config* config)
{
	PUERTO = config_get_string_value(config, "PUERTO");
	TAMANIO_MEMORIA = config_get_int_value(config, "TAMANIO_MEMORIA");
	ESQUEMA_MEMORIA = config_get_string_value(config, "ESQUEMA_MEMORIA");
	TAMANIO_PAGINA = config_get_int_value(config, "TAMANIO_PAGINA");
	TAMANIO_SWAP = config_get_int_value(config, "TAMANIO_SWAP");
	PATH_SWAP = config_get_string_value(config, "PATH_SWAP");
	ALGORITMO_REEMPLAZO = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	CRITERIO_SELECCION = config_get_string_value(config, "CRITERIO_SELECCION");
}


void iniciar_comunicacion(){
	log_info(logger, "Servidor activo, esperando instrucciones ... \n");

	int32_t conexion_servidor = iniciar_servidor(IP, PUERTO);

	while(1) {

		int32_t* conexion_cliente = esperar_conexion(conexion_servidor);

		pthread_create(&hilo_recibir_mensajes, NULL,(void*)escuchar_conexion, conexion_cliente);

		pthread_detach(hilo_recibir_mensajes);

	}
}



void procesar_mensajes(codigo_operacion operacion, int32_t conexion)
{
	t_iniciar_patota* patota_recibida;
	int32_t tamanio_patota;
	t_queue* tareas_de_la_patota;
	t_respuesta_iniciar_patota* respuesta_iniciar_patota;

	t_id_tripulante* tripulante_a_eliminar;

	switch(operacion)
{
			case INICIAR_PATOTA:
				patota_recibida = malloc(sizeof(t_iniciar_patota));
				respuesta_iniciar_patota = malloc(sizeof(t_respuesta_iniciar_patota));

				recibir_mensaje(patota_recibida, operacion, conexion);

				t_tabla_segmentos_patota* tabla;


				printf("Cantidad de tripulantes: %d \n" , patota_recibida->cantidad_tripulantes);
				printf("Contenido de tareas: %s \n", patota_recibida->tareas_de_patota);
				printf("Posiciones de los tripulantes: %s \n", patota_recibida->posiciones);
				parser_posiciones = string_split(patota_recibida->posiciones, "|");

				char* ids_enviar = string_new();


				// Tareas de UNA Patota
				string_trim(&patota_recibida->tareas_de_patota);
				char** parser_tarea = obtener_tareas(patota_recibida->tareas_de_patota);

				uint32_t cant_tareas = cantidad_tareas(parser_tarea);

				tareas_de_la_patota = queue_create();

				for(uint32_t i=0; i<cant_tareas; i++){
					queue_push(tareas_de_la_patota, obtener_la_tarea(parser_tarea[i]));
				}

				tamanio_patota = sizeof(t_pcb) + (sizeof(t_tarea)* queue_size(tareas_de_la_patota)) + (sizeof(t_tcb) * patota_recibida->cantidad_tripulantes);


				printf("Tamanio de un PCB: %d\n", sizeof(t_pcb));
				printf("Tamanio de un TCB: %d\n", sizeof(t_tcb));
				printf("tarea %d\n", sizeof(t_tarea));

				// Verifica si hay espacio para guardar en memoria
				if(validar_espacio_por_patota(tamanio_patota) == 0) {
					respuesta_iniciar_patota->numero_de_patota = 0;
					respuesta_iniciar_patota->respuesta = 0;
					respuesta_iniciar_patota->tamanio_ids = 0;
					respuesta_iniciar_patota->ids_tripu = malloc(respuesta_iniciar_patota->tamanio_ids+1);
					strcpy(respuesta_iniciar_patota->ids_tripu, "");

				}
				else { // Hay suficiente espacio en memoria, puedo guardarlo y envio una respuesta a Discordiador

					t_pcb* nueva_patota = crear_pcb();

					if(esquema_elegido == 'S') {

						tabla = crear_tabla_segmentos(nueva_patota);


						t_segmento* segmento_patota = crear_segmento(nueva_patota, PATOTA);
						list_add(tabla->segmentos,segmento_patota);
						sem_wait(crear_segmento_sem);
						free(segmento_patota);


						t_segmento* segmento_tareas = crear_segmento(tareas_de_la_patota, TAREAS);
						list_add(tabla->segmentos,segmento_tareas);


						tabla->patota->tareas = segmento_tareas->inicio;

						// Mutex OFF

						sem_wait(crear_segmento_sem);

						free(segmento_tareas);

						for(int i=0;i<patota_recibida->cantidad_tripulantes;i++){

							///uint32_t direccion_pcb = segmento_patota->inicio;
							t_tcb* nuevo_tripulante = crear_tcb(0,atoi(parser_posiciones[i]),atoi(parser_posiciones[i+1]),0);


							t_segmento* segmento_tripulante = crear_segmento(nuevo_tripulante, TRIPULANTE);
							list_add(tabla->segmentos,segmento_tripulante);
							sem_wait(crear_segmento_sem);
							free(segmento_tripulante);

							string_append_with_format(&ids_enviar, "%u|", contador_id_tripu);
							contador_id_tripu++;
							free(nuevo_tripulante);
						}

						list_add(tablas_segmentos,tabla);



						// Tengo que buscar por la tabla de segmentos y encuentro cada tabla de patota
						//   y de acuerdo a esa tabla de patota, tengo que buscar en cada segmento y traducir esos bytes para
						// obtener los valores de cada estructura
						printf("PID: %u \n", tabla->patota->pid);
						printf("Direccion Tareas: %u\n\n", tabla->patota->tareas);

						free(crear_segmento_sem);

					}

					else if(esquema_elegido  == 'P') {
						//crear_pagina(estructura, tipo_estructura);
					}

					else {
						log_error(logger, "No se puede guardar la estructura en Memoria");
					}

					strcat(ids_enviar,"\0");

					respuesta_iniciar_patota->numero_de_patota = nueva_patota->pid;
					respuesta_iniciar_patota->respuesta = 1;
					respuesta_iniciar_patota->tamanio_ids = strlen(ids_enviar);
					respuesta_iniciar_patota->ids_tripu = malloc(respuesta_iniciar_patota->tamanio_ids+1);
					strcpy(respuesta_iniciar_patota->ids_tripu,ids_enviar);

					contador_id_patota++;
					free(nueva_patota);

				}



				printf("Tamanio Patota %d \n", tamanio_patota);
				printf("Tamanio Memoria Principal %d \n", TAMANIO_MEMORIA);
				printf("Memoria Restante: %d \n", memoria_restante);

				printf("Inicio de proximo segmento: %d \n", inicio);
				printf("Numero de proximo segmento: %d \n\n\n", contador_segmento);


				enviar_mensaje(respuesta_iniciar_patota, RESPUESTA_INICIAR_PATOTA, conexion);

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

				break;

			case PEDIDO_PROXIMA_TAREA:

				if(esquema_elegido == 'S') {

				}
				else if(esquema_elegido == 'P') {

				}
				// return queue_pop(tareas_de_la_patota);
				break;

			case EXPULSAR_TRIPULANTE:
				tripulante_a_eliminar = malloc(sizeof(t_id_tripulante));
				recibir_mensaje(tripulante_a_eliminar, operacion, conexion);

				printf("Tripulante a Expulsar: %u \n", tripulante_a_eliminar->id_tripulante);

				if(esquema_elegido == 'S') {

				}
				else if(esquema_elegido == 'P') {

				}
				// Eliminar el tripulante o hilo dentro de la memoria y del mapa

				free(tripulante_a_eliminar);
				break;

			case CERRAR_MODULO:
				cerrar_conexion(logger, conexion);

				printf("Terminando programa... \n");
				sleep(1);
				printf("-------------------------------------------------------------------------------------------------------------------------------------------------- \n");
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

int32_t validar_espacio_por_patota(uint32_t tamanio){
	int32_t restante = memoria_restante - tamanio;
	return (restante > 0);
}

t_pcb* crear_pcb(){
	t_pcb* proceso_patota =  malloc(sizeof(t_pcb));
	proceso_patota->pid = contador_id_patota;
	proceso_patota->tareas = 0; //Direccion de memoria de las tareas
	return proceso_patota;
}

t_tcb* crear_tcb(uint32_t dir_logica_pcb, uint32_t posicion_x, uint32_t posicion_y, uint32_t id_proxima_tarea){

	t_tcb* tripulante = malloc(sizeof(t_tcb));
	tripulante->id_tripulante = contador_id_tripu;
	tripulante->estado_tripulante = 'N';
	tripulante->posicion_x = posicion_x;
	tripulante->posicion_y = posicion_y;
	tripulante->id_proxima_instruccion = id_proxima_tarea;
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


void mostrar_tripulante(t_tcb* tripulante) {

	printf("Id tripulante: %u \n", tripulante->id_tripulante);
	printf("Estado tripulante: %c \n", tripulante->estado_tripulante);
	printf("Posicion X: %i \n", tripulante->posicion_x);
	printf("Posicion Y: %i \n", tripulante->posicion_y);
	printf("Id proxima instruccion a realizar: %i \n\n", tripulante->id_proxima_instruccion);
}


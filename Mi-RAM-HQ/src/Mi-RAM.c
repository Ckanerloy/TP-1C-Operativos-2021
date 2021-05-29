#include "Mi-RAM.h"

int main(void)
{
	MEMORIA_PRINCIPAL = NULL;
	AREA_SWAP = NULL;
	config = crear_config(CONFIG_PATH);

	obtener_datos_de_config(config);
	logger = crear_log("mi-ram-hq.log", "Mi-RAM HQ");

	printf("Tamanio de Memoria Principal: %u \n", TAMANIO_MEMORIA);
	printf("Equema de Memoria utilizado: %s \n", ESQUEMA_MEMORIA);
	printf("Tamanio de Pagina: %u \n", TAMANIO_PAGINA);
	printf("Tamanio Area Swap: %u \n", TAMANIO_SWAP);
	printf("Algoritmo de Reemplazo: %s \n", ALGORITMO_REEMPLAZO);
	printf("Criterio de Seleccion para meter en Segmentacion: %s \n", CRITERIO_SELECCION);

	MEMORIA_PRINCIPAL = malloc(TAMANIO_MEMORIA);
	if(MEMORIA_PRINCIPAL != NULL){
		printf("Memoria Principal iniciada... \n");
	}
	else {
		log_error(logger, "Error al iniciar la Memoria Principal.\n");
		sleep(1);
		abort();
	}

	AREA_SWAP = malloc(TAMANIO_SWAP);
	if(AREA_SWAP != NULL){
		printf("Area de Swap iniciada...\n");
	}
	else{
		log_error(logger, "Error al iniciar el Area de Swap.\n");
		sleep(1);
		abort();
	}


	//Iniciar tabla de segmentos de Patota
	//lista_de_patotas = malloc(sizeof(tablas_segmenos_patotas));
	//inicializar_lista_de_patotas(lista_de_patotas);



	iniciar_comunicacion();


	//espera = malloc(sizeof(sem_t));
	//sem_init(espera, 0, 1);
	/*sem_recibir = malloc(sizeof(sem_t));
	sem_init(sem_recibir, 0, 1);

	sem_enviar = malloc(sizeof(sem_t));
	sem_init(sem_enviar, 0, 0);*/

	/*int32_t conexion_servidor = iniciar_servidor(IP, PUERTO);
	log_info(logger, "Servidor activo, esperando instrucciones ... \n");
	while(1) {

		//sem_wait(sem_recibir);
		int32_t* conexion_cliente = esperar_conexion(conexion_servidor);

		pthread_create(&hilo_recibir_mensajes, NULL,(void*)escuchar_conexion, conexion_cliente);

		pthread_detach(hilo_recibir_mensajes);

		sem_wait(sem_enviar);

	}*/


	free(MEMORIA_PRINCIPAL);
	printf("Memoria Principal liberada...\n");
	free(AREA_SWAP);
	printf("Area de Swap liberada...\n");

	terminar_programa(config, logger);
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
	//t_tcb* tripulante_recibido;
	t_id_tripulante* tripulante_a_eliminar;
	t_tarea** tareas_tripulantes;
	t_respuesta* respuesta;
	//sem_post(espera);

	switch(operacion)
{
			case INICIAR_PATOTA:
				patota_recibida = malloc(sizeof(t_iniciar_patota));
				recibir_mensaje(patota_recibida, operacion, conexion);
				respuesta = malloc(sizeof(t_respuesta));

				printf("Cantidad de tripulantes: %d \n" , patota_recibida->cantidad_tripulantes);
				printf("Contenido de tareas: %s \n", patota_recibida->tareas_de_patota);
				printf("Posiciones de los tripulantes: %s \n", patota_recibida->posiciones);
				//printf("PCB PATOTA: %s \n", patota_recibida->pid_patota);

				string_trim(&patota_recibida->tareas_de_patota);
				char** parser_tarea = obtener_tareas(patota_recibida->tareas_de_patota);

				uint32_t cant_tareas = cantidad_tareas(parser_tarea);

				tareas_tripulantes = malloc(sizeof(t_tarea));
				for(uint32_t i=0; i<cant_tareas; i++){
					tareas_tripulantes[i] = obtener_la_tarea(parser_tarea[i]);

					printf("Operacion: %u \n", tareas_tripulantes[i]->operacion);
					printf(" - Cantidad: %u \n", tareas_tripulantes[i]->parametros->cantidad);
					printf(" - Posicion X: %u \n", tareas_tripulantes[i]->parametros->posicion_x);
					printf(" - Posicion Y:%u \n", tareas_tripulantes[i]->parametros->posicion_y);
					printf(" - Tiempo: %u \n\n", tareas_tripulantes[i]->parametros->tiempo);
				}


				// Crea el PCB de la Patota, ALMACENAR EN LA MEMORIA (SEGMENTACION O PAGINACION)
				respuesta->respuesta = 1;
				enviar_mensaje(respuesta, RESPUESTA_INICIAR_PATOTA, conexion);

				/*
				 * Cuando se crea el PCB de la patota, se mete su PID, y la direccion que apunta a las tareas previamente hechas
				 * y almacenadas en la Memoria.
				 */


/*				t_tarea** tareas_tripulantes = malloc(sizeof(t_tarea));
				tareas_tripulantes[0] = obtener_la_tarea(parser_tarea[1]);
				tareas_tripulantes[1] = obtener_la_tarea(parser_tarea[2]);


				printf("%u \n", tareas_tripulantes[0]->operacion);
				printf("%u \n", tareas_tripulantes[0]->parametros->cantidad);
				printf("%u \n", tareas_tripulantes[0]->parametros->posicion_x);
				printf("%u \n", tareas_tripulantes[0]->parametros->posicion_y);
				printf("%u \n", tareas_tripulantes[0]->parametros->tiempo);

				printf("%u \n", tareas_tripulantes[1]->operacion);
				printf("%u \n", tareas_tripulantes[1]->parametros->cantidad);
				printf("%u \n", tareas_tripulantes[1]->parametros->posicion_x);
				printf("%u \n", tareas_tripulantes[1]->parametros->posicion_y);
				printf("%u \n", tareas_tripulantes[1]->parametros->tiempo);


				int posicion = 0;
				while(parser_tarea[posicion] != NULL){
					string_trim(&parser_tarea[posicion]);
					string_append(&parser_tarea[posicion], ";");
					printf("%s \n", parser_tarea[posicion]);
					posicion++;
				}*/


	//			codigo_tarea codigo_tarea;
//				t_parametros_tarea* parametros_tarea;
				//obtener_operando(tareas, codigo_tarea, parametros_tarea);


//				tripulante_recibido = malloc(sizeof(t_tcb));
				//t_pcb* pcb_patota = crear_pcb();
			//	tripulante_recibido = crear_tcbs(pcb_patota, patota_recibida);

			//	enviar_mensaje(tripulante_recibido, INICIAR_TRIPULANTE, conexion);

				//free(tareas);

				free(parser_tarea);
				free(patota_recibida->tareas_de_patota);
				free(patota_recibida->posiciones);
				free(patota_recibida);
				break;

			case INICIAR_TRIPULANTE:
			//	tripulante_recibido = malloc(sizeof(t_tcb));
			//	recibir_mensaje(tripulante_recibido, operacion, conexion);

			//	mostrar_tripulante(tripulante_recibido);

			//	free(tripulante_recibido);
				break;

			case RECIBIR_UBICACION_TRIPULANTE:
				break;

			case ENVIAR_PROXIMA_TAREA:
				break;

			case EXPULSAR_TRIPULANTE:
				tripulante_a_eliminar = malloc(sizeof(t_id_tripulante));
				recibir_mensaje(tripulante_a_eliminar, operacion, conexion);

				printf("Tripulante a Expulsar: %u \n", tripulante_a_eliminar->id_tripulante);
				// Eliminar el tripulante o hilo dentro de la memoria y del mapa

				free(tripulante_a_eliminar);
				break;

			default:
				log_warning(logger, "Operacion desconocida. No quieras meter la pata");
				break;
			}
}


t_pcb* crear_pcb(void){
	t_pcb* proceso_patota =  malloc(sizeof(t_pcb));
	proceso_patota->pid = process_getpid();
	proceso_patota->tareas = 0; //Direccion de memoria de las tareas
	return proceso_patota;
}

t_tcb* crear_tcbs(t_pcb* pcb_patota, t_iniciar_patota* patota_recibida){
	uint32_t cantidad_tripulantes = patota_recibida->cantidad_tripulantes;

	char** parser_posiciones = string_split(patota_recibida->posiciones, "|");
	string_trim(parser_posiciones);

	int posicion = 0;
	//for(uint32_t c=0; c<cantidad_tripulantes; c++){
		int posicion_x = atoi(parser_posiciones[posicion]);
		int posicion_y = atoi(parser_posiciones[posicion+1]);

		return crear_tripulante(posicion_x, posicion_y, pcb_patota);

		//agregar_tripulante_mapa(tripulante);

		//posicion += 2;
	//}
}

t_tcb* crear_tripulante(int posicion_x, int posicion_y, t_pcb* pcb_patota){

	t_tcb* tripulante = malloc(sizeof(t_tcb));
	tripulante->tid = process_get_thread_id();
	tripulante->estado_tripulante = 'N';
	tripulante->posicion_x = posicion_x;
	tripulante->posicion_y = posicion_y;
	tripulante->id_proxima_instruccion = 0;
	tripulante->puntero_PCB = &pcb_patota;

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

	printf("Id tripulante: %u \n", tripulante->tid);
	printf("Estado tripulante: %c \n", tripulante->estado_tripulante);
	printf("Posicion X: %i \n", tripulante->posicion_x);
	printf("Posicion Y: %i \n", tripulante->posicion_y);
	printf("Id proxima instruccion a realizar: %i \n\n", tripulante->id_proxima_instruccion);
}


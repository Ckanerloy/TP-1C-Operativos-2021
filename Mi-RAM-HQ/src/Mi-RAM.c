#include "Mi-RAM.h"

int main(void)
{
	MEMORIA_PRINCIPAL = NULL;
	AREA_SWAP = NULL;
	config = crear_config(CONFIG_PATH);
	contador_id_tripu=1;
	contador_id_patota=1;
	ids=list_create();     //CREA LA LISTA DE IDS

	obtener_datos_de_config(config);
	logger = crear_log("mi-ram-hq.log", "Mi-RAM HQ");

	printf("Tamanio de Memoria Principal: %u \n", TAMANIO_MEMORIA);
	printf("Esquema de Memoria utilizado: %s \n", ESQUEMA_MEMORIA);
	printf("Tamanio de Pagina: %u \n", TAMANIO_PAGINA);
	printf("Tamanio Area Swap: %u \n", TAMANIO_SWAP);
	printf("Algoritmo de Reemplazo: %s \n", ALGORITMO_REEMPLAZO);
	printf("Criterio de Seleccion para meter en Segmentacion: %s \n", CRITERIO_SELECCION);

	MEMORIA_PRINCIPAL = malloc(TAMANIO_MEMORIA);
	MEMORIA_RESTANTE = TAMANIO_MEMORIA;

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

	// Elige el ESQUEMA DE MEMORIA elegido en el Config
	elegir_esquema_de_memoria(ESQUEMA_MEMORIA);


	//esto va a depender de si la configuracion es segmentacion o paginacion

	//t_segmentos_patota* segmento_patota = malloc(sizeof(t_segmentos_patota));
	//inicializar_tabla_segmentos_de_patota(segmento_patota);
	pthread_mutex_init(&mutexTablasDeSegmentos,NULL);

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

				printf("Cantidad de tripulantes: %d \n" , patota_recibida->cantidad_tripulantes);
				printf("Contenido de tareas: %s \n", patota_recibida->tareas_de_patota);
				printf("Posiciones de los tripulantes: %s \n", patota_recibida->posiciones);
				parser_posiciones = string_split(patota_recibida->posiciones, "|");

				char* ids_enviar = string_new();

				t_pcb* nueva_patota = crear_pcb();

				// Tareas de UNA Patota
				string_trim(&patota_recibida->tareas_de_patota);
				char** parser_tarea = obtener_tareas(patota_recibida->tareas_de_patota);

				uint32_t cant_tareas = cantidad_tareas(parser_tarea);

				tareas_de_la_patota = queue_create();

				for(uint32_t i=0; i<cant_tareas; i++){
					queue_push(tareas_de_la_patota, obtener_la_tarea(parser_tarea[i]));
				}


				for(uint32_t c=0; c<cant_tareas; c++) {
					t_tarea* tarea_ejemplo = malloc(sizeof(t_tarea));
					tarea_ejemplo = queue_pop(tareas_de_la_patota);

					printf("Operacion: %u \n", tarea_ejemplo->operacion);
					printf(" - Cantidad: %u \n", tarea_ejemplo->parametros->cantidad);
					printf(" - Posicion X: %u \n", tarea_ejemplo->parametros->posicion_x);
					printf(" - Posicion Y:%u \n", tarea_ejemplo->parametros->posicion_y);
					printf(" - Tiempo: %u \n\n", tarea_ejemplo->parametros->tiempo);

					free(tarea_ejemplo);
				}

				tamanio_patota = sizeof(nueva_patota) + queue_size(tareas_de_la_patota);


				//calcular direccion logica de la misma
				for(int i=0;i<patota_recibida->cantidad_tripulantes;i++)
				{
					t_tcb* nuevo_tripulante = crear_tcb(0,atoi(parser_posiciones[i]),atoi(parser_posiciones[i+1]),0);

					tamanio_patota += sizeof(nuevo_tripulante);

					string_append_with_format(&ids_enviar, "%u|", contador_id_tripu);
					contador_id_tripu++;
					free(nuevo_tripulante);
				}
				strcat(ids_enviar,"\0");

				printf("Tamanio patota %d \n", tamanio_patota);
				printf("Tamanio memoria Principal %d \n\n", TAMANIO_MEMORIA);

				respuesta_iniciar_patota->numero_de_patota = nueva_patota->pid;


				// Verifica si hay espacio para guardar en memoria
				if(validar_espacio_por_patota(tamanio_patota) == 0) {
					respuesta_iniciar_patota->respuesta = 0;
					respuesta_iniciar_patota->tamanio_ids = 0;
					respuesta_iniciar_patota->ids_tripu = "";

					MEMORIA_RESTANTE += tamanio_patota;
				}
				else{ // Hay suficiente espacio en memoria, puedo guardarlo y envio una respuesta a Discordiador
					t_tabla_segmentos_patota* tabla = crear_tabla_segmentos(nueva_patota);
					//int resultado_guardado = guardar_patota();
					respuesta_iniciar_patota->respuesta = 1;
					respuesta_iniciar_patota->tamanio_ids = strlen(ids_enviar);
					respuesta_iniciar_patota->ids_tripu = malloc(respuesta_iniciar_patota->tamanio_ids+1);
					strcpy(respuesta_iniciar_patota->ids_tripu,ids_enviar);

					contador_id_patota++;
				}
				enviar_mensaje(respuesta_iniciar_patota, RESPUESTA_INICIAR_PATOTA, conexion);
				free(ids_enviar);
				free(nueva_patota);
				free(parser_tarea);
				free(tareas_de_la_patota);
				free(respuesta_iniciar_patota->ids_tripu);
				free(respuesta_iniciar_patota);
				free(patota_recibida->tareas_de_patota);
				free(patota_recibida->posiciones);
				free(patota_recibida);
				break;

				//guardar_patota();

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

int32_t validar_espacio_por_patota(uint32_t tamanio){
	int32_t restante = MEMORIA_RESTANTE - tamanio;
	return (restante > 0);
}

t_pcb* crear_pcb(){
	t_pcb* proceso_patota =  malloc(sizeof(t_pcb));
	proceso_patota->pid = contador_id_patota;
	proceso_patota->tareas = 0; //Direccion de memoria de las tareas
	return proceso_patota;
}

t_tcb* crear_tcb(uint32_t dir_logica_pcb, uint32_t posicion_x,uint32_t posicion_y,uint32_t dir_logica_prox_instruc){

	t_tcb* tripulante = malloc(sizeof(t_tcb));
	tripulante->id_tripulante = contador_id_tripu;
	tripulante->estado_tripulante = 'N';
	tripulante->posicion_x = posicion_x;
	tripulante->posicion_y = posicion_y;
	tripulante->id_proxima_instruccion = dir_logica_prox_instruc;
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


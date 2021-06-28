#include "Discordiador.h"

int main(void) {

	logger = crear_log("discordiador.log", "DISCORDIADOR");
	config = crear_config(CONFIG_PATH);
	obtener_datos_de_config(config);
	obtener_planificacion_de_config(config);

	inicializar_semaforos();
	inicializar_listas();
	iniciar_planificacion();

	crear_hilos();

	while(1){
	//DISCORDIADOR COMO SERVIDOR DE MONGO-STORE
	// Conexion de escucha con MongoStore?  POR SABOTAJE
	//sem_wait(sabotaje);
	//pthread_create(&hilo_sabotaje, NULL, (void*)iniciar_escucha_sabotaje, NULL);

	//pthread_detach(hilo_sabotaje);
	}
	return EXIT_SUCCESS;
}

void inicializar_listas(){
	lista_semaforos_tripulantes=list_create();

	lista_tripulantes=list_create();
}

void inicializar_semaforos() {
	mutex_sabotaje = malloc(sizeof(sem_t));
	sem_init(mutex_sabotaje, 0, 1);

	comando_para_ejecutar = malloc(sizeof(sem_t));
	sem_init(comando_para_ejecutar, 0, 1);

	termino_operacion = malloc(sizeof(sem_t));
	sem_init(termino_operacion, 0, 1);
}


void finalizar_semaforos() {
	//free(sabotaje);
	free(termino_operacion);
	free(comando_para_ejecutar);
}


void crear_hilos(){
	pthread_create(&hilo_consola, NULL,(void*)iniciar_escucha_por_consola, NULL);
	pthread_detach(hilo_consola);

	//pthread_create(&hilo_sabotaje, NULL,(void*)iniciar_escucha_sabotaje, NULL);
	//pthread_detach(hilo_sabotaje);


	pthread_create(&hilo_new_ready, NULL,(void*)new_ready, NULL);
	pthread_detach(hilo_new_ready);

	pthread_create(&hilo_ready_running, NULL,(void*)ready_running, NULL);
	pthread_detach(hilo_ready_running);

}

void iniciar_escucha_sabotaje(void){

	//conexion_sabotaje = iniciar_servidor(IP_MONGO_STORE, PUERTO_MONGO_STORE);
	//if(resultado_conexion(conexion_sabotaje, logger, "i-mongo") == -1){
	//	exit(-1);
	//}
	/*
	t_respuesta_mongo* respuesta;
	while(1){
		//recibir_mensaje(respuesta, RECIBIR_SABOTAJE, conexion_sabotaje);
		sem_wait(mutex_sabotaje);
		valor_sabotaje = 1;
		sem_post(mutex_sabotaje);
		//filesystem esperar señal server espeando posible sabotaje

		//la señal te llega  filesystem tiene ip y puerto de disc
		int largo;



		tripulante_plani* tripu_mas_cercano = malloc(sizeof(tripulante_plani));

		tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));



		largo = list_size(lista_tripulantes);
		for(int i=0;i<largo;i++){
			tripulante = list_get(lista_tripulantes,i);
			if(tripulante->estado == 'E'){
				//sem_wait(tripulante->sem_tripu);
				list_add_sorted(bloqueado_suspendido, (void*) tripulante,(void*)menorId);
			}
		}

		list_map(bloqueado_suspendido, (void*) poner_en_cero_semaforos);

		for(int i=0;i<largo;i++){
			tripulante = list_get(lista_tripulantes,i);
			if(tripulante->estado == 'R'){
				list_add_sorted(bloqueado_suspendido_ready, (void*) tripulante,(void*)menorId);
			}
		}

		list_iterate(bloqueado_suspendido_ready, (void*) poner_en_uno_semaforos);

		list_add_all(bloqueado_suspendido,bloqueado_suspendido_ready);

		tripu_mas_cercano = list_fold1(bloqueado_suspendido, (void*) mas_cercano);

		tripulante_sabotaje* tripu_sabotaje = malloc(sizeof(tripulante_plani));



		tripu_sabotaje->id_tripulante=tripu_mas_cercano->id_tripulante;
		tripu_sabotaje->id_patota=tripu_mas_cercano->numero_patota;
		tripu_sabotaje->posicion_sabotaje=respuesta->posicion_sabotaje;
	//	tripu_sabotaje->posicion_inicial=

		tripu_mas_cercano->estado='E';

		pthread_create(&hilo_tripulante_sabo,NULL,(void*)hilo_tripulante_sabotaje,tripu_sabotaje);
		pthread_detach(hilo_tripulante_sabo);






		//FINAL
		sem_wait(mutex_sabotaje);
		valor_sabotaje=0;
		sem_post(mutex_sabotaje);
	}
	*/
}



tripulante_plani* mas_cercano(tripulante_plani* tripulante1,tripulante_plani* tripulante2){
	//tenemos variable global q dice la posicion del sabotaje
	posiciones* posicion_tripu1;
	posicion_tripu1 = malloc(sizeof(posiciones));
	posiciones* posicion_tripu2;
	posicion_tripu2 = malloc(sizeof(posiciones));
	obtener_distancia(posicion_tripu1, posicion_tripu2);
}



bool menorId(tripulante_plani* tripulante1,tripulante_plani* tripulante2){
	return tripulante1->id_tripulante<tripulante2->id_tripulante;
}


void iniciar_escucha_por_consola(){

	while(1){
		sem_wait(comando_para_ejecutar);
		obtener_orden_input();
	}

}

void obtener_datos_de_config(t_config* config) {

	IP_MI_RAM = config_get_string_value(config, "IP_MI_RAM_HQ");
	PUERTO_MI_RAM = config_get_string_value(config, "PUERTO_MI_RAM_HQ");

	IP_MONGO_STORE = config_get_string_value(config, "IP_I_MONGO_STORE");
	PUERTO_MONGO_STORE = config_get_string_value(config, "PUERTO_I_MONGO_STORE");

	DURACION_SABOTAJE = config_get_int_value(config, "DURACION_SABOTAJE");
	RETARDO_CICLO_CPU = config_get_int_value(config, "RETARDO_CICLO_CPU");

}

void obtener_orden_input(){
	 char* cadena_ingresada = NULL;
	 size_t longitud = 0;

	 t_respuesta_iniciar_patota* respuesta_iniciar_patota;

	 sem_wait(termino_operacion);

	 printf("Inserte orden:\n");

	 getline(&cadena_ingresada, &longitud, stdin);

	 string_trim(&cadena_ingresada);

	 if(strcmp(cadena_ingresada, "") == 0)
	 {
			printf("No se ha ingresado ninguna orden.\n");
			free(cadena_ingresada);
			sem_post(comando_para_ejecutar);
			return;
	 }

	 parser_consola = string_split(cadena_ingresada, " ");

	 char* comando_ingresado = malloc(strlen(parser_consola[0])+1);

	 strcpy(comando_ingresado, parser_consola[0]);

	 sem_post(comando_para_ejecutar);
	 operacion = mapeo_valor_consola(comando_ingresado);
	 free(comando_ingresado);

	 int32_t valor_semaforo;
	// tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));
	 //tripulante_plani* tripulante_a_ready = malloc(sizeof(tripulante_plani));

	 //int32_t valor_semaforo;
	 tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));
	 int largo;
	 int recorrido;

	 tripulante_plani* tripulante_a_expulsar = malloc(sizeof(tripulante_plani));
	 t_tripulante* id_tripulante_a_expulsar = malloc(sizeof(t_tripulante));
	 switch(operacion){


		case INICIAR_PLANIFICACION:

			// ARRANCA LA PLANIFICACION DE LOS TRIPULANTES (BUSCANDO EL ALGORITMO QUE ESTA EN CONFIG)
	//		sem_getvalue(planificacion_on,&valor_semaforo);

		//	if(valor_semaforo == 0){
				printf("Iniciando Planificacion....... \n");
			//}

			elegir_algoritmo(conexion_mi_ram);

			//list_map(lista_semaforos_tripulantes, (void*) poner_en_uno_semaforos);
			list_iterate(lista_semaforos_tripulantes, (void*) poner_en_uno_semaforos);
			sem_post(planificacion_on);
			sem_post(planificacion_on_ready_running);

			break;

		case PAUSAR_PLANIFICACION:


			//list_map(lista_semaforos_tripulantes, (void*) poner_en_cero_semaforos);
			list_iterate(lista_semaforos_tripulantes, (void*) poner_en_cero_semaforos);

			sem_wait(planificacion_on);
			sem_wait(planificacion_on_ready_running);

			break;

		case INICIAR_PATOTA:
			// Ej: INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4 1|1
			// Ej: INICIAR_PATOTA 2 /home/utnso/tareas/tareasPatota5.txt 5|5 5|5
			// LISTAR_TRIPULANTES
			if(parser_consola[1] == NULL || parser_consola[2] == NULL){
				log_error(logger, "Faltan argumentos. Debe iniciarse de la forma: INICIAR_PATOTA <CantidadTripulantes> >Ubicación txt Tareas>.");
				break;
			}

			uint32_t cantidad_argumentos = cantidad_argumentos_ingresados(parser_consola);
			uint32_t cantidad_posiciones = cantidad_argumentos - 3;
			int cantidad_tripulantes = atoi(parser_consola[1]);
			int posiciones_faltantes = cantidad_tripulantes - cantidad_posiciones;
			respuesta_iniciar_patota = malloc(sizeof(t_respuesta_iniciar_patota));

			if(posiciones_faltantes < 0) {
				log_error(logger, "Se ingresaron posiciones demás. Solo puede como máximo haber tantas posiciones como cantidad de tripulantes.\n");
				break;
			}


			strcat(parser_consola[1],"\0");
			strcat(parser_consola[2],"\0");

			// Indica que arranca a leer a partir de los primeros 3 argumentos: COMANDO CANTIDAD_TRIPULANTES ARCHIVO_TAREAS
			int ubicacion_parser = 3;
			// Meter todas las posiciones de los tripulantes en una sola cadena
			char* posiciones = string_new();
			while(parser_consola[ubicacion_parser] != NULL && ubicacion_parser < cantidad_argumentos) {
				string_append_with_format(&posiciones, "%s|", parser_consola[ubicacion_parser]);
				ubicacion_parser++;
			}

			for(int i = 0; i<posiciones_faltantes; i++) {
				string_append_with_format(&posiciones, "%s|", "0|0");
			}
			strcat(posiciones, "\0");


			// Conexión con Mi-RAM HQ
			conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);
			if(resultado_conexion(conexion_mi_ram, logger, "Mi-RAM HQ") == -1){
				break;
			}


			// Obtiene el contenido del Archivo de Tareas de la Patota
			FILE* archivo_tareas = fopen(parser_consola[2], "r");

				if(archivo_tareas > 0) {
					printf("El archivo existe en %s.\n", parser_consola[2]);
				}
				else {
					log_error(logger, "El archivo %s no existe. \n", parser_consola[2]);
					break;
				}

			fseek(archivo_tareas, 0, SEEK_END);
			long int tamanio_archivo = ftell(archivo_tareas);
			rewind(archivo_tareas);

			char* tareas_totales = malloc(sizeof(char)*tamanio_archivo);

			fread(tareas_totales, tamanio_archivo+1, 1, archivo_tareas);
			tareas_totales = string_substring_until(tareas_totales, tamanio_archivo);
			strcat(tareas_totales, "\0");


			// Creo la estructura de datos de la Patota
			t_iniciar_patota* mensaje_patota = malloc(sizeof(t_iniciar_patota));
			mensaje_patota->cantidad_tripulantes = atoi(parser_consola[1]);

			mensaje_patota->tamanio_tareas = strlen(tareas_totales);
			mensaje_patota->tareas_de_patota = malloc(mensaje_patota->tamanio_tareas+1);
			strcpy(mensaje_patota->tareas_de_patota, tareas_totales);

			mensaje_patota->tamanio_posiciones = strlen(posiciones);
			mensaje_patota->posiciones = malloc(mensaje_patota->tamanio_posiciones+1);
			strcpy(mensaje_patota->posiciones, posiciones);
			//mensaje_patota->pid_patota = pcb_patota->pid;

			enviar_mensaje(mensaje_patota, INICIAR_PATOTA, conexion_mi_ram);

			if(validacion_envio(conexion_mi_ram) == 1) {
				recibir_mensaje(respuesta_iniciar_patota, RESPUESTA_INICIAR_PATOTA, conexion_mi_ram);

				if(respuesta_iniciar_patota->respuesta == 1){
					printf("La respuesta fue positiva. \n");
					printf("los id de los tripulantes %s\n",respuesta_iniciar_patota->ids_tripu);
					printf("Numero de patota: %u \n", respuesta_iniciar_patota->numero_de_patota);

					char** parser_ids = string_split(respuesta_iniciar_patota->ids_tripu, "|");

					for(int i=0; mensaje_patota->cantidad_tripulantes>i; i++) {

						tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));
						tripulante->id_tripulante = atoi(parser_ids[i]);
						tripulante->numero_patota = respuesta_iniciar_patota->numero_de_patota; //esto lo devuelve mi ram
						tripulante->estado = 'N';
						tripulante->expulsado=0;

						sem_t* sem_plani=malloc(sizeof(sem_t));
						sem_init(sem_plani,0,0);

						tripulante->sem_planificacion = sem_plani;

						sem_t* semaforo_exec = malloc(sizeof(sem_t));
						sem_init(semaforo_exec, 0, 0);

						tripulante->sem_tripu = semaforo_exec;

						pthread_create(&hilo_tripulante,NULL,(void*)tripulante_hilo,tripulante);
						pthread_detach(hilo_tripulante);

						list_add(lista_semaforos_tripulantes, tripulante->sem_tripu);
						list_add(lista_tripulantes,tripulante);

						sem_wait(mutex_new);
						queue_push(cola_new,tripulante);
						sem_post(mutex_new);


						sem_post(contador_tripulantes_en_new);
					}

				}
				else {
					printf("La respuesta fue negativa. \n");			// Salgo del Switch, ya que no pudo crearse la Patota en Mi-RAM HQ
					break;
				}
			}


			// Libero la memoria usada
			fclose(archivo_tareas);
			free(tareas_totales);
			free(respuesta_iniciar_patota);

			free(posiciones);
			free(parser_posiciones);

			free(mensaje_patota->tareas_de_patota);
			free(mensaje_patota->posiciones);
			free(mensaje_patota);
			cerrar_conexion(logger, conexion_mi_ram);
			break;


		case LISTAR_TRIPULANTES:

			if(parser_consola[1] != NULL) {
				log_warning(logger, "Sobran argumentos. Debe iniciarse de la forma LISTAR_TRIPULANTES.");
				break;

			}


			largo=list_size(lista_tripulantes);


			listar_tripulantes();

			for(recorrido=0;recorrido<largo;recorrido++){
				tripulante=list_get(lista_tripulantes, recorrido);
				printf("Tripulante: %u           Patota: %u      Status: %c \n",tripulante->id_tripulante,tripulante->numero_patota,tripulante->estado);

			}


			break;

		case OBTENER_BITACORA:

			if(parser_consola[1] == NULL) {
				log_error(logger, "Faltan argumentos. Debe inciarse de la forma OBTENER_BITACORA <Id_Tripulante>.");
				break;
			}
			strcat(parser_consola[1], "\0");

			t_tripulante* id_tripulante_x_bitacora = malloc(sizeof(t_tripulante));
			id_tripulante_x_bitacora->id_tripulante = atoi(parser_consola[1]);

			conexion_mongo_store = crear_conexion(IP_MONGO_STORE, PUERTO_MONGO_STORE);

			if(conexion_mongo_store < 0) {
				log_error(logger, "No se pudo conectar. \n");
				break;
			}

			enviar_mensaje(id_tripulante_x_bitacora, OBTENER_BITACORA, conexion_mongo_store);

			free(id_tripulante_x_bitacora);
			cerrar_conexion(logger,conexion_mongo_store);
			break;

		case EXPULSAR_TRIPULANTE:
			// ELIMINA EL TRIPULANTE DEL HILO DEL DISCORDIADOR

			// CONECTA CON MI RAM HQ Y LO ELIMINA DE MEMORIA
			// 		- TAMBIEN LO ELIMINA DEL MAPA

			if(parser_consola[1] == NULL) {
			   	log_error(logger, "Faltan argumentos. Debe inciarse de la forma EXPULSAR_TRIPULANTE <Id_Tripulante>.");
			  	break;
			}
			strcat(parser_consola[1], "\0");

			id_tripulante_a_expulsar->id_tripulante = atoi(parser_consola[1]);

			bool mismo_id(tripulante_plani* tripu)
			{
				return tripu->id_tripulante == id_tripulante_a_expulsar->id_tripulante;
			}

			tripulante_a_expulsar = list_find(lista_tripulantes, (void*)mismo_id);

			printf("id del tripu a eliminar: %u \n",id_tripulante_a_expulsar->id_tripulante);
			printf("id del patota a eliminar: %u \n",id_tripulante_a_expulsar->id_patota);

			if(tripulante_a_expulsar != NULL){

				id_tripulante_a_expulsar->id_patota = tripulante_a_expulsar->numero_patota;
				switch(tripulante_a_expulsar->estado){
					case 'R':
							tripulante_a_expulsar->expulsado = 1;
							sem_post(tripulante_a_expulsar->sem_planificacion);
							ready_exit(tripulante_a_expulsar);
							break;

					case 'E':
							tripulante_a_expulsar->expulsado = 1;
							running_exit(tripulante_a_expulsar);
							break;

					case 'B':
						tripulante_a_expulsar->expulsado = 1;
						block_exit(tripulante_a_expulsar);
						break;
				}



				conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);
				if(resultado_conexion(conexion_mi_ram, logger, "Mi-RAM HQ") == -1){
					break;
				}

				//enviar_mensaje(id_tripulante_a_expulsar, EXPULSAR_TRIPULANTE, conexion_mi_ram);

				cerrar_conexion(logger,conexion_mi_ram);
			}else{
				log_error(logger, "No existe el tripulante que se desea eliminar");
			}

			free(id_tripulante_a_expulsar);
			free(tripulante_a_expulsar);
			break;

		case TERMINAR_PROGRAMA:

			conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);
			if(resultado_conexion(conexion_mi_ram, logger, "Mi-RAM HQ") != -1){
				enviar_mensaje("", CERRAR_MODULO, conexion_mi_ram);
				cerrar_conexion(logger, conexion_mi_ram);
			}
			/*conexion_mongo_store = crear_conexion(IP_MONGO_STORE, PUERTO_MONGO_STORE);
	  		if(resultado_conexion(conexion_mongo_store, logger, "Mongo Store") != -1) {
	  			enviar_mensaje("", CERRAR_MODULO, conexion_mongo_store);
	 			cerrar_conexion(logger, conexion_mongo_store);
	 			}*/



			printf("Terminando programa... \n");
			sleep(1);
			printf("-------------------------------------------------------------------------------------------------------------------------------------------------- \n");
			// Libero memoria
			free(parser_consola);
			free(cadena_ingresada);
			finalizar_semaforos();
			finalizar_semaforos_plani();
			terminar_programa(config, logger);
			exit(0);


		default:
			printf("No se reconoce ese comando. Por favor, ingrese un comando válido.\n");
			break;
		}

	sem_post(termino_operacion);

	free(parser_consola);
	free(cadena_ingresada);

}

void arreglar_sabotaje() {
	// MANDA TRIPULANTE MAS CERCANO A LA UBICACION DEL SABOTAJE PARA QUE LO SOLUCIONE
}


void poner_en_cero_semaforos(sem_t* semaforo){

    sem_wait(semaforo);
	//return semaforo;
	/*int valor;
	sem_getvalue(semaforo,&valor);
	printf("aca");
	if(valor==1){
		sem_wait(semaforo);
	}else{
		pthread_create(&hilo_solucion,NULL,(void*)esperadorDeUno,semaforo);
		pthread_detach(hilo_solucion);
	}
	*/

}

void poner_en_uno_semaforos(sem_t* semaforo){
	sem_post(semaforo);
}



void esperadorDeUno(sem_t* semaforo){
	int valor;
	sem_getvalue(semaforo,&valor);
	printf("hola");
	//while(valor!=1);
	//sem_wait(semaforo);
}


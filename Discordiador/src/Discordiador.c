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

void inicializar_semaforos(){
	sabotaje = malloc(sizeof(sem_t));
	sem_init(sabotaje, 0, 1);

	comando_para_ejecutar = malloc(sizeof(sem_t));
	sem_init(comando_para_ejecutar, 0, 1);

	termino_operacion = malloc(sizeof(sem_t));
	sem_init(termino_operacion, 0, 1);
}


void finalizar_semaforos() {
	free(sabotaje);
	free(termino_operacion);
	free(comando_para_ejecutar);
}


void crear_hilos(){
	pthread_create(&hilo_consola, NULL,(void*)iniciar_escucha_por_consola, NULL);
	pthread_detach(hilo_consola);

	pthread_create(&hilo_new_ready, NULL,(void*)new_ready, NULL);
	pthread_detach(hilo_new_ready);

	pthread_create(&hilo_ready_running, NULL,(void*)ready_running, NULL);
	pthread_detach(hilo_ready_running);

}

//void iniciar_escucha_sabotaje(void){
//	while(1){
//		obtener_orden_sabotaje();
//	}
//}

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

	 t_respuesta* respuesta;

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
	 tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));
	 //tripulante_plani* tripulante_a_ready = malloc(sizeof(tripulante_plani));
	 switch(operacion){


	// Arranca todo pausado

		case INICIAR_PLANIFICACION:

			// ARRANCA LA PLANIFICACION DE LOS TRIPULANTES (BUSCANDO EL ALGORITMO QUE ESTA EN CONFIG)
	//		sem_getvalue(planificacion_on,&valor_semaforo);

		//	if(valor_semaforo == 0){
				printf("Iniciando Planificacion....... \n");
			//}


			lista_semaforos_tripulantes = list_map(lista_semaforos_tripulantes, (void*) poner_en_uno_semaforos);
			sem_post(planificacion_on);
			sem_post(planificacion_on_ready_running);

			// LOS TRIPULANTES ESTAN DEFINIDOS POR HILO -> CADA HILO IRIA A UNA COLA
			// PONE A TODOS LOS TRIPULANTES EN EL ESTADO EXECUTE

			elegir_algoritmo();


			break;

		case PAUSAR_PLANIFICACION:



			lista_semaforos_tripulantes = list_map(lista_semaforos_tripulantes, (void*) poner_en_cero_semaforos);
			sem_wait(planificacion_on);
			sem_wait(planificacion_on_ready_running);
			//A
			// PAUSA LA PLANIFICACION DE LOS TRIPULANTES, ES DECIR TODOS EN PAUSA? o se mete algun WAIT(signal)
			// PONE A TODOS LOS TRIPULANTES EN EL ESTADO BLOCK I/O
			break;

		case INICIAR_PATOTA:
			// Ej: INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4 1|1
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/tareasPatota5.txt 5|5 5|5 5|5
			// LISTAR_TRIPULANTES
			if(parser_consola[1] == NULL || parser_consola[2] == NULL){
				log_error(logger, "Faltan argumentos. Debe iniciarse de la forma: INICIAR_PATOTA <CantidadTripulantes> >Ubicación txt Tareas>.");
				break;
			}

			uint32_t cantidad_argumentos = cantidad_argumentos_ingresados(parser_consola);
			uint32_t cantidad_posiciones = cantidad_argumentos - 3;
			int cantidad_tripulantes = atoi(parser_consola[1]);
			int posiciones_faltantes = cantidad_tripulantes - cantidad_posiciones;
			respuesta = malloc(sizeof(t_respuesta));

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
				recibir_mensaje(respuesta, RESPUESTA_INICIAR_PATOTA, conexion_mi_ram);
				if(respuesta->respuesta == 1){
					printf("La respuesta fue positiva. \n");
					printf("los id de los tripulantes %s\n",respuesta->ids_tripu);
					char** parser_ids= string_split(respuesta->ids_tripu, "|");
					for(int i=0;mensaje_patota->cantidad_tripulantes>i;i++){
						tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));
						tripulante->id_tripulante = atoi(parser_ids[i]);
						tripulante->numero_patota = 5;
						tripulante->estado='N';//A

						sem_t* sem_plani=malloc(sizeof(sem_t));
						sem_init(sem_plani,0,0);

						tripulante->sem_planificacion= sem_plani;

						sem_t* semaforo_exec= malloc(sizeof(sem_t));
						sem_init(semaforo_exec, 0, 0);

						tripulante->sem_tripu= semaforo_exec;

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
			free(respuesta);

			free(posiciones);
			free(parser_posiciones);

			free(mensaje_patota->tareas_de_patota);
			free(mensaje_patota->posiciones);
			free(mensaje_patota);
			cerrar_conexion(logger, conexion_mi_ram);
			break;

			/*
			 * COSAS QUE FALTAN:
			 * 	-Mi-Ram debe devolvernos el numero de patota cuando devuelve el numero de tripulante A
			 * 	-Agregar las cosas a la serializacion
			 *
			 *
			*/


		case LISTAR_TRIPULANTES:

			if(parser_consola[1] != NULL) {
				log_warning(logger, "Sobran argumentos. Debe iniciarse de la forma LISTAR_TRIPULANTES.");
				break;

			}


			int largo=list_size(lista_tripulantes);
			printf("largo de la lista %d",largo);
			int recorrido;

			tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));
			for(recorrido=0;recorrido<largo;recorrido++){
				tripulante=list_get(lista_tripulantes, recorrido);
				printf("el tripulante %u esta en el estado %c \n ",tripulante->id_tripulante,tripulante->estado);

			}

			//while(queue_size(cola_exit)>0){
			//	tripulante_plani* tripulante_a_ready = queue_pop(cola_exit);
			//	printf("id tripulante (COLA exit): %u \n",tripulante_a_ready->id_tripulante);
			//}

			printf("-----------------\n");

			listar_tripulantes();
			break;

		case OBTENER_BITACORA:

			if(parser_consola[1] == NULL) {
				log_error(logger, "Faltan argumentos. Debe inciarse de la forma OBTENER_BITACORA <Id_Tripulante>.");
				break;
			}
			strcat(parser_consola[1], "\0");

			t_id_tripulante* id_tripulante_x_bitacora = malloc(sizeof(t_id_tripulante));
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

			t_id_tripulante* id_tripulante_a_expulsar = malloc(sizeof(t_id_tripulante));
			id_tripulante_a_expulsar->id_tripulante = atoi(parser_consola[1]);

			conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);
			if(resultado_conexion(conexion_mi_ram, logger, "Mi-RAM HQ") == -1){
				break;
			}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// ENVIA EL ID DEL TRIPULANTE A EXPULSAR: LO ELIMINA DEL MAPA, LO ELIMINA DE LA MEMORIA
			// Y TAMBIEN HAY QUE ELIMINARLO DE LA LISTA QUE GUARDA DISCORDIADOR

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


			enviar_mensaje(id_tripulante_a_expulsar, EXPULSAR_TRIPULANTE, conexion_mi_ram);

			free(id_tripulante_a_expulsar);
			cerrar_conexion(logger,conexion_mi_ram);
			break;

		case TERMINAR_PROGRAMA:



		//	printf("%d",prueba);
		//	printf("%d",queue_size(cola_exit));

			/*printf("Terminando programa... \n");
			sleep(1);
			printf("-------------------------------------------------------------------------------------------------------------------------------------------------- \n");
			// Libero memoria
			free(parser_consola);
			free(cadena_ingresada);
			finalizar_semaforos();
			terminar_programa(config, logger);
			breack
			exit(0);
*/
			break;
		default:
			printf("No se reconoce ese comando. Por favor, ingrese un comando válido.\n");
			break;
		}

	sem_post(termino_operacion);



	free(parser_consola);
	free(cadena_ingresada);

}


/*
t_pcb* crear_pcb(void){
	t_pcb* proceso_patota =  malloc(sizeof(t_pcb));
	proceso_patota->pid = process_getpid();
	proceso_patota->tareas = 0; //Hay que buscar que es la direccion logica del archivo de tareas
	return proceso_patota;
}*/


void obtener_orden_sabotaje(void){

	//conexion_mongo_store = crear_conexion(IP_MONGO_STORE,PUERTO_MONGO_STORE);
	conexion_sabotaje = iniciar_servidor(IP_MONGO_STORE, PUERTO_MONGO_STORE);
	// SABOTAJE -> BLOQUEAR A TODOS LOS TRIPULANTES
}


void arreglar_sabotaje() {
	// MANDA TRIPULANTE MAS CERCANO A LA UBICACION DEL SABOTAJE PARA QUE LO SOLUCIONE
}


void poner_en_cero_semaforos(sem_t* semaforo){
	int valor;
	sem_getvalue(semaforo,&valor);
	printf("aca");
	if(valor==1){
		sem_wait(semaforo);
	}else{
		pthread_create(&hilo_solucion,NULL,(void*)esperadorDeUno,semaforo);
		pthread_detach(hilo_solucion);
	}


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



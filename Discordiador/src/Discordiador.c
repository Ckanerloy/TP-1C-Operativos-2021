#include "Discordiador.h"

int main(void) {

	logger = crear_log("discordiador.log", "DISCORDIADOR");
	config = crear_config(CONFIG_PATH);
	obtener_datos_de_config(config);
	obtener_planificacion_de_config(config);
	sabotaje = malloc(sizeof(sem_t));
	sem_init(sabotaje, 0, 1);

	comando_para_ejecutar = malloc(sizeof(sem_t));
	sem_init(comando_para_ejecutar, 0, 1);

	termino_operacion = malloc(sizeof(sem_t));
	sem_init(termino_operacion, 0, 1);

	while(1){
	//DISCORDIADOR COMO SERVIDOR DE MONGO-STORE
	// Conexion de escucha con MongoStore?  POR SABOTAJE
	//sem_wait(sabotaje);
	//pthread_create(&hilo_sabotaje, NULL, (void*)iniciar_escucha_sabotaje, NULL);
	sem_wait(comando_para_ejecutar);

	pthread_create(&hilo_consola, NULL,(void*)obtener_orden_input, NULL);


	//pthread_detach(hilo_sabotaje);
	pthread_detach(hilo_consola);
	}
	return EXIT_SUCCESS;
}

//void iniciar_escucha_sabotaje(void){
//	while(1){
//		obtener_orden_sabotaje();
//	}
//}

//void iniciar_escucha_por_consola(void){
//	while(1){
//		obtener_orden_input();
//	}

//}


void obtener_datos_de_config(t_config* config) {

	IP_MI_RAM = config_get_string_value(config, "IP_MI_RAM_HQ");
	PUERTO_MI_RAM = config_get_string_value(config, "PUERTO_MI_RAM_HQ");

	IP_MONGO_STORE = config_get_string_value(config, "IP_I_MONGO_STORE");
	PUERTO_MONGO_STORE = config_get_string_value(config, "PUERTO_I_MONGO_STORE");

	DURACION_SABOTAJE = config_get_int_value(config, "DURACION_SABOTAJE");
	RETARDO_CICLO_CPU = config_get_int_value(config, "RETARDO_CICLO_CPU");

}

void obtener_orden_input()
{
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

	switch(operacion){


	// Arranca todo pausado

		case INICIAR_PLANIFICACION:

			// ARRANCA LA PLANIFICACION DE LOS TRIPULANTES (BUSCANDO EL ALGORITMO QUE ESTA EN CONFIG)
			// LOS TRIPULANTES ESTAN DEFINIDOS POR HILO -> CADA HILO IRIA A UNA COLA
			// PONE A TODOS LOS TRIPULANTES EN EL ESTADO EXECUTE

			elegir_algoritmo();


			break;

		case PAUSAR_PLANIFICACION:

			// PAUSA LA PLANIFICACION DE LOS TRIPULANTES, ES DECIR TODOS EN PAUSA? o se mete algun WAIT(signal)
			// PONE A TODOS LOS TRIPULANTES EN EL ESTADO BLOCK I/O
			break;

		case INICIAR_PATOTA:
			// Ej: INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4 1|1

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

			t_tcb** tripulante;

			if(validacion_envio(conexion_mi_ram) == 1) {
				recibir_mensaje(respuesta, RESPUESTA_INICIAR_PATOTA, conexion_mi_ram);

				printf("%u\n", respuesta->respuesta);

				if(respuesta->respuesta == 1){
					printf("La respuesta fue positiva. \n");
				}
				else {
					printf("La respuesta fue negativa. \n");			// Salgo del Switch, ya que no pudo crearse la Patota en Mi-RAM HQ
					break;
				}

				/*int posicion = 0;
				for(int c=1; c <= cantidad_tripulantes; c++){
				 	tripulante[c] = malloc(sizeof(t_tcb));

					//tripulante[c]->id_tripulante = VARIABLE GLOBAL;
					tripulante[c]->estado_tripulante = 'N';
					tripulante[c]->posicion_x = atoi(parser_posiciones[posicion]);
					tripulante[c]->posicion_y = atoi(parser_posiciones[posicion+1]);
					tripulante[c]->id_proxima_instruccion = 0;
					tripulante[c]->puntero_PCB = 0;

					// Crear el Hilo
					// Mandar a la cola de New
					enviar_mensaje(tripulante[c], INICIAR_TRIPULANTE, conexion_mi_ram);

					// Valido el envio
					// Recibo la respuesta
					// SI es positiva
						// Cambiar el estado a READY
					// SI es Negativa

					posicion += 2;
				}*/
			}




			//Me envia MiRam la confirmaciones del PCB

			//for(int i=0; i<cantidad_tripulantes;i++){
			//tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));
			//tripulante->id_tripulante=contador_id;

			//}



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

		case LISTAR_TRIPULANTES:

			if(parser_consola[1] != NULL) {
				log_warning(logger, "Sobran argumentos. Debe iniciarse de la forma LISTAR_TRIPULANTES.");
				break;
			}

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

			// Buscar el hilo del tripulante a expulsar y eliminarlo (DELETE)?

			conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);		// Me conecto con el modulo Mi RAM HQ

			if(conexion_mi_ram < 0) {										// En el caso que no pueda conectar, sale del CASE
			   log_error(logger, "No se pudo conectar. \n");
			   break;
			}

			enviar_mensaje(id_tripulante_a_expulsar, EXPULSAR_TRIPULANTE, conexion_mi_ram);

			free(id_tripulante_a_expulsar);
			cerrar_conexion(logger,conexion_mi_ram);
			break;

		case TERMINAR_PROGRAMA:
			printf("Terminando programa... \n");
			sleep(1);
			printf("-------------------------------------------------------------------------------------------------------------------------------------------------- \n");
			// Libero memoria
			free(parser_consola);
			free(cadena_ingresada);
			terminar_programa(config, logger);

			exit(0);

		default:
			printf("No se reconoce ese comando. Por favor, ingrese un comando válido.\n");
			break;
		}

	sem_post(termino_operacion);

	free(parser_consola);
	free(cadena_ingresada);

	return;
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

/*
 * CODIGO QUE PODEMOS UTILIZAR
 *
		int posicion = 0;
		for(uint32_t c=0; c<cantidad_tripulantes; c++){
			t_datos_hilo* datos_hilo = malloc(sizeof(t_datos_hilo));
			datos_hilo->posicion_x = atoi(parser_posiciones[posicion]);
			datos_hilo->posicion_y = atoi(parser_posiciones[posicion+1]);

			// Array para guardar todos los tripulantes (guarda los hilos)
			// Array para guardar todas las patotas (guarda los procesos)

			pthread_create(&(hilo_tripulante), NULL, (t_tcb*)crear_tripulante, (t_datos_hilo*) datos_hilo);

			//tripulantes = tripulantes ->sig siempre que sea NULL, y ahi guarda el tripulante creado
			pthread_join(hilo_tripulante, &tripulantes);

			posicion += 2;
			free(datos_hilo);
		}

		 t_tcb** tripulantes = malloc(sizeof(t_tcb)*cantidad_tripulantes);

		int posicion1 = 0;
		for(uint32_t c=1; c<=cantidad_tripulantes; c++){
			tripulantes[c] = malloc(sizeof(t_tcb));
			tripulantes[c]->id_tripulante = c;
			tripulantes[c]->estado_tripulante = 'N';
			tripulantes[c]->posicion_x = atoi(parser_posiciones[posicion1]);
			tripulantes[c]->posicion_y = atoi(parser_posiciones[posicion1+1]);
			tripulantes[c]->id_proxima_instruccion = 0;

			posicion1 += 2;
		}

		for(uint32_t k=1; k<=cantidad_tripulantes; k++) {
			mostrar_tripulante(tripulantes[k]);
		}

		for(uint32_t k=1; k<=cantidad_tripulantes; k++) {
			enviar_mensaje(tripulantes[k], INICIAR_PATOTA, conexion_mi_ram);
		}
		*/

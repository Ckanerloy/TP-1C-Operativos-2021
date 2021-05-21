#include "Discordiador.h"

int main(void) {

	logger = crear_log("discordiador.log", "DISCORDIADOR");
	t_config* config = crear_config(CONFIG_PATH);
	obtener_datos_de_config(config);

	comando_para_ejecutar = malloc(sizeof(sem_t));
	sabotaje = malloc(sizeof(sem_t));

	sem_init(sabotaje, 0, 1);
	sem_init(comando_para_ejecutar, 0, 1);

	while(1) {


		// Conexion de escucha con MongoStore?  POR SABOTAJE
		//sem_wait(sabotaje);
		//pthread_create(&hilo_sabotaje, NULL, (void*)estar_atento_por_sabotaje, NULL);


		sem_wait(comando_para_ejecutar);
		pthread_create(&hilo_consola, NULL,(void*)obtener_orden_input, NULL);


		pthread_detach(hilo_sabotaje);
		//pthread_detach(hilo_consola);
	}

	terminar_programa(config, logger);

	return EXIT_SUCCESS;
}



void obtener_datos_de_config(t_config* config) {

	IP_MI_RAM = config_get_string_value(config, "IP_MI_RAM_HQ");
	PUERTO_MI_RAM = config_get_string_value(config, "PUERTO_MI_RAM_HQ");

	IP_MONGO_STORE = config_get_string_value(config, "IP_I_MONGO_STORE");
	PUERTO_MONGO_STORE = config_get_string_value(config, "PUERTO_I_MONGO_STORE");

	GRADO_MULTITAREA = config_get_int_value(config, "GRADO_MULTITAREA");
	ALGORITMO = config_get_string_value(config, "ALGORITMO");
	QUANTUM = config_get_int_value(config, "QUANTUM");
	DURACION_SABOTAJE = config_get_int_value(config, "DURACION_SABOTAJE");
	RETARDO_CICLO_CPU = config_get_int_value(config, "RETARDO_CICLO_CPU");

}

void obtener_orden_input() {


	 char* cadena_ingresada = NULL;
		//respuesta_ok_error* estructuraRespuesta;
	 size_t longitud = 0;

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

		case INICIAR_PLANIFICACION:

			// ARRANCA LA PLANIFICACION DE LOS TRIPULANTES (BUSCANDO EL ALGORITMO QUE ESTA EN CONFIG)
			// LOS TRIPULANTES ESTAN DEFINIDOS POR HILO -> CADA HILO IRIA A UNA COLA
			// PONE A TODOS LOS TRIPULANTES EN EL ESTADO EXECUTE

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
			uint32_t tamanio_tripulacion = 0;

			if(posiciones_faltantes < 0) {
				log_error(logger, "Se ingresaron posiciones demás. Solo puede como máximo haber tantas posiciones como cantidad de tripulantes.\n");
				break;
			}

			strcat(parser_consola[1],"\0");
			strcat(parser_consola[2],"\0");

			t_patota* mensaje_patota = malloc(sizeof(t_patota));
			mensaje_patota->cantidad_tripulantes = atoi(parser_consola[1]);
			mensaje_patota->tamanio_tareas = strlen(parser_consola[2]);
			mensaje_patota->archivo_tareas = malloc(mensaje_patota->tamanio_tareas+1);
			strcpy(mensaje_patota->archivo_tareas, parser_consola[2]);

			// Indica que arranca a leer a partir de los primeros 3 argumentos: COMANDO CANTIDAD_TRIPULANTES ARCHIVO_TAREAS

			int ubicacion_parser = 3;

			char* posiciones = string_new();

			while(parser_consola[ubicacion_parser] != NULL && ubicacion_parser < cantidad_argumentos) {
				string_append_with_format(&posiciones, "%s|", parser_consola[ubicacion_parser]);
				ubicacion_parser++;
			}
			for(int i = 0; i<posiciones_faltantes; i++) {
				string_append_with_format(&posiciones, "%s|", "0|0");
			}

			printf("%s \n", posiciones);


			parser_posiciones = string_split(posiciones, "|");
			string_trim(parser_posiciones);


			int n = 0;
			while(parser_posiciones[n] != NULL){
				printf("%s \n", parser_posiciones[n]);
				n++;
			}

			/*

			while(parser_consola[ubicacion_parser] != NULL && ubicacion_parser<cantidad_argumentos) {
				printf("%s \n", parser_consola[ubicacion_parser]);
				parser_posiciones = string_split(parser_consola[ubicacion_parser], "|");
				//printf("Posicion X: %s \n", parser_posiciones[0]);
				posiciones[n] = parser_posiciones[0];
				//printf("%s \n", posiciones[n]);
				n++;
				posiciones[n] = parser_posiciones[1];
				//printf("%s \n", posiciones[n]);
				//printf("Posicion Y: %s \n", parser_posiciones[1]);
				ubicacion_parser++;
				n ++;
			}

			for(int i=0; i<posiciones_faltantes; i++){
				//printf("0|0 \n");
				posiciones[n] = "0";
				//printf("%u \n", atoi(posiciones[n]));
				//printf("%s \n", posiciones[n]);
				n++;
				posiciones[n] = "0";
				//printf("%s \n", posiciones[n]);
				n++;
				//printf("Posicion X: 0 \n");
				//printf("Posicion Y: 0 \n");
			}
			int c = 0;
			while(posiciones[c] != NULL) {
				printf("%s \n", posiciones[c]);
				c++;
			}*/

			// Ejemplo para mostrar como no se puede hacer parser_posiciones = string_split(parser_consola[n], "|");
			//printf("%s \n", parser_posiciones[0]);
			//printf("%s \n", parser_posiciones[1]);
			//printf("%s \n", parser_posiciones[2]);
			//printf("%s \n", parser_posiciones[3]);

			/*tamanio_tripulacion += sizeof(mensaje_patota->tripulantes[n]);

			for(int c= 0; c<cantidad_tripulantes; c++) {
			 	mensaje_patota->tripulantes[c] = malloc(sizeof(tripulante));
				mensaje_patota->tripulantes[c] = crear_tripulante(...);

				tamanio_tripulacion += sizeof(mensaje_patota->tripulantes[c]);
			}*/

			mensaje_patota->tamanio_tripulantes = tamanio_tripulacion;



			conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);		// Me conecto con el modulo Mi RAM HQ

			if(conexion_mi_ram < 0) {										// En el caso que no pueda conectar, sale del CASE
				log_error(logger, "No se pudo conectar. \n");
				break;
			}

			//Falta la validacion de CantidadTripulantes con respecto con la cantidad de ubicaciones ingresadas

			/*for(int i=0;i<cantidadUbicaciones;i++){
				mensaje_patota->ubicacionTripulantes[i]=malloc(sizeof(char*));
			    strcpy(mensaje_patota->ubicacionTripulantes[i],parser_consola[3+i]);
			    //printf("%s",mensaje_patota->ubicacionTripulantes[i]);
			}

			enviar_mensaje(mensaje_patota, INICIAR_PATOTA, conexion_mi_ram);*/



			free(cantidad_argumentos);
			free(cantidad_posiciones);
			free(cantidad_tripulantes);
			free(posiciones_faltantes);
			free(tamanio_tripulacion);
			free(n);

			free(mensaje_patota->archivo_tareas);
			//free(mensaje_patota->cantidad_tripulantes);
			free(mensaje_patota);
			close(conexion_mi_ram);
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
				//printf("Faltan argumentos. Debe inciarse de la forma OBTENER_BITACORA <Id_Tripulante>.\n");
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

			// Consulta con Mongo Store y le pasa la bitacora del tripulante pasado por parametro
			// Y tambien tengo que escuchar al modulo Mongo Store, para recibir la lista y mostrarla? (IDEM como LISTAR_TRIPULANTES

			free(id_tripulante_x_bitacora);
			close(conexion_mongo_store);
			break;

		case EXPULSAR_TRIPULANTE:
				// ELIMINA EL TRIPULANTE DEL HILO DEL DISCORDIADOR

				// CONECTA CON MI RAM HQ Y LO ELIMINA DE MEMORIA
			    // 		- TAMBIEN LO ELIMINA DEL MAPA

			    if(parser_consola[1] == NULL) {
			    	//printf("Faltan argumentos. Debe inciarse de la forma EXPULSAR_TRIPULANTE <Id_Tripulante>.\n");
			    	log_error(logger, "Faltan argumentos. Debe inciarse de la forma EXPULSAR_TRIPULANTE <Id_Tripulante>.");
			    	break;
			    }
			    strcat(parser_consola[1], "\0");

			    t_id_tripulante* id_tripulante_a_expulsar = malloc(sizeof(t_id_tripulante));
			    id_tripulante_a_expulsar->id_tripulante = atoi(parser_consola[1]);

			    conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);		// Me conecto con el modulo Mi RAM HQ

			    if(conexion_mi_ram < 0) {										// En el caso que no pueda conectar, sale del CASE
			    	log_error(logger, "No se pudo conectar. \n");
			    	break;
			    }

			    enviar_mensaje(id_tripulante_a_expulsar, EXPULSAR_TRIPULANTE, conexion_mi_ram);

			    free(id_tripulante_a_expulsar);
			    close(conexion_mi_ram);
			break;

		case -1:

			break;

		default:
			printf("No se reconoce ese comando. Por favor, ingrese un comando válido.\n");
				// CODIGO POR DEFAULT, o sea si no pudo hacer nada, o quiero terminar
			break;
		}

	free(parser_consola);
	free(cadena_ingresada);
	return;
}


void estar_atento_por_sabotaje(){

	//conexion_mongo_store = crear_conexion(IP_MONGO_STORE,PUERTO_MONGO_STORE);
	conexion_sabotaje = iniciar_servidor(IP_MONGO_STORE, PUERTO_MONGO_STORE);
	// SABOTAJE -> BLOQUEAR A TODOS LOS TRIPULANTES
}


void arreglar_sabotaje() {
	// MANDA TRIPULANTE MAS CERCANO A LA UBICACION DEL SABOTAJE PARA QUE LO SOLUCIONE
}


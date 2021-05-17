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

	 strcpy(comando_ingresado,parser_consola[0]);

	 sem_post(comando_para_ejecutar);
	 operacion = mapeo_valor_consola(comando_ingresado);

	switch(operacion) {

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
			// El primer argumento es la cantidad de tripulantes
			// El segundo argumento es la ubicacion del archivo de tareas para cada patota
			// A partir del tercer argumento, es la posicion inicial de cada tripulante

			// Ej: INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 3|4 1|1

			if(parser_consola[1] == NULL || parser_consola[2] == NULL){
				//printf("Faltan argumentos. Debe iniciarse de la forma: INICIAR_PATOTA <CantidadTripulantes> >Ubicación txt Tareas>\n");
				log_warning(logger, "Faltan argumentos. Debe iniciarse de la forma: INICIAR_PATOTA <CantidadTripulantes> >Ubicación txt Tareas>.");
				break;
			}

			strcat(parser_consola[1],"\0");
			strcat(parser_consola[2],"\0");


			conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);		// Me conecto con el modulo Mi RAM HQ

			if(conexion_mi_ram < 0) {										// En el caso que no pueda conectar, sale del CASE
				log_error(logger, "No se pudo conectar. \n");
				break;
			}

			patota* mensaje_patota = malloc(sizeof(patota));
			mensaje_patota->cantidad_tripulantes = atoi(parser_consola[1]);
			mensaje_patota->tamanio_tareas = strlen(parser_consola[2]);
			mensaje_patota->archivo_tareas = malloc(mensaje_patota->tamanio_tareas+1);
			strcpy(mensaje_patota->archivo_tareas, parser_consola[2]);

			// TE DEBO COMO OBTENER LA POSICION DE CADA TRIPULANTE
			//Primero tengo que saber cuantas posiciones de tripulantes estan inicializados
			// una vez sabiendo esa cantidad, puedo incializar un vector para meter en las primeras N posiciones la ubicacion de esos tripulantes
			// en el caso que no esten inicializados, se toman por defecto 0|0
			int cantidadUbicaciones=cantidadStringsIngresados(parser_consola) - 3;
			//Falta la validacion de CantidadTripulantes con respecto con la cantidad de ubicaciones ingresadas
			mensaje_patota->ubicacionTripulantes=malloc(cantidadUbicaciones*sizeof(char *));

			for(int i=0;i<cantidadUbicaciones;i++){
				mensaje_patota->ubicacionTripulantes[i]=malloc(sizeof(char*));
			    strcpy(mensaje_patota->ubicacionTripulantes[i],parser_consola[3+i]);
			    //printf("%s",mensaje_patota->ubicacionTripulantes[i]);
			}

			enviar_mensaje(mensaje_patota, INICIAR_PATOTA, conexion_mi_ram);

			enviar_mensaje(mensaje_patota, INICIAR_PATOTA, conexion_mi_ram);

			for(int i=0;i<cantidadUbicaciones;i++){
				free(mensaje_patota->ubicacionTripulantes[i]);
			}

			free(mensaje_patota->ubicacionTripulantes);
			free(mensaje_patota->archivo_tareas);
			//free(mensaje_patota->cantidad_tripulantes);
			free(mensaje_patota);
			close(conexion_mi_ram);
			break;

		case LISTAR_TRIPULANTES:

			if(parser_consola[1] != NULL) {
				//printf("Sobran argumentos. Debe iniciarse de la forma LISTAR_TRIPULANTES.\n");
				log_warning(logger, "Sobran argumentos. Debe iniciarse de la forma LISTAR_TRIPULANTES.");
				break;
			}
			strcat(parser_consola[1], "\0");

			conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);


			//enviar_mensaje(mensajeVacio, LISTAR_TRIPULANTES, conexion_mi_ram);
			// Escucharia al modulo Mi RAM, y este ultimo le envia toda una lista para mostrar en pantalla de acuerdo a este orden
			// Estado de la Nave:  DD/MM/AAAA  HH:MM:SS
			// Tripulante: N	Patota: P	Status: E
			// N: Id de tripulante
			// P: Id de la patota
			// E: Estado del tripulante

			close(conexion_mi_ram);
			break;

		case OBTENER_BITACORA:

			if(parser_consola[1] == NULL) {
				//printf("Faltan argumentos. Debe inciarse de la forma OBTENER_BITACORA <Id_Tripulante>.\n");
				log_warning(logger, "Faltan argumentos. Debe inciarse de la forma OBTENER_BITACORA <Id_Tripulante>.");
				break;
			}
			strcat(parser_consola[1], "\0");

			conexion_mongo_store = crear_conexion(IP_MONGO_STORE, PUERTO_MONGO_STORE);

			tripulante* mensaje_bitacora = malloc(sizeof(tripulante));
			mensaje_bitacora->id_tripulante = atoi(parser_consola[1]);

			enviar_mensaje(mensaje_bitacora, OBTENER_BITACORA, conexion_mongo_store);
			// OBTENGO EL ID DEL TRIPULANTE PARA PEDIRLE LA BITACORA

			// Consulta con Mongo Store y le pasa la bitacora del tripulante pasado por parametro
			// Y tambien tengo que escuchar al modulo Mongo Store, para recibir la lista y mostrarla? (IDEM como LISTAR_TRIPULANTES

			free(mensaje_bitacora->id_tripulante);
			free(mensaje_bitacora);
			close(conexion_mongo_store);
			break;

		case EXPULSAR_TRIPULANTE:
				// ELIMINA EL TRIPULANTE DEL HILO DEL DISCORDIADOR
			    conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);
				// CONECTA CON MI RAM HQ Y LO ELIMINA DE MEMORIA
			    // 		- TAMBIEN LO ELIMINA DEL MAPA

			    if(parser_consola[1] == NULL) {
			    	//printf("Faltan argumentos. Debe inciarse de la forma EXPULSAR_TRIPULANTE <Id_Tripulante>.\n");
			    	log_warning(logger, "Faltan argumentos. Debe inciarse de la forma EXPULSAR_TRIPULANTE <Id_Tripulante>.");
			    	break;
			    }

			    tripulante* mensaje_tripulante = malloc(sizeof(tripulante));
			    mensaje_tripulante->id_tripulante = atoi(parser_consola[1]);

			    enviar_mensaje(mensaje_tripulante, EXPULSAR_TRIPULANTE, conexion_mi_ram);

			    free(mensaje_tripulante->id_tripulante);
			    free(mensaje_tripulante);
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

int cantidadStringsIngresados(char** parser_consola){  //la vamos a usar tanto para verifiaciones de ingresos
	int cantidad=0;									   // por consola, como para cantidad de tripu que arrancan en 0,0
	while(parser_consola[cantidad] != NULL){
		cantidad++;
	}
	return cantidad;

}

void estar_atento_por_sabotaje(){

	//conexion_mongo_store = crear_conexion(IP_MONGO_STORE,PUERTO_MONGO_STORE);
	conexion_sabotaje = iniciar_servidor(IP_MONGO_STORE, PUERTO_MONGO_STORE);
	// SABOTAJE -> BLOQUEAR A TODOS LOS TRIPULANTES
}


void arreglar_sabotaje() {
	// MANDA TRIPULANTE MAS CERCANO A LA UBICACION DEL SABOTAJE PARA QUE LO SOLUCIONE
}


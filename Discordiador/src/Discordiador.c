#include "Discordiador.h"

int main(void) {

	obtener_datos_de_config(CONFIG_PATH);

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

		/*if(strcmp(leido, "1") == 0 ){
			conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);

			if(conexion_mi_ram != -1) {
				t_paquete* paquete = armar_paquete();
					enviar_paquete(paquete, conexion_mi_ram);

					close(conexion_mi_ram);
			}
			else {
				puts("No se pudo conectar al servidor.");
			}


		}
		else if(strcmp(leido, "2") == 0 ){
			conexion_mongo_store = crear_conexion(IP_MONGO_STORE,PUERTO_MONGO_STORE);

			if(conexion_mongo_store != -1) {
				t_paquete* paquete = armar_paquete();
				enviar_paquete(paquete, conexion_mongo_store);

				close(conexion_mongo_store);
			}
			else {
				puts("No se pudo conectar al servidor.");
			}

		}
		else{
			puts("Cerrando el programa.");
			config_destroy(config);
			return EXIT_SUCCESS;
		}*/

	return EXIT_SUCCESS;
}



t_log* iniciar_logger(void)
{
	return log_create("discordiador.log", "TP0", 1, LOG_LEVEL_INFO);
}


void obtener_datos_de_config(char* config_path) {

	config = config_create(config_path);

	IP_MI_RAM = config_get_string_value(config, "IP_MI_RAM_HQ");
	PUERTO_MI_RAM = config_get_string_value(config, "PUERTO_MI_RAM_HQ");

	IP_MONGO_STORE = config_get_string_value(config, "IP_I_MONGO_STORE");
	PUERTO_MONGO_STORE = config_get_string_value(config, "PUERTO_I_MONGO_STORE");

}

void obtener_orden_input(char* comando_entrada) {


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
				printf("Faltan argumentos. Debe iniciarse de la forma: INICIAR_PATOTA [CantidadTripulantes] [Ubicación txt Tareas]\n");
				break;
			}

			strcat(parser_consola[2],"\0");

			// Me conecto con el modulo Mi RAM HQ
			conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);

			// Primero reservo la memoria de lo que le voy a enviar, y acto seguido le guardo la "data"
			patota* mensaje_patota = malloc(sizeof(patota));
			mensaje_patota->cantidad_tripulantes = atoi(parser_consola[1]);
			mensaje_patota->tamanio_tareas = strlen(parser_consola[2] + 1);
			strcpy(mensaje_patota->archivo_tareas, parser_consola[2]);

			// TE DEBO COMO OBTENER LA POSICION DE CADA TRIPULANTE
			//Primero tengo que saber cuantas posiciones de tripulantes estan inicializados
			// una vez sabiendo esa cantidad, puedo incializar un vector para meter en las primeras N posiciones la ubicacion de esos tripulantes
			// en el caso que no esten inicializados, se toman por defecto 0|0

			// enviar_mensaje(EL DATO A ENVIAR, LA FUNCION QUE ESTOY USANDO PARA ENVIAR, EL SOCKET A ENVIAR);
			// enviar_mensaje(mensaje_patota, INICIAR_PATOTA, conexion_mi_ram);


			free(mensaje_patota->cantidad_tripulantes);
			free(mensaje_patota->archivo_tareas);
			free(mensaje_patota);
			close(conexion_mi_ram);
			break;

		case LISTAR_TRIPULANTES:

			// Escucharia al modulo Mi RAM, y este ultimo le envia toda una lista para mostrar en pantalla de acuerdo a este orden
			// Estado de la Nave:  DD/MM/AAAA  HH:MM:SS
			// Tripulante: N	Patota: P	Status: E
			// N: Id de tripulante
			// P: Id de la patota
			// E: Estado del tripulante

			break;
		case OBTENER_BITACORA:

			if(parser_consola[1] == NULL) {
				printf("Faltan argumentos. Debe inciarse de la forma OBTENER_BITACORA [ID_TRIPULANTE].\n");
				break;
			}

			conexion_mongo_store = crear_conexion(IP_MONGO_STORE, PUERTO_MONGO_STORE);

			bitacora* mensaje_bitacora = malloc(sizeof(bitacora));
			mensaje_bitacora->id_tripulante = atoi(parser_consola[1]);
			// OBTENGO EL ID DEL TRIPULANTE PARA PEDIRLE LA BITACORA

			// Consulta con Mongo Store y le pasa la bitacora del tripulante pasado por parametro
			// Y tambien tengo que escuchar al modulo Mongo Store, para recibir la lista y mostrarla? (IDEM como LISTAR_TRIPULANTES

			close(conexion_mongo_store);
			break;
		case EXPULSAR_TRIPULANTE:
				// ELIMINA EL TRIPULANTE DEL HILO DEL DISCORDIADOR
			    conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);
				// CONECTA CON MI RAM HQ Y LO ELIMINA DE MEMORIA
					// - TAMBIEN LO ELIMINA DEL MAPA
				// CODIGO DE LA FUNCION

			    close(conexion_mi_ram);
			break;
		default:
			printf("No se reconoce ese comando. Por favor, ingrese un comando válido.\n");
				// CODIGO POR DEFAULT, o sea si no pudo hacer nada, o quiero terminar
			break;
		}
}


void estar_atento_por_sabotaje(){

	//conexion_mongo_store = crear_conexion(IP_MONGO_STORE,PUERTO_MONGO_STORE);
	conexion_sabotaje = iniciar_servidor(IP_MONGO_STORE, PUERTO_MONGO_STORE);
	// SABOTAJE -> BLOQUEAR A TODOS LOS TRIPULANTES
}


void arreglar_sabotaje() {
	// MANDA TRIPULANTE MAS CERCANO A LA UBICACION DEL SABOTAJE PARA QUE LO SOLUCIONE
}



t_paquete* armar_paquete() {
	t_paquete* paquete = crear_paquete();

	char* leido = readline(">Ingrese un valor: ");

	while(strcmp(leido, "") != 0 ) {
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		free(leido);
		leido = readline(">Ingrese un valor: ");
	}

	free(leido);

	return paquete;
}

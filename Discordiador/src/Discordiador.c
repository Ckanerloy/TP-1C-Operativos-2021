#include "Discordiador.h"

int main(void) {

	obtener_datos_de_config(CONFIG_PATH);

	comando_para_ejecutar = malloc(sizeof(sem_t));

	sem_init(comando_para_ejecutar, 0, 1);

	while(1) {


		// Conexion de escucha con MongoStore?  POR SABOTAJE
		//sem_wait(sabotaje);
		//pthread_create(&hilo_sabotaje, NULL, (void*)estar_atento_por_sabotaje, NULL);
		//pthread_detach(hilo_sabotaje);

		sem_wait(comando_para_ejecutar);
		pthread_create(&hilo_consola, NULL,(void*)obtener_orden_input, NULL);
		pthread_detach(hilo_consola);



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
	 uint32_t switcher;
		//respuesta_ok_error* estructuraRespuesta;

	 int32_t conexion_socket;
	 int32_t tamanio_a_guardar = 0;
	 uint32_t exito = 0;
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
				puts("Iniciando planificacion");
				puts("-----------------------------------------------------------------------------------------------------------------");
				conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);
				// ARRANCA LA PLANIFICACION DE LOS TRIPULANTES (BUSCANDO EL ALGORITMO QUE ESTA EN CONFIG)
					// - SE COMUNICA CON MI RAM HQ CON EL METODO INICIAR_TRIPULANTE
				// LOS TRIPULANTES ESTAN DEFINIDOS POR HILO -> CADA HILO IRIA A UNA COLA
				// CODIGO DE LA FUNCION
			break;
		case PAUSAR_PLANIFICACION:
				puts("SE PAUSO LA PLANIFICACION");
				puts("ZZZ ZZZ ZZZ ZZZ ZZZ ZZZ ZZZ ZZZ ");
				// CODIGO DE LA FUNCION
			break;
		case INICIAR_PATOTA:
				// CODIGO DE LA FUNCION
			break;
		case LISTAR_TRIPULANTES:
				// CODIGO DE LA FUNCION
			break;
		case OBTENER_BITACORA:
				// CODIGO DE LA FUNCION
			break;
		case EXPULSAR_TRIPULANTE:
				// ELIMINA EL TRIPULANTE DEL HILO DEL DISCORDIADOR
			    conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);
				// CONECTA CON MI RAM HQ Y LO ELIMINA DE MEMORIA
					// - TAMBIEN LO ELIMINA DEL MAPA
				// CODIGO DE LA FUNCION
			break;
		default:
				// CODIGO POR DEFAULT, o sea si no pudo hacer nada, o quiero terminar
			break;
		}
}


void estar_atento_por_sabotaje(){

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

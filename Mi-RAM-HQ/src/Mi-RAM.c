#include "Mi-RAM.h"

int main(void)
{
	t_config* config = crear_config(CONFIG_PATH);
	obtener_datos_de_config(config);

	t_log* logger = crear_log("mi-ram-hq.log", "Mi-RAM HQ");
	log_info(logger, "Servido activo, esperando instrucciones ... \n");

	//espera = malloc(sizeof(sem_t));
	//sem_init(espera, 0, 1);

	int32_t conexion_servidor = iniciar_servidor(IP, PUERTO);

	while(1) {

		int32_t* conexion_cliente = esperar_conexion(conexion_servidor);
		//sem_wait(espera);
		pthread_create(&hilo_recibir_mensajes, NULL,(void*)escuchar_conexion, conexion_cliente);

		pthread_detach(hilo_recibir_mensajes);
	}


	liberarMemoria(config, logger);
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
}




void escuchar_conexion(int32_t* conexion_cliente)
{
	codigo_operacion operacion;

	recv(*conexion_cliente, &operacion, sizeof(operacion), MSG_WAITALL);

	procesar_mensajes(operacion, *conexion_cliente);
}


void procesar_mensajes(codigo_operacion operacion, int32_t conexion)
{
	t_patota* patota_recibida;
	//t_tripulante* tripulante_recibido;
	t_id_tripulante* tripulante_recibido;

	//sem_post(espera);

	switch(operacion)
			{
			case INICIAR_PATOTA:
				patota_recibida = malloc(sizeof(t_patota));
				recibir_mensaje(patota_recibida, operacion, conexion);
				/*
				printf("Cantidad de tripulantes: %d \n" , patota_recibida->cantidad_tripulantes);
				printf("Archivo de tareas: %s \n", patota_recibida->archivo_tareas);
 	 	 	 	 */
				//fork();
				//crear_patota(patota_recibida);
				// Recibe la patota de parte del discordiador
				// Y prepara cada patota como un PROCESO, y cada tripulante como un HILO
				free(patota_recibida->archivo_tareas);
				free(patota_recibida);
				break;

			case EXPULSAR_TRIPULANTE:
				tripulante_recibido = malloc(sizeof(t_id_tripulante));
				recibir_mensaje(tripulante_recibido, operacion, conexion);

				printf("Tripulante a Expulsar: %u \n", tripulante_recibido->id_tripulante);
				// Eliminar el tripulante o hilo dentro de la memoria y del mapa

				free(tripulante_recibido);
				break;

			default:
				log_warning(logger, "Operacion desconocida. No quieras meter la pata");
				break;
			}
}

#include "Mi-RAM.h"

int main(void)
{
	t_config* config = crear_config(CONFIG_PATH);

	obtener_datos_de_config(CONFIG_PATH);

	t_log* logger = crear_log("mi-ram-hq.log", "Mi-RAM HQ");


	int32_t conexion_servidor = iniciar_servidor(IP, PUERTO);

	//if(SI NO SE PUDO CONECTAR) {

//	}
	log_info(logger, "Servido activo, esperando instrucciones ... \n");

	int32_t conexion_cliente = esperar_cliente(conexion_servidor);


	int operacion = recibir_operacion(conexion_cliente);

	procesar_mensajes(operacion, conexion_cliente);

	// CUANDO SE CONECTA CON EL DISCORDIADOR, SE CONECTA EN UN HILO


	liberarMemoria(config, logger);
	return EXIT_SUCCESS;
}



void obtener_datos_de_config(char* config_path) {

	config = config_create(config_path);

	PUERTO = config_get_string_value(config, "PUERTO");
	TAMANIO_MEMORIA = config_get_string_value(config, "TAMANIO_MEMORIA");
	char* ESQUEMA_MEMORIA;
	int TAMANIO_PAGINA;
	int TAMANIO_SWAP;
	char* PATH_SWAP;
	char* ALGORITMO_REEMPLAZO;


}




void procesar_mensajes(codigo_operacion operacion, int32_t conexion) {

	patota* patota_recibida;
	tripulante* tripulante_recibido;


	switch(operacion)
			{
			case INICIAR_PATOTA:
				patota_recibida = malloc(sizeof(patota));
				recibir_mensaje(patota_recibida, operacion, conexion);

				//fork();
				//crear_patota(patota_recibida);
				// Recibe la patota de parte del discordiador
				// Y prepara cada patota como un PROCESO, y cada tripulante como un HILO

				break;

			case LISTAR_TRIPULANTES:
				//Aca habria que listar todos los tripulantes, pero no recibiria ningun parametro, solamente el codigo de operacion
				recibir_mensaje("", operacion, conexion);

				break;

			case EXPULSAR_TRIPULANTE:
				tripulante_recibido = malloc(sizeof(tripulante));
				recibir_mensaje(tripulante_recibido, operacion, conexion);
				break;

			default:
				log_warning(logger, "Operacion desconocida. No quieras meter la pata");
				break;
			}
}

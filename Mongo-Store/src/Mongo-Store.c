#include "Mongo-Store.h"

int main(void)
{
	t_config* config = crear_config(CONFIG_PATH);

	obtener_datos_de_config(config);

	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);



	int server_fd = iniciar_servidor(IP, PUERTO);
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);
	codigo_operacion operacion;
	t_list* lista;
	while(1)
	{
		recibir_operacion(cliente_fd, operacion);

		switch(operacion)
		{
		case OBTENER_BITACORA:
			break;

		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			config_destroy(config);
			return EXIT_FAILURE;
		default:
			log_warning(logger, "Operacion desconocida. No quieras meter la pata");
			break;
		}
	}

	config_destroy(config);
	return EXIT_SUCCESS;
}


void obtener_datos_de_config(t_config* config) {

	PUERTO = config_get_string_value(config, "PUERTO");
	PUERTO_MONTAJE = config_get_string_value(config, "PUERTO_MONTAJE");
	TIEMPO_SINCRONIZACION = config_get_int_value(config, "TIEMPO_SINCRONIZACION");

}



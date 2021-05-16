#include "Mongo-Store.h"

int main(void)
{
	t_config* config = crear_config(CONFIG_PATH);

	void iterator(char* value)
	{
		printf("%s\n", value);
	}

	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	char* PUERTO = obtenerPuerto(config, CONFIG_PATH);

	int server_fd = iniciar_servidor(IP, PUERTO);
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(server_fd);

	t_list* lista;
	while(1)
	{
		int cod_op = recibir_operacion(cliente_fd);
		switch(cod_op)
		{
		case OBTENER_BITACORA:
			recibir_mensaje(cliente_fd);
			break;
		//case PAQUETE:
			//lista = recibir_paquete(cliente_fd);
			//printf("Me llegaron los siguientes valores:\n");
			//list_iterate(lista, (void*) iterator);
			//break;
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

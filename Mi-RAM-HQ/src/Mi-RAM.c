#include "Mi-RAM.h"

int main(void)
{
	t_config* config = crear_config(CONFIG_PATH);
	t_log* logger = crear_log("mi-ram-hq.log", "Mi-RAM HQ");

	void iterator(char* value)
	{
		printf("%s\n", value);
	}

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
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			printf("Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			liberarMemoria(config, logger);
			return EXIT_FAILURE;
		default:
			log_warning(logger, "Operacion desconocida. No quieras meter la pata");
			break;
		}
	}

	// CUANDO SE CONECTA CON EL DISCORDIADOR, SE CONECTA EN UN HILO


	liberarMemoria(config, logger);
	return EXIT_SUCCESS;
}

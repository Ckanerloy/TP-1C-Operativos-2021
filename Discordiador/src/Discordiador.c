#include "Discordiador.h"

int main(void) {
	char* leido;

	int conexion_cliente_mongo_store;
	int conexion_cliente_mi_ram;

	t_config* config = crear_config();

	char* IP_MI_RAM = config_get_string_value(config, "IP_MI_RAM_HQ");
	char* PUERTO_MI_RAM = config_get_string_value(config, "PUERTO_MI_RAM_HQ");

	char* IP_MONGO_STORE = config_get_string_value(config, "IP_I_MONGO_STORE");
	char* PUERTO_MONGO_STORE = config_get_string_value(config, "PUERTO_I_MONGO_STORE");

	puts(IP_MI_RAM);
	puts(PUERTO_MI_RAM);

	puts(IP_MONGO_STORE);
	puts(PUERTO_MONGO_STORE);

	while(1) {
		puts("> Indique con quien se quiere comunicar:");
		puts("1. Mi-RAM-HQ");
		puts("2. Mongo-Store");

		leido = readline(">");

		if(strcmp(leido, "1") == 0 ){
			conexion_cliente_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);


			close(conexion_cliente_mi_ram);
		}
		else if(strcmp(leido, "2") == 0 ){
			conexion_cliente_mongo_store = crear_conexion(IP_MONGO_STORE, PUERTO_MONGO_STORE);


			close(conexion_cliente_mongo_store);
		}
		else{
			puts("Cerrando el servidor.");
			config_destroy(config);
			return EXIT_SUCCESS;
		}
	}


	config_destroy(config);

	//close(conexion_cliente_mi_ram);
	//close(conexion_cliente_mongo_store);
	return EXIT_SUCCESS;
}



t_log* iniciar_logger(void)
{
	return log_create("discordiador.log", "TP0", 1, LOG_LEVEL_INFO);
}

t_config* crear_config(void)
{
	return config_create("discordiador.config");
}



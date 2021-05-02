#include "Discordiador.h"

int main(void) {
	puts("!!!Hello World!!!");

	t_config* config = crear_config();

	char* IP_MI_RAM = config_get_string_value(config, "IP_MI_RAM_HQ");
	char* PUERTO_MI_RAM = config_get_string_value(config, "PUERTO_MI_RAM_HQ");

	char* IP_MONGO_STORE = config_get_string_value(config, "IP_I_MONGO_STORE");
	char* PUERTO_MONGO_STORE = config_get_string_value(config, "PUERTO_I_MONGO_STORE");

	puts(IP_MI_RAM);
	puts(PUERTO_MI_RAM);

	puts(IP_MONGO_STORE);
	puts(PUERTO_MONGO_STORE);

	int conexion_cliente_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);
	int conexion_cliente_mongo_store = crear_conexion(IP_MONGO_STORE, PUERTO_MONGO_STORE);


	while(1) {

		//if(ME QUIERO CONECTAR AL MI RAM) {
		//	int conexion_cliente_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);
		//}

	//	if(ME QUIERO CONECTAR A MONGO STORE) {
		//	int conexion_cliente_mongo_store = crear_conexion(IP_MONGO_STORE, PUERTO_MONGO_STORE);
		//}

	}


	config_destroy(config);

	close(conexion_cliente_mi_ram);
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



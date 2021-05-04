#include "Discordiador.h"

int main(void) {
	char* leido;
	int conexion_cliente_mongo_store;
	int conexion_cliente_mi_ram;

	obtenerDatosDeConfig(CONFIG_PATH);

	while(1) {
		puts("> Indique con quien se quiere comunicar:");
		puts("1. Mi-RAM-HQ");
		puts("2. Mongo-Store");

		leido = readline(">");

		if(strcmp(leido, "1") == 0 ){
			conexion_cliente_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);


			t_paquete* paquete = armar_paquete();
			enviar_paquete(paquete, conexion_cliente_mi_ram);

			/*leido = readline(">");
			while(strcmp(leido,"EXIT") != 0){
				leido = readline(">");
			}*/

			close(conexion_cliente_mi_ram);
		}
		else if(strcmp(leido, "2") == 0 ){
			conexion_cliente_mongo_store = crear_conexion(IP_MONGO_STORE,PUERTO_MONGO_STORE);

			t_paquete* paquete = armar_paquete();
			enviar_paquete(paquete, conexion_cliente_mongo_store);

			close(conexion_cliente_mongo_store);
		}
		else{
			puts("Cerrando el programa.");
			config_destroy(config);
			return EXIT_SUCCESS;
		}
	}

	return EXIT_SUCCESS;
}



t_log* iniciar_logger(void)
{
	return log_create("discordiador.log", "TP0", 1, LOG_LEVEL_INFO);
}


void obtenerDatosDeConfig(char* config_path) {

	config = config_create(config_path);

	IP_MI_RAM = config_get_string_value(config, "IP_MI_RAM_HQ");
	PUERTO_MI_RAM = config_get_string_value(config, "PUERTO_MI_RAM_HQ");

	IP_MONGO_STORE = config_get_string_value(config, "IP_I_MONGO_STORE");
	PUERTO_MONGO_STORE = config_get_string_value(config, "PUERTO_I_MONGO_STORE");

}


t_paquete* armar_paquete() {
	t_paquete* paquete = crear_paquete();

	char* leido = readline("> Ingrese un valor:");

	while(strcmp(leido, "EXIT") != 0 ) {
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		free(leido);
		leido = readline("> Ingrese un valor:");
	}

	free(leido);

	return paquete;
}

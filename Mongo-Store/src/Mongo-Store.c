#include "Mongo-Store.h"

int main(void)
{
	config = crear_config(CONFIG_PATH);

	obtener_datos_de_config(config);

	logger = crear_log("mongo-store.log", "Mongo Store");
	log_info(logger, "Servidor Mongo Store activo...");

	int32_t conexion_servidor = iniciar_servidor(IP, PUERTO);

	while(1)
	{
		int32_t* conexion_cliente = esperar_conexion(conexion_servidor);

		pthread_create(&hilo_recibir_mensajes, NULL, (void*)escuchar_conexion, conexion_cliente);
		pthread_detach(hilo_recibir_mensajes);

	}

	terminar_programa(config, logger);
	return EXIT_SUCCESS;
}


void obtener_datos_de_config(t_config* config) {

	PUERTO = config_get_string_value(config, "PUERTO");
	PUNTO_MONTAJE = config_get_string_value(config, "PUNTO_MONTAJE");
	TIEMPO_SINCRONIZACION = config_get_int_value(config, "TIEMPO_SINCRONIZACION");
	POSICIONES_SABOTAJE = config_get_array_value(config, "POSICIONES_SABOTAJE");

}


void procesar_mensajes(codigo_operacion operacion, int32_t conexion) {

	t_tripulante* tripulante_por_bitacora;

	switch(operacion)
{
			case OBTENER_BITACORA:
				tripulante_por_bitacora = malloc(sizeof(t_tripulante));
				recibir_mensaje(tripulante_por_bitacora, operacion, conexion);

				// Recibe un id tripulante, y le envia la BITACORA del tripulante buscado
				free(tripulante_por_bitacora);
				break;

			default:
				log_warning(logger, "Operacion desconocida. No quieras meter la pata");
				break;
			}
}






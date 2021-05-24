#include "loader.h"

t_config* crear_config(char* config_path) {
	return config_create(config_path);
}

t_log* crear_log(char* ruta_log, char* nombreLog) {
	return log_create(ruta_log, nombreLog, 1, LOG_LEVEL_INFO);
}

void terminar_programa(t_config* config, t_log* logger)
{
	config_destroy(config);
	printf("Puntero a archivo .config destruido.\n");
	log_destroy(logger);
	printf("Puntero al logger destruido.\n");
}

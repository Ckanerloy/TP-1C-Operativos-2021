#ifndef LOADER_H_
#define LOADER_H_

#include "commons/config.h"
#include "commons/log.h"


t_config* crear_config(char* config_path);
t_log* crear_log(char* ruta_log, char* nombreLog);
t_log* crear_log_sin_pantalla(char* ruta_log, char* nombreLog);
void terminar_programa(t_config* config, t_log* logger);
void terminar_programa_discordiador(t_config* config, t_log* logger, t_log* logger_on);

#endif /* LOADER_H_ */

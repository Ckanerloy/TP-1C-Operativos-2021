#ifndef LOADER_H_
#define LOADER_H_

#include "commons/config.h"
#include "commons/log.h"


t_config* crear_config(char* config_path);
t_log* crear_log(char* ruta_log, char* nombreLog);
void liberarMemoria(t_config* config, t_log* logger);

#endif /* LOADER_H_ */
#ifndef UTILSDISCORDIADOR_H_
#define UTILSDISCORDIADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include "commons/log.h"
#include "commons/string.h"
#include "commons/config.h"

#include"utils/estructuras.h"

uint32_t mapeo_valor_consola(char* comando_ingresado);
void terminar_programa(t_log* logger, t_config* config);

void listar_tripulantes();
uint32_t cantidad_argumentos_ingresados(char** parser_consola);

void crear_tripulanteV2(t_datos_hilo* datos_hilo);

#endif /* UTILSDISCORDIADOR_H_ */

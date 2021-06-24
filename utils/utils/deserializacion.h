#ifndef UTILS_DESERIALIZACION_H_
#define UTILS_DESERIALIZACION_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include "commons/log.h"
#include "commons/config.h"
#include "commons/collections/list.h"
#include "estructuras.h"
#include "sockets.h"

t_log* logger;


void deserializar_iniciar_patota(t_iniciar_patota* mensaje, int32_t conexion);
void deserializar_respuesta_patota(t_respuesta_iniciar_patota* mensaje, int32_t conexion);
void deserealizar_tripulante(t_tripulante* mensaje, int32_t conexion);
void deseralizar_id_tripulante(t_tripulante* mensaje, int32_t conexion);

#endif /* UTILS_DESERIALIZACION_H_ */

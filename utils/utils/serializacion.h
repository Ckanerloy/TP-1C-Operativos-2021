#ifndef UTILS_SERIALIZACION_H_
#define UTILS_SERIALIZACION_H_

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

t_log* logger;

void* serializar_paquete(t_paquete* paquete, void* mensaje, codigo_operacion operacion, uint32_t* tamanio_paquete);
uint32_t serializar_paquete_iniciar_patota(t_paquete* paquete, t_iniciar_patota* mensaje);
uint32_t serializar_respuesta_iniciar_patota(t_paquete* paquete, t_respuesta_iniciar_patota* mensaje);
uint32_t serializar_paquete_tripulante(t_paquete* paquete, t_tcb* mensaje);
uint32_t serializar_paquete_id_tripulante(t_paquete* paquete, t_tripulante* mensaje);


#endif /* UTILS_SERIALIZACION_H_ */

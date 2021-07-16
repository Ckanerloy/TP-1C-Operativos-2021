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
#include "sockets.h"

t_log* logger;

void enviar_mensaje(void* mensaje, codigo_operacion operacion, int32_t conexion);
void* serializar_paquete(t_paquete* paquete, void* mensaje, codigo_operacion operacion, uint32_t* tamanio_paquete);

// Paquetes a enviar
uint32_t serializar_paquete_iniciar_patota(t_paquete* paquete, t_iniciar_patota* mensaje);
uint32_t serializar_paquete_tripulante(t_paquete* paquete, t_tripulante* mensaje);
uint32_t serializar_paquete_ubicacion_tripulante(t_paquete* paquete, t_tripulante_ubicacion* mensaje);
uint32_t serializar_paquete_estado_tripulante(t_paquete* paquete, t_tripulante_estado* mensaje);

// Respuestas
uint32_t serializar_respuesta_iniciar_patota(t_paquete* paquete, t_respuesta_iniciar_patota* mensaje);
uint32_t serializar_respuesta_tripulante(t_paquete* paquete, t_respuesta_tripulante* mensaje);
uint32_t serializar_respuesta_nueva_ubicacion(t_paquete* paquete, t_respuesta_tripulante_ubicacion* mensaje);
uint32_t serializar_respuesta_tarea_tripulante(t_paquete* paquete, t_respuesta_tarea_tripulante* mensaje);

//Tareas I/O
uint32_t serializar_paquete_tarea_io(t_paquete* paquete, archivo_tarea* mensaje);

#endif /* UTILS_SERIALIZACION_H_ */

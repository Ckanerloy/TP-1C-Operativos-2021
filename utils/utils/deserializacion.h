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

void recibir_mensaje(void* mensaje, codigo_operacion operacion, int32_t conexion);

// Paquetes a enviar
void deserializar_iniciar_patota(t_iniciar_patota* mensaje, int32_t conexion);
void deserializar_tripulante(t_tripulante* mensaje, int32_t conexion);
void deserializar_ubicacion_tripulante(t_tripulante_ubicacion* mensaje, int32_t conexion);
void deserializar_estado_tripulante(t_tripulante_estado* mensaje, int32_t conexion);
void deserializar_sabotaje(posiciones* mensaje, int32_t conexion);

// Respuestas
void deserializar_respuesta_patota(t_respuesta_iniciar_patota* mensaje, int32_t conexion);
void deserializar_respuesta_tripulante(t_respuesta_tripulante* mensaje, int32_t conexion);
void deserializar_respuesta_nueva_ubicacion(t_respuesta_tripulante_ubicacion* mensaje, int32_t conexion);
void deserializar_respuesta_nueva_tarea(t_respuesta_tarea_tripulante* mensaje, int32_t conexion);

// Tareas I/O
void deserializar_tarea_io(archivo_tarea* mensaje, int32_t conexion);

// Bitacora
void deserializar_bitacora(bitacora* mensaje, int32_t conexion);

#endif /* UTILS_DESERIALIZACION_H_ */

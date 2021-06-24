#ifndef UTILS_SOCKETS_H_
#define UTILS_SOCKETS_H_

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
#include "serializacion.h"
#include "deserializacion.h"

t_log* logger;
t_config* config;

// Conexiones
int32_t crear_conexion(char* ip, char* puerto);
void cerrar_conexion(t_log* logger, int32_t socket);
int32_t iniciar_servidor(char* IP, char* PUERTO);
void escuchar_conexion(int32_t* conexion_cliente);
int32_t* esperar_conexion(int32_t conexion_servidor);
void eliminar_paquete(t_paquete* paquete);

// Validaciones
int32_t resultado_conexion(int32_t conexion, t_log* logger, char* modulo) ;
int32_t validacion_envio(int32_t conexion_cliente);

void crear_buffer(t_paquete* paquete);

void recibir_operacion(int32_t socket_cliente, codigo_operacion operacion);
void* recibir_buffer(uint32_t* size, int32_t conexion_cliente);

#endif /* UTILS_SOCKETS_H_ */

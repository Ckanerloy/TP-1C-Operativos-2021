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

// Envio de mensajes
void enviar_mensaje(void* mensaje, codigo_operacion operacion, int32_t conexion);
void crear_buffer(t_paquete* paquete);

// Recepcion de mensajes
void recibir_mensaje(void* mensaje, codigo_operacion operacion, int32_t conexion);
void recibir_operacion(int32_t socket_cliente, codigo_operacion operacion);
void* recibir_buffer(uint32_t* size, int32_t conexion_cliente);


<<<<<<< HEAD
=======
// Serializaciones (para enviar un mensaje)
void* serializar_paquete(t_paquete* paquete, void* mensaje, codigo_operacion operacion, uint32_t* tamanio_paquete);
uint32_t serializar_paquete_iniciar_patota(t_paquete* paquete, t_iniciar_patota* mensaje);
uint32_t serializar_respuesta_iniciar_patota(t_paquete* paquete, t_respuesta* mensaje);
uint32_t serializar_paquete_tripulante(t_paquete* paquete, t_tcb* mensaje);
uint32_t serializar_paquete_id_tripulante(t_paquete* paquete, t_id_tripulante* mensaje);


// Desserializaciones (para recibir un mensaje)
void deserializar_iniciar_patota(t_iniciar_patota* mensaje, int32_t conexion);
void deserializar_respuesta_patota(t_respuesta* mensaje, int32_t conexion);
void deserealizar_tripulante(t_tcb* mensaje, int32_t conexion);
void deseralizar_id_tripulante(t_id_tripulante* mensaje, int32_t conexion);
void deserealizar_sabotaje(t_respuesta_mongo* mensaje,int32_t conexion);
>>>>>>> planificacion
#endif /* UTILS_SOCKETS_H_ */

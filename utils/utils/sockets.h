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




t_log* logger;
t_config* config;

// Conexiones
int32_t crear_conexion(char* ip, char* puerto);
void cerrar_conexion(int32_t socket);
int32_t iniciar_servidor(char* IP, char* PUERTO);


void enviar_mensaje(void* mensaje, codigo_operacion operacion, int32_t conexion);
void recibir_mensaje(void* mensaje, codigo_operacion operacion, int32_t conexion);


void crear_buffer(t_paquete* paquete);
//void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
//void enviar_paquete(t_paquete* paquete, int socket_cliente);
void eliminar_paquete(t_paquete* paquete);


int32_t* esperar_conexion(int32_t conexion_servidor);
void recibir_operacion(int32_t socket_cliente, codigo_operacion operacion);
void* recibir_buffer(uint32_t* size, int32_t conexion_cliente);
//t_list* recibir_paquete(int socket_cliente);


// Serializaciones (para enviar un mensaje)
void* serializar_paquete(t_paquete* paquete, void* mensaje, codigo_operacion operacion, uint32_t* tamanio_paquete);
uint32_t serializar_paquete_iniciar_patota(t_paquete* paquete, t_patota* mensaje);
uint32_t serializar_paquete_tripulante(t_paquete* paquete, t_tcb* mensaje);
uint32_t serializar_paquete_id_tripulante(t_paquete* paquete, t_id_tripulante* mensaje);


// Desserializaciones (para recibir un mensaje)
void deserializar_iniciar_patota(t_patota* mensaje, int32_t conexion);
void deserealizar_tripulante(t_tcb* mensaje, int32_t conexion);
void deseralizar_id_tripulante(t_id_tripulante* mensaje, int32_t conexion);

#endif /* UTILS_SOCKETS_H_ */

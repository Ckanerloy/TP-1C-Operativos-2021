#ifndef UTILSMONGO_STORE_H_
#define UTILSMONGO_STORE_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<string.h>


#define IP "127.0.0.1"
#define PUERTO "5002"


typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

t_log* logger;
t_config* config;

t_config* crear_config(void);

char* obtenerPuerto(void);
//int iniciar_servidor(char* PUERTO);
int iniciar_servidor(void);
int esperar_cliente(int socket_servidor);
int recibir_operacion(int socket_cliente);
void* recibir_buffer(int* size, int socket_cliente);
void recibir_mensaje(int socket_cliente);
t_list* recibir_paquete(int socket_cliente);

#endif /* UTILSMONGO_STORE_H_ */

#ifndef UTILSMIRAM_H_
#define UTILSMIRAM_H_

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
#define CONFIG_PATH "/home/utnso/tp-2021-1c-UTNIX/Mi-RAM-HQ/Mi-RAM.config"

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;


// Patota Control Block (PCB)
typedef struct {
	uint32_t id_patota;
	void* direccion_tareas;
}pcb;


// Tripulante Control Block (TCB)
typedef struct {
	uint32_t id_tripulante;
	char estado_tripulante;
	uint32_t posicion_x;
	uint32_t posicion_y;
	uint32_t id_proxima_instruccion;
}tcb;


t_log* logger;
t_config* config;

char* obtenerPuerto(void);
int iniciar_servidor(char* PUERTO);
int esperar_cliente(int socket_servidor);
int recibir_operacion(int socket_cliente);
void* recibir_buffer(int* size, int socket_cliente);
void recibir_mensaje(int socket_cliente);
t_list* recibir_paquete(int socket_cliente);

#endif /* UTILSMIRAM_H_ */

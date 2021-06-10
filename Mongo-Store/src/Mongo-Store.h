#ifndef MONGO_STORE_H_
#define MONGO_STORE_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <pthread.h>
#include <semaphore.h>

#include "utils/sockets.h"
#include "utils/loader.h"
#include "utils/estructuras.h"

#define IP "127.0.0.1"
#define CONFIG_PATH "/home/utnso/tp-2021-1c-UTNIX/Mongo-Store/Mongo-Store.config"

// Datos del Config
char* PUERTO;
char* PUNTO_MONTAJE;
int TIEMPO_SINCRONIZACION;
char** POSICIONES_SABOTAJE;

t_log* logger;
t_config* config;

pthread_t hilo_recibir_mensajes;

void obtener_datos_de_config(t_config* config);
void procesar_mensajes(codigo_operacion operacion, int32_t conexion);

#endif /* MONGO_STORE_H_ */

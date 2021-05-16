#ifndef DISCORDIADOR_H_
#define DISCORDIADOR_H_

#include <stdio.h>
#include <stdlib.h>
#include "commons/log.h"
#include "commons/string.h"
#include "commons/config.h"
#include <readline/readline.h>
#include <pthread.h>
#include <semaphore.h>

#include "utils/sockets.h"
#include "utilsDiscordiador.h"

#define CONFIG_PATH "/home/utnso/tp-2021-1c-UTNIX/Discordiador/discordiador.config"

char** parser_consola;
uint32_t operacion;

// Conexiones
char* IP_MI_RAM;
char* PUERTO_MI_RAM;
char* IP_MONGO_STORE;
char* PUERTO_MONGO_STORE;
int32_t conexion_mongo_store;
int32_t conexion_mi_ram;

// Data del Config
int GRADO_MULTITAREA;
char* ALGORITMO;
int QUANTUM;
int DURACION_SABOTAJE;
int RETARDO_CICLO_CPU;

//int32_t conexion_socket;
int32_t conexion_sabotaje;

// Semaforos
sem_t* comando_para_ejecutar;
sem_t* sabotaje;

// Hilos
pthread_t hilo_consola;
pthread_t hilo_sabotaje;

t_config* config;
t_log* logger;

t_log* iniciar_logger();
void obtener_datos_de_config(char*);
void obtener_orden_input(char*);
void estar_atento_por_sabotaje(void);
t_paquete* armar_paquete(void);
int cantidadStringsIngresados(char** );

#endif /* DISCORDIADOR_H_ */

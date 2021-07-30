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
#include "utils/loader.h"
#include "utilsDiscordiador.h"
#include "planificador.h"
#include "utils/estructuras.h"

#define CONFIG_PATH "/home/utnso/tp-2021-1c-UTNIX/Discordiador/Discordiador.config"

char** parser_consola;
char** parser_posiciones;
uint32_t operacion;

// Conexiones
char* IP_DISCORDIADOR;
char* PUERTO_DISCORDIADOR;
char* IP_MI_RAM;
char* PUERTO_MI_RAM;
char* IP_MONGO_STORE;
char* PUERTO_MONGO_STORE;

int32_t conexion_mongo_store;
int32_t conexion_mi_ram;
int32_t conexion_sabotaje;

// Data del Config
//int GRADO_MULTITAREA;
//char* ALGORITMO;
//int QUANTUM;
int DURACION_SABOTAJE;
int RETARDO_CICLO_CPU;


// Semaforos
sem_t* comando_para_ejecutar;
sem_t* mutex_sabotaje;
sem_t* termino_operacion;
sem_t* finalizar_programa;
sem_t* termine_sabotaje;

// Hilos
pthread_t hilo_consola;
pthread_t hilo_sabotaje;
pthread_t hilo_tripulante;
pthread_t hilo_new_ready;
pthread_t hilo_ready_running;
pthread_t hilo_solucion;   //preguntar
pthread_t hilo_creador_rafagas;
pthread_t hilo_susp_block;

pthread_t hilo_tripulante_sabo;

t_log* logger;
t_log* logger_on;
t_config* config;

//posiciones* posicion_sabotaje;

//void iniciar_escucha_sabotaje(void);
void iniciar_escucha_por_consola();
void obtener_datos_de_config(t_config* config);
void obtener_orden_input();						// Leo lo que escriba por consola y ejecuta la operacion a realizar
void crear_hilos();
void iniciar_escucha_sabotaje();
void inicializar_semaforos();


// Sabotaje
void iniciar_escucha_sabotaje(void);
void procesar_mensajes(codigo_operacion operacion, int32_t conexion);


tripulante_plani* mas_cercano(tripulante_plani* tripulante1, tripulante_plani* tripulante2);

bool menorId(tripulante_plani* tripulante1, tripulante_plani* tripulante2);
// POR AHORA SON UNA IDEA


int obtener_indice(t_list* lista, void* valor);

t_pcb* crear_pcb(void);

// Colas de planificacion
//t_queue* colaNew;

#endif /* DISCORDIADOR_H_ */

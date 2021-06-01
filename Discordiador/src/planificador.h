#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/collections/queue.h>
#include <pthread.h>
#include <semaphore.h>

#include "commons/log.h"
#include "commons/string.h"
#include "commons/config.h"

#include "utils/estructuras.h"

typedef enum{
	FIFO,
	RR
}algoritmo_planificacion;

//Colas
t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_block;
t_queue* cola_suspendido;

//Semaforos
sem_t* sem_ready;
sem_t* mutex_ready;
sem_t* contador_tripulantes_en_new;
sem_t* mutex_new;
sem_t* planificacion_on;

//Hilos
//pthread_t hilo_new_ready;			Por que se pone aca, pero tambien esta en planificador.c?
//pthread_t hilo_block_ready;

// Datos del Config
int GRADO_MULTITAREA;
char* ALGORITMO;
int QUANTUM;

algoritmo_planificacion mapeo_algoritmo_planificacion(char* algoritmo);
void elegir_algoritmo();
void obtener_planificacion_de_config(t_config* config);
void new_ready();
void iniciar_planificacion();
void inicializar_semaforos();

#endif /* PLANIFICADOR_H_ */

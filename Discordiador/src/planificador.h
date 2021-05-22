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


t_queue* cola_new;
t_list* cola_ready;
t_list* cola_block;

sem_t* sem_ready;


typedef enum
{
	FIFO,
	RR
} algoritmo_planificacion;




// Datos del Config
int GRADO_MULTITAREA;
char* ALGORITMO;
int QUANTUM;

algoritmo_planificacion mapeo_algoritmo_planificacion(char* algoritmo);
void elegir_algoritmo(void);

void obtener_planificacion_de_config(t_config* config);

#endif /* PLANIFICADOR_H_ */

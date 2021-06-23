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

uint32_t multitarea_Disponible;


typedef enum{
	FIFO,
	RR
}algoritmo_planificacion;

//Colas
t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_exit;

//Lista de los semaforos de los tripulantes
t_list* lista_semaforos_tripulantes;
t_list* lista_tripulantes;


t_list* bloquedo_suspendido;
//Semaforos

sem_t* mutex_ready;
sem_t* mutex_exit;
sem_t* mutex_new;


sem_t* planificacion_on;
sem_t* planificacion_on_ready_running;

sem_t* mutex_valorMultitarea;

sem_t* contador_tripulantes_en_ready;
sem_t* contador_tripulantes_en_new;

//Hilos
//pthread_t hilo_new_ready;			Por que se pone aca, pero tambien esta en planificador.c?
//pthread_t hilo_block_ready;



int valor_sabotaje;


// Datos del Config
int GRADO_MULTITAREA;
char* ALGORITMO;
int QUANTUM;
int RETARDO_CICLO_CPU;

algoritmo_planificacion algoritmo_elegido;
algoritmo_planificacion mapeo_algoritmo_planificacion(char* algoritmo);
void elegir_algoritmo();
void obtener_planificacion_de_config(t_config* config);
void new_ready();
void ready_running();
void iniciar_planificacion();
void inicializar_semaforos();
void inicializar_listas();
void pulso_rafaga();
void running_ready(tripulante_plani* tripulante);
void tripulante_hilo(void* tripulante);
t_tarea* obtener_siguiente_tarea(uint32_t numero_patota);
posiciones* obtener_posiciones(uint32_t tripulante);
uint32_t obtener_distancia(posiciones* posicion_tripu, posiciones* posicion_tarea);

//Pasar a utils O REVISAR
t_tarea* obtener_la_tarea(char* tarea_tripulante);
void realizar_tarea(tripulante_plani* tripulante,uint32_t* cantidadRealizado);
void generar_insumo(char* nombre_archivo, char caracter_llenado,tripulante_plani* tripu);
void consumir_insumo(char* nombre_archivo, char caracter_a_consumir,tripulante_plani* tripu);
void descartar_basura(tripulante_plani* tripu);
void otras_tareas(tripulante_plani* tripu,uint32_t* cantidadRealizado);

#endif /* PLANIFICADOR_H_ */

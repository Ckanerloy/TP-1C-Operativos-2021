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

#include "Discordiador.h"
#include "utils/estructuras.h"
#include "utils/tareas.h"

typedef enum{
	FIFO,
	RR
}algoritmo_planificacion;

//Colas
t_queue* cola_new;
t_queue* cola_ready;
t_queue* cola_exit;
t_queue* cola_auxiliar_sabotaje;

//Lista de los semaforos de los tripulantes
t_list* lista_semaforos_tripulantes;
t_list* lista_tripulantes;

t_list* bloqueado_suspendido;
t_list* bloqueado_suspendido_ready;

t_list* tripulantes_exec_block;

//Semaforos

sem_t* mutex_ready;
sem_t* mutex_exit;
sem_t* mutex_new;
//sem_t* mutex_expulsado;
sem_t* mutex_ready_running;
sem_t* mutex_new_ready;
sem_t* mutex_rafaga;

sem_t* planificacion_on;
sem_t* planificacion_on_ready_running;
sem_t* planificion_rafaga;

sem_t* multitarea_disponible;

sem_t* contador_tripulantes_en_ready;
sem_t* contador_tripulantes_en_new;


//Hilos
//pthread_t hilo_new_ready;			Por que se pone aca, pero tambien esta en planificador.c?
//pthread_t hilo_block_ready;



int valor_sabotaje;
uint32_t new_ready_off;
uint32_t ready_running_off;
uint32_t dar_pulsos_off;

// Datos del Config
int GRADO_MULTITAREA;
char* ALGORITMO;
int QUANTUM;
int RETARDO_CICLO_CPU;

algoritmo_planificacion algoritmo_elegido;
algoritmo_planificacion mapeo_algoritmo_planificacion(char* algoritmo);
void elegir_algoritmo(void);
void obtener_planificacion_de_config(t_config* config);
void new_ready();
void ready_running();
void iniciar_planificacion();
void ready_exit(tripulante_plani* tripu);
void ready_suspendido(tripulante_plani* tripu);
void running_suspendido(tripulante_plani* tripu);

void inicializar_semaforos_plani();
void finalizar_semaforos_plani();

void inicializar_listas();
void pulso_rafaga();
void running_ready(tripulante_plani* tripulante);
void block_exit(tripulante_plani* tripu);
void new_exit(tripulante_plani* tripu);
void running_exit(tripulante_plani* tripu);
void tripulante_hilo(void* tripulante);
void rafaga_cpu(t_list* lista_todos_tripulantes);
bool esta_exec_o_block(void* tripulante);
void poner_en_uno_semaforo(tripulante_plani* tripulante);

void suspendido_ready(tripulante_plani* tripu);
void actualizar_estado(tripulante_plani* tripu, char estado);
t_tarea* obtener_siguiente_tarea(uint32_t id_tripulante, uint32_t numero_patota);
posiciones* obtener_posiciones(uint32_t id_tripulante,uint32_t id_patota);
uint32_t obtener_distancia(posiciones* posicion_tripu, posiciones* posicion_tarea);


void realizar_tarea(tripulante_plani* tripulante);
void generar_insumo(char* nombre_archivo, char caracter_llenado,tripulante_plani* tripu);
void consumir_insumo(char* nombre_archivo, char caracter_a_consumir,tripulante_plani* tripu);
void descartar_basura(tripulante_plani* tripu);
void otras_tareas(tripulante_plani* tripu);
void realizar_tarea_sabotaje(tripulante_plani* tripu);
void cambios_de_tarea(tripulante_plani* tripu);


posiciones* obtener_nueva_posicion(posiciones* posicion_tripu, posiciones* posicion_tarea, tripulante_plani* tripu);

void actualizar_posicion(tripulante_plani* tripu, posiciones* nuevaPosicion);


#endif /* PLANIFICADOR_H_ */

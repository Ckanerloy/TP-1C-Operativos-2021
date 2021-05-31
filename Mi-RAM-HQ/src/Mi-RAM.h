#ifndef MI_RAM_H_
#define MI_RAM_H_

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
#include "tareas.h"
#include "memoria.h"

#define IP "127.0.0.1"
#define CONFIG_PATH "/home/utnso/tp-2021-1c-UTNIX/Mi-RAM-HQ/Mi-RAM.config"

// Config y Log
t_config* config;
t_log* logger;

// Datos del Config
char* PUERTO;
uint32_t TAMANIO_MEMORIA;
char* ESQUEMA_MEMORIA;
uint32_t TAMANIO_PAGINA;
uint32_t TAMANIO_SWAP;
char* PATH_SWAP;
char* ALGORITMO_REEMPLAZO;
char* CRITERIO_SELECCION;


sem_t* espera;

pthread_t hilo_recibir_mensajes;



void iniciar_comunicacion(void);
void obtener_datos_de_config(t_config* config);
void procesar_mensajes(codigo_operacion operacion, int32_t conexion);


t_list* ids;
uint32_t cantidad_tareas(char** parser_tarea);
char** parser_posiciones;
uint32_t contador_id_tripu;
uint32_t contador_id_patota;
t_pcb* crear_pcb(void);
t_tcb* crear_tcb(uint32_t dir_logica_pcb, uint32_t posicion_x,uint32_t posicion_y,uint32_t dir_logica_prox_instruc);

void mostrar_tripulante(t_tcb* tripulante);

#endif /* MI_RAM_H_ */

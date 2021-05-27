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

#define IP "127.0.0.1"
#define CONFIG_PATH "/home/utnso/tp-2021-1c-UTNIX/Mi-RAM-HQ/Mi-RAM.config"

// Datos del Config
char* PUERTO;
int TAMANIO_MEMORIA;
char* ESQUEMA_MEMORIA;
int TAMANIO_PAGINA;
int TAMANIO_SWAP;
char* PATH_SWAP;
char* ALGORITMO_REEMPLAZO;


sem_t* espera;

pthread_t hilo_recibir_mensajes;

void obtener_datos_de_config(t_config* config);

void procesar_mensajes(codigo_operacion operacion, int32_t conexion);

t_tcb* crear_tripulante(int pos_x, int pos_y, t_pcb* pcb_patota);

t_tcb* crear_tcbs(t_pcb* pcb_patota, t_iniciar_patota* patota_recibida);

//t_pcb* crear_pcb(void);

void mostrar_tripulante(t_tcb* tripulante);

#endif /* MI_RAM_H_ */

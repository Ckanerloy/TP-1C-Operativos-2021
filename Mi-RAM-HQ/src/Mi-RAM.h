#ifndef MI_RAM_H_
#define MI_RAM_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/queue.h>
#include "utils/sockets.h"
#include "utils/loader.h"
#include "utils/estructuras.h"
#include "utils/tareas.h"
#include "dump_memoria.h"
#include "segmentacion.h"
#include "paginacion.h"

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

void* memoria_principal;

// Elección de algoritmos
char esquema_elegido;
criterio_seleccion criterio_elegido;
algoritmo_reemplazo algoritmo_elegido;

// Contadores
uint32_t contador_segmento;
uint32_t contador_id_tripu;
uint32_t contador_id_patota;

// Indicadores de Memoria
uint32_t base_segmento;
int32_t memoria_restante;
int32_t memoria_libre_por_segmento;
int32_t memoria_libre_total;				// memoria_compactada = memoria_restante + memoria_libre_por_segmento;

// Mapa
//NIVEL* amongOs;
int columnas, filas;

// Tamanio de estructuras utilizadas
uint32_t tamanio_tripulante;
uint32_t tamanio_tripulantes;
uint32_t tamanio_patota;
uint32_t tamanio_tareas;

sem_t* crear_segmento_sem;
sem_t* espera;

pthread_t hilo_recibir_mensajes;
pthread_t hilo_actualizar_estado;

// Datos de tripulantes
t_list* ids;
char** parser_posiciones;

// Eleccion de algoritmos para la memoria
void elegir_esquema_de_memoria(char* ESQUEMA);
criterio_seleccion elegir_criterio_seleccion(char* criterio);
algoritmo_reemplazo elegir_algoritmo_reemplazo(char* algoritmo);

// Inicio de Mi-RAM HQ
void obtener_datos_de_config(t_config* config);
void iniciar_variables_y_semaforos(void);
void inicializar_memoria(void);
void iniciar_mapa(void);
void iniciar_comunicacion(void);
void procesar_mensajes(codigo_operacion operacion, int32_t conexion);

// Validación de espacio por esquema de memoria
bool validar_espacio_por_patota_segmentacion(uint32_t tamanio);
bool validar_espacio_por_patota_paginacion(uint32_t tamanio);
void chequear_memoria(void);

// Otras funciones
uint32_t cantidad_tareas(char** parser_tarea);
t_pcb* crear_pcb(void);
t_tcb* crear_tcb(uint32_t dir_logica_pcb, uint32_t posicion_x, uint32_t posicion_y);

#endif /* MI_RAM_H_ */

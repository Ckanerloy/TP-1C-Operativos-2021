#ifndef MONGO_STORE_H_
#define MONGO_STORE_H_
#define _GNU_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <limits.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>


#include "utils/sockets.h"
#include "utils/loader.h"
#include "utils/estructuras.h"
#include "utils/tareas.h"
#include "bitArray.h"
#include "commons/bitarray.h"
#include "iniciarFileSystem.h"
#include "sabotajes.h"

#define IP "127.0.0.1"
#define CONFIG_PATH "/home/utnso/tp-2021-1c-UTNIX/Mongo-Store/Mongo-Store.config"

// Datos del Config
char* PUERTO;
char* PUERTO_DISCORDIADOR;
char* PUNTO_MONTAJE;
int TIEMPO_SINCRONIZACION;
int BLOCK_SIZE;
int BLOCKS;
void* blocks;
int archivo_blocks;
char* bitmap;
t_bitarray* bitArraySB;
int archivo;
void *super_bloque;
void* informacion_blocks;
void* informacion_superBloque;
uint32_t tamanio_bloque_en_SB;
uint32_t cantidad_bloques_en_SB;
int32_t contador_tripulantes;

t_log* logger;
t_config* config;

typedef struct{
	char** bloques_asignados_anterior;
	int size;
}t_metadata;

//semaforos
sem_t* mutex_blocks;
sem_t* mutex_config;
sem_t* mutex_bitacora;
sem_t* mutex_copia;
sem_t* mutex_recursos;
sem_t* mutex_metadata;
sem_t* mutex_bitarray;

sem_t* sem_oxigeno;
sem_t* sem_comida;
sem_t* sem_basura;

pthread_t hilo_recibir_mensajes;
pthread_t hilo_sincronizador;


void obtener_datos_de_config(t_config* config);
void procesar_mensajes(codigo_operacion operacion, int32_t conexion);
void crear_archivo_metadata_recurso(char* nombre_archivo);
void crear_archivo_metadata_bitacora(char* nombre_archivo);

t_metadata* actualizar_archivo_metadata_bitacora(char* path, uint32_t tamanio_accion);

t_metadata* actualizar_archivo_metadata_recurso(char* path, char caracter_llenado, int32_t tamanio_recurso, char* nombre_recurso);

t_list* obtener_array_bloques_a_usar(uint32_t tamanio_a_guardar);
int obtener_bloque_libre(void);
int32_t cantidad_bloques_a_usar(int32_t tamanio_a_guardar);
void sincronizar();
char* hash_MD5(char* cadena_a_hashear, char* nombre_archivo);
uint32_t cantidad_elementos(char** parser);


// Funciones sobre Bloques
void copiar_en_memoria_recurso(int nro_bloque, char* caracter_a_guardar, int cantidad);
void guardar_en_blocks_bitacora(char* path_completo, char* valor, t_metadata* metadata_bitacora);
void guardar_en_blocks_recursos(char* path_completo, char caracter_llenado, char* nombre_recurso);
void eliminar_en_blocks(char* path_completo, char* valor, t_metadata* metadata_bitacora);
void eliminar_cantidad_recurso(t_metadata* metadata_recurso, uint32_t cantidad_a_eliminar);
void eliminar_recurso_blocks(char* path_completo, t_metadata* metadata_recurso);
char* concatenar_contenido_blocks(char** lista_bloques);


// Funciones sobre Archivos
int leer_size_archivo(char* path_archivo, char* clave);
char** leer_blocks_archivo(char* path_archivo, char* clave);
char* leer_caracter_archivo(char* path_archivo, char* clave);
void guardar_nuevos_datos_en_archivo(char* path_archivo, void* valor, char* clave);

//funcion semaforos

void activar_semaforo_recurso(char* recurso);
void liberar_semaforo_recurso(char* recurso);
void inicializar_semaforos(void);
void semaforo_recurso(recursos_archivos recurso, void(*funcion)(void*));

recursos_archivos mapeo_string_a_recurso(char* recurso);
char* armar_recurso(char caracter_llenado, uint32_t cantidad);
void loggear_liberacion_archivo(char* nombre, int nro_bloque);
char* crear_ruta_recurso(char* nombre_recurso);
char* crear_ruta_bitacora(int32_t id_tripulante);
bool existe_archivo(char* path);

#endif /* MONGO_STORE_H_ */

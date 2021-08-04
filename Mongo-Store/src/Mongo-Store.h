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

t_log* logger;
t_config* config;

typedef struct{
	char** bloques_asignados_anterior;
	int size;
}t_metadata;


//semaforos
sem_t* mutex_recursos;

pthread_t hilo_recibir_mensajes;
pthread_t hilo_sincronizador;


void obtener_datos_de_config(t_config* config);
void procesar_mensajes(codigo_operacion operacion, int32_t conexion);
void crear_archivo_metadata_recurso(char* nombre_archivo);
void crear_archivo_metadata_bitacora(char* nombre_archivo);

t_metadata* actualizar_archivo_metadata_bitacora(char* path, uint32_t tamanio_accion);

t_metadata* actualizar_archivo_metadata_recurso(char* path, archivo_tarea* tarea_io);

t_list* obtener_array_bloques_a_usar(uint32_t tamanio_a_guardar);
int32_t cantidad_bloques_a_usar(uint32_t tamanio_a_guardar);
void sincronizar();
void hash_MD5(char* cadena_a_hashear, char caracter_llenado);
uint32_t cantidad_elementos(char** parser);



// Funciones sobre Bloques
void guardar_en_blocks(char* path_completo, char* valor, t_metadata* metadata_bitacora);



// Funciones sobre Archivos
int leer_size_archivo(char* path_archivo, char* clave);
char** leer_blocks_archivo(char* path_archivo, char* clave);
void guardar_nuevo_size_archivo(char* path_archivo, int valor, char* clave);
//void guardar_nuevos_blocks_archivo(char* path_archivo, char* valor, char* clave);
void guardar_nuevos_datos_en_archivo(char* path_archivo, void* valor, char* clave);



char* armar_recurso(archivo_tarea* tarea_io);

#endif /* MONGO_STORE_H_ */

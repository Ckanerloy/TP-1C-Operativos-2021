#ifndef DUMP_MEMORIA_H_
#define DUMP_MEMORIA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/queue.h>

#include "segmentacion.h"


// Dump
typedef struct {
	char* path_dump;
	uint32_t tamanio_path;
	sem_t* sem_dump;
} archivo_dump;


// Dump de Memoria
archivo_dump* dump_memoria;

void iniciar_archivo_dump(void);
void iniciar_dump_memoria(void);
void registrar_dump_segmentacion(void);
void registrar_dump_paginacion(void);

void escribir_en_archivo(char* buffer);

#endif /* DUMP_MEMORIA_H_ */

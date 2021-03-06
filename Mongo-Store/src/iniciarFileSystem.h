#ifndef SRC_INICIAR_FILE_SYSTEM_H_
#define SRC_INICIAR_FILE_SYSTEM_H_


#include "Mongo-Store.h"
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>

char *concatenar_path(char*);
void inicializar_file_system(void);
int existe_file_system(void);
void iniciar_superbloque(void);
void creacion_directorio(char* direccion_punto_montaje, char* nombre_directorio);
void crear_archivo_blocks(void);
void escribir_archivo_blocks(uint32_t , char* , uint32_t );
void levantar_archivo_blocks(void);
void levantar_archivo_superBloque(void);


typedef struct {
	uint32_t tamanioBloque;
	uint32_t cantidadBloques;
	t_bitarray* bitmap;
} bloque_t;





#endif /*SRC_INICIAR_FILE_SYSTEM_H_*/

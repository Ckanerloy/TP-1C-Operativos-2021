#ifndef SRC_INICIAR_FILE_SYSTEM_H_
#define SRC_INICIAR_FILE_SYSTEM_H_


#include "Mongo-Store.h"
	#include <stdbool.h>
	#include <limits.h>
	#include <unistd.h>

void inicializar_file_system();
int existeFileSystem();
void crear_superbloque();
void creacion_directorio(char* direccion_punto_montaje, char* nombre_directorio);

typedef struct {
	char *bitarray;
	size_t size;
	bit_numbering_t mode;
} t_bitarray;

typedef struct {
	uint32_t tamanioBloque;
	uint32_t cantidadBloques;
	t_bitarray* bitmap;
} bloque_t;





#endif /*SRC_INICIAR_FILE_SYSTEM_H_*/

#ifndef SABOTAJES_H_
#define SABOTAJES_H_

typedef enum{
	OXIGENO,
	COMIDA,
	BASURA
} recursos_archivos;

#include "Mongo-Store.h"

char** POSICIONES_SABOTAJE;
int32_t num_sabotaje;

void iniciar_sabotaje(void);
void inicio_protocolo_fsck(void);


t_list* recursos_activos(void);
char* mapeo_recurso_a_string(recursos_archivos recurso);
bool esta_presente_en_lista(t_list* lista, int valor);


// Protocolos del Sabotaje
bool sabotaje_superbloque_cantidad_bloques(void);
void reparacion_superBloque_cantidad_bloques(void);
bool sabotaje_superBloque_bitmap(void);
void reparacion_superBloque_bitmap(void);
t_list* obtener_blocks_ocupados_total(void);
bool mismo_size_archivo(recursos_archivos recurso);
void reparar_size(recursos_archivos recurso);
bool mismo_block_count_archivo(recursos_archivos recurso);
void reparar_block_count(recursos_archivos recurso);
bool bloques_ordenados_archivo(recursos_archivos recurso);
void reparar_orden_bloques(recursos_archivos recurso);
void agregar_bloque_faltante(char** bloques, char* path, char* caracter, int size);
void reordenar_bloques(char** bloques, char* path, char* caracter, int size);


//Funciones auxiliares
int calcular_tamanio_archivo(char* path_archivo);

#endif /* SABOTAJES_H_ */

#ifndef SABOTAJES_H_
#define SABOTAJES_H_

#include "Mongo-Store.h"

typedef enum{
	OXIGENO,
	COMIDA,
	BASURA
} recursos_archivos;


char** POSICIONES_SABOTAJE;
int32_t num_sabotaje;

void iniciar_sabotaje(void);
void inicio_protocolo_fsck(void);


t_list* recursos_activos(void);
char* mapeo_recurso_a_string(recursos_archivos recurso);

// Protocolos del Sabotaje
bool sabotaje_superbloque_cantidad_bloques(void);
void reparacion_superbloque_cantidad_bloques(void);
bool sabotaje_superbloque_bitmap(void);
void reparacion_superbloque_bitmap(void);

#endif /* SABOTAJES_H_ */

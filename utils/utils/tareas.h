#ifndef TAREAS_H_
#define TAREAS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

typedef enum
{
	GENERAR_OXIGENO,
	CONSUMIR_OXIGENO,
	GENERAR_COMIDA,
	CONSUMIR_COMIDA,
	GENERAR_BASURA,
	DESCARTAR_BASURA,
	REALIZAR_TAREA_SABOTAJE,
	TAREA_VACIA,
	CUALQUIER_TAREA
} codigo_tarea;


typedef enum
{
	SABOTAJE_SUPERBLOQUE,
	SABOTAJE_FILES
} codigo_sabotaje;



// cambiar a char*
typedef struct {
	codigo_tarea operacion;
	uint32_t cantidad;
	uint32_t posicion_x;
	uint32_t posicion_y;
	int32_t tiempo;
} t_tarea;







char** obtener_tareas(char* tareas_patota);
t_tarea* obtener_la_tarea(char* tarea_tripulante);
void limpiar_parser(char** parser);

#endif /* TAREAS_H_ */


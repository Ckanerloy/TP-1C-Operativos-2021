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
	MOVERSE
} codigo_tarea;


typedef struct {
	codigo_tarea operacion;
	uint32_t cantidad;
	uint32_t posicion_x;
	uint32_t posicion_y;
	int32_t tiempo;
} t_tarea;


typedef enum
{
	SABOTAJE_SUPERBLOQUE,
	SABOTAJE_FILES
} codigo_sabotaje;


typedef struct {
	codigo_sabotaje operacion;
	int32_t tiempo;
	uint32_t posicion_x;
	uint32_t posicion_y;

} t_sabotaje;



char** obtener_tareas(char* tareas_patota);
t_tarea* obtener_la_tarea(char* tarea_tripulante);


#endif /* TAREAS_H_ */


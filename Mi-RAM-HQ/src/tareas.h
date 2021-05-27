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

#include "utils/estructuras.h"


typedef struct{
	uint32_t cantidad;
	int posicion_x;
	int posicion_y;
	uint32_t tiempo;
} t_parametros_tarea;

typedef struct {
	codigo_tarea operacion;
	t_parametros_tarea* parametros;
} t_tarea;


char** obtener_tareas(char* tareas_patota);
t_parametros_tarea* recibir_parametros(char* parametros);

#endif /* TAREAS_H_ */

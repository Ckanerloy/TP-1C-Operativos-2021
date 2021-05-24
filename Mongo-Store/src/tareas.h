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

#endif /* TAREAS_H_ */

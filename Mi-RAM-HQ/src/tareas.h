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
#include "planificador.h"
#include "planificador.c"




char** obtener_tareas(char* tareas_patota);
//t_tarea* obtener_la_tarea(char* tarea_tripulante);
//t_parametros_tarea* recibir_parametros(char* parametros);
//void realizar_tarea(t_tarea* tarea, tripulante_plani tripulante);

#endif /* TAREAS_H_ */


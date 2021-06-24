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
	int32_t cantidad;
	uint32_t posicion_x;
	uint32_t posicion_y;
	int32_t tiempo;
} t_tarea;



char** obtener_tareas(char* tareas_patota);
t_tarea* obtener_la_tarea(char* tarea_tripulante);
//void realizar_tarea_de_io(codigo_tarea tarea, char* PARAMETROS);

//t_tarea* obtener_la_tarea(char* tarea_tripulante);
//t_parametros_tarea* recibir_parametros(char* parametros);
//void realizar_tarea(t_tarea* tarea, tripulante_plani tripulante);


#endif /* TAREAS_H_ */


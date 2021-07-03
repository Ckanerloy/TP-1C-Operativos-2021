#include "tareas.h"

codigo_tarea mapeo_tareas_tripulantes(char* tarea) {

	codigo_tarea tarea_a_realizar;

	if(strcmp(tarea, "GENERAR_OXIGENO") == 0) {
		tarea_a_realizar = GENERAR_OXIGENO;
	}

	if(strcmp(tarea, "CONSUMIR_OXIGENO") == 0) {
		tarea_a_realizar = CONSUMIR_OXIGENO;
	}

	if(strcmp(tarea, "GENERAR_COMIDA") == 0) {
		tarea_a_realizar = GENERAR_COMIDA;
	}

	if(strcmp(tarea, "CONSUMIR_COMIDA") == 0) {
		tarea_a_realizar = CONSUMIR_COMIDA;
	}

	if(strcmp(tarea, "GENERAR_BASURA") == 0) {
		tarea_a_realizar = GENERAR_BASURA;
	}

	if(strcmp(tarea, "DESCARTAR_BASURA") == 0) {
		tarea_a_realizar = DESCARTAR_BASURA;
	}

	return tarea_a_realizar;
}


char** obtener_tareas(char* tareas_patota) {
	return string_split(tareas_patota, "\n");
}


t_tarea* obtener_la_tarea(char* tarea_tripulante) {
	char** parser_tarea = string_split(tarea_tripulante, " ");

	char** parser_parametros = NULL;

	t_tarea* tarea_nueva = malloc(sizeof(t_tarea));

	tarea_nueva->operacion = mapeo_tareas_tripulantes(parser_tarea[0]);

	parser_parametros = string_split(parser_tarea[1], ";");

	tarea_nueva->cantidad = atoi(parser_parametros[0]);
	tarea_nueva->posicion_x = atoi(parser_parametros[1]);
	tarea_nueva->posicion_y = atoi(parser_parametros[2]);
	tarea_nueva->tiempo = atoi(parser_parametros[3]);

	free(parser_tarea);
	free(parser_parametros);
	return tarea_nueva;
}



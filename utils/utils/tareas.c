#include "tareas.h"

codigo_tarea mapeo_tareas_tripulantes(char* tarea) {

	codigo_tarea tarea_a_realizar;

	if(strcmp(tarea, "GENERAR_OXIGENO") == 0) {
		tarea_a_realizar = GENERAR_OXIGENO; // @suppress("Symbol is not resolved")
	}

	else if(strcmp(tarea, "CONSUMIR_OXIGENO") == 0) {
		tarea_a_realizar = CONSUMIR_OXIGENO;// @suppress("Symbol is not resolved")
	}

	else if(strcmp(tarea, "GENERAR_COMIDA") == 0) {
		tarea_a_realizar = GENERAR_COMIDA;// @suppress("Symbol is not resolved")
	}

	else if(strcmp(tarea, "CONSUMIR_COMIDA") == 0) {
		tarea_a_realizar = CONSUMIR_COMIDA;// @suppress("Symbol is not resolved")
	}

	else if(strcmp(tarea, "GENERAR_BASURA") == 0) {
		tarea_a_realizar = GENERAR_BASURA;// @suppress("Symbol is not resolved")
	}

	else if(strcmp(tarea, "DESCARTAR_BASURA") == 0) {
		tarea_a_realizar = DESCARTAR_BASURA;// @suppress("Symbol is not resolved")
	}

	else {
		tarea_a_realizar = CUALQUIER_TAREA;				// Aca entraria cualquier tarea que no este arriba
	}

	return tarea_a_realizar;
}


t_tarea* obtener_la_tarea(char* tarea_tripulante) {
	char** parser_tarea = string_split(tarea_tripulante, " ");

	// Para las Tareas de Entrada y Salida
	if(parser_tarea[1] != NULL) {
		t_tarea* tarea_nueva = malloc(sizeof(t_tarea));

		tarea_nueva->operacion = mapeo_tareas_tripulantes(parser_tarea[0]);

		char** parser_parametros = string_split(parser_tarea[1], ";");

		tarea_nueva->cantidad = atoi(parser_parametros[0]);
		tarea_nueva->posicion_x = atoi(parser_parametros[1]);
		tarea_nueva->posicion_y = atoi(parser_parametros[2]);
		tarea_nueva->tiempo = atoi(parser_parametros[3]);

		limpiar_parser(parser_tarea);
		limpiar_parser(parser_parametros);
		return tarea_nueva;
	}
	else {	// Para las Tareas comunes
		char** parser_tarea_comun = string_split(parser_tarea[0], ";");

		t_tarea* tarea_nueva = malloc(sizeof(t_tarea));

		tarea_nueva->operacion = mapeo_tareas_tripulantes(parser_tarea_comun[0]);

		tarea_nueva->cantidad = 0;
		tarea_nueva->posicion_x = atoi(parser_tarea_comun[1]);
		tarea_nueva->posicion_y = atoi(parser_tarea_comun[2]);
		tarea_nueva->tiempo = atoi(parser_tarea_comun[3]);

		limpiar_parser(parser_tarea);
		limpiar_parser(parser_tarea_comun);
		return tarea_nueva;
	}
}


void limpiar_parser(char** parser) {
	int limpiador = 0;
	while(parser[limpiador] != NULL) {
		free(parser[limpiador]);
		limpiador++;
	}
	free(parser);
}

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
	return string_split(tareas_patota, "|");
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


/*
t_parametros_tarea* recibir_parametros(char* parametros) {

	char** parser_parametros = NULL;

	t_parametros_tarea* estructura = malloc(sizeof(t_parametros_tarea));
	parser_parametros = string_split(parametros, ";");

	estructura->cantidad = atoi(parser_parametros[0]);
	estructura->posicion_x = atoi(parser_parametros[1]);
	estructura->posicion_y = atoi(parser_parametros[2]);
	estructura->tiempo = atoi(parser_parametros[3]);

	free(parser_parametros);
	return estructura;
}


void realizar_tarea(t_tarea* tarea, tripulante_plani tripulante){

	switch(tarea->operacion) {

		case GENERAR_OXIGENO:
			generar_insumo("Oxigeno.ims", 'O', tarea->parametros);
			break;

		case CONSUMIR_OXIGENO:
			consumir_insumo("Oxigeno.ims", 'O', tarea->parametros);
			break;

		case GENERAR_COMIDA:
			generar_insumo("Comida.ims", 'C', tarea->parametros);
			break;

		case CONSUMIR_COMIDA:
			consumir_insumo("Comida.ims",'C', tarea->parametros);
			break;

		case GENERAR_BASURA:
			generar_insumo("Basura.ims", 'B', tarea->parametros);
			break;

		case DESCARTAR_BASURA:
			descartar_basura();
			break;

		default:
			break;
		}
}


// Tareas de I/O del Tripulante
void generar_oxigeno(char* PARAMETROS) {

	t_parametros_tarea parametros = recibir_parametros(PARAMETROS);

	if(SI ESTA EL ARCHIVO) {
		modificar_archivo("Oxigeno.ims", parametros->cantidad);
	}
	else {
		crear_archivo("Oxigeno.ims", 'O');
	}
}


void generar_comida(char* PARAMETROS) {

	t_parametros_tarea parametros = recibir_parametros(PARAMETROS);

	if(SI ESTA EL ARCHIVO) {
		modificar_archivo("Comida.ims", parametros->cantidad);
	}
	else {
		crear_archivo("Comida.ims", 'C');
	}
}



void generar_insumo(char* nombre_archivo, char caracter_llenado, t_parametros_tarea* parametros,tripulante_plani tripulante) {

	sem_wait();
	//llamar al i-mongo y gastar 1 ciclo de cpu
	sleep(RETARDO_CICLO_CPU);
	running_block(tripulante->id_tripulante);
	sem_post();
	if(SI ESTA EL ARCHIVO) {
		modificar_archivo(nombre_archivo, parametros->cantidad);
	}
	else {
		crear_archivo(nombre_archivo, caracter_llenado);
	}

}

void consumir_insumo(char* nombre_archivo, char caracter_a_consumir, t_parametros_tarea* parametros) {

	if(SI ESTA EL ARCHIVO) {
		modificar_archivo(nombre_archivo, parametros->cantidad);
	}
	else {
		crear_archivo(nombre_archivo, caracter_a_consumir);
	}
}

void descartar_basura() {
	if(SI ESTA EL ARCHIVO) {
		eliminar_archivo("Basura.ims");
	}
	else {
		log_info(logger, "El archivo 'Basura.ims' no existe. \n");
	}
}
*/
//Las tareas que no se encuentren en este anexo consistirán en llegar a la ubicación y esperar que pase el tiempo solicitado en el estado EXEC
//y no tienen que hacer ninguna operación sobre archivos de recursos.

//Todas las listadas a continuación consumirán 1 ciclo de CPU para realizar la petición de E/S.


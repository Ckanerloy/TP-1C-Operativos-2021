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


t_parametros_tarea* recibir_parametros(char* parametros) {

	char ** parser_parametros;

	t_parametros_tarea* estructura = malloc(sizeof(t_parametros_tarea));
	parser_parametros = string_split(parametros, ";");

	estructura->cantidad = atoi(parser_parametros[0]);
	estructura->posicion_x = atoi(parser_parametros[1]);
	estructura->posicion_y = atoi(parser_parametros[2]);
	estructura->tiempo = atoi(parser_parametros[3]);

	free(parser_parametros);
	return estructura;
}


// Tareas a Realizar por el Tripulante
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

void realizar_tarea(codigo_tarea tarea, char* PARAMETROS){

	t_parametros_tarea* parametros = malloc(sizeof(t_parametros_tarea));

	parametros = recibir_parametros(PARAMETROS);
	switch(tarea) {

		case GENERAR_OXIGENO:
			generar_insumo("Oxigeno.ims", 'O', parametros);
			break;

		case CONSUMIR_OXIGENO:
			consumir_insumo("Oxigeno.ims", 'O', parametros);
			break;

		case GENERAR_COMIDA:
			generar_insumo("Comida.ims", 'C', parametros);
			break;

		case CONSUMIR_COMIDA:
			consumir_insumo("Comida.ims",'C', parametros);
			break;

		case GENERAR_BASURA:
			generar_insumo("Basura.ims", 'B', parametros);
			break;

		case DESCARTAR_BASURA:
			descartar_basura();
			break;

		default:
			break;
		}
}

void generar_insumo(char* nombre_archivo, char caracter_llenado, t_parametros_tarea* parametros) {

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


#include "memoria.h"


codigo_memoria mapeo_esquema_memoria(char* ESQUEMA)
{
	codigo_memoria esquema_memoria;

	if(strcmp(ESQUEMA, "PAGINACION") == 0) {
		esquema_memoria = PAGINACION;
	}

	if(strcmp(ESQUEMA, "SEGMENTACION") == 0) {
		esquema_memoria = SEGMENTACION;
	}

	return esquema_memoria;
}


void elegir_esquema_de_memoria(char* ESQUEMA)
{
	codigo_memoria cod_mem;

	cod_mem = mapeo_esquema_memoria(ESQUEMA);

	switch(cod_mem) {

		case PAGINACION:

			esquema_elegido = 'P';
			tabla_paginas = list_create();

			break;
		case SEGMENTACION:

			esquema_elegido = 'S';
			tabla_segmentos = list_create();

			break;
		default:
			break;
	}
}







void guardar_estructura(void* estructura)
{
	if(esquema_elegido == 'P') {

	}
	if(esquema_elegido == 'S') {
		//crear_segmento
	}
}



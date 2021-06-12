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
			pagina_patota = malloc(sizeof(t_paginas));
			inicializar_tabla_paginas_de_patota(pagina_patota);

			break;
		case SEGMENTACION:

			esquema_elegido = 'S';
			segmento_patota = malloc(sizeof(t_segmentos));
			inicializar_tabla_segmentos_de_patota(segmento_patota);

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



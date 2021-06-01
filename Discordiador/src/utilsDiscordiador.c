#include "utilsDiscordiador.h"

codigo_operacion mapeo_valor_consola(char* comando_ingresado) {

	codigo_operacion operacion;

	if(strcmp(comando_ingresado, "INICIAR_PLANIFICACION") == 0) {
		operacion = INICIAR_PLANIFICACION;
	}

	if(strcmp(comando_ingresado, "PAUSAR_PLANIFICACION") == 0) {
		operacion = PAUSAR_PLANIFICACION;
	}

	if(strcmp(comando_ingresado, "INICIAR_PATOTA") == 0) {
		operacion = INICIAR_PATOTA;
	}

	if(strcmp(comando_ingresado, "EXPULSAR_TRIPULANTE") == 0) {
		operacion = EXPULSAR_TRIPULANTE;
	}

	if(strcmp(comando_ingresado, "LISTAR_TRIPULANTES") == 0) {
		operacion = LISTAR_TRIPULANTES;
	}

	if(strcmp(comando_ingresado, "OBTENER_BITACORA") == 0) {
		operacion = OBTENER_BITACORA;
	}

	if(strcmp(comando_ingresado, "TERMINAR_PROGRAMA") == 0) {
		operacion = TERMINAR_PROGRAMA;
	}

	return operacion;
}

void leer_archivo_tareas(FILE* archivo_tareas, char* tareas) {
	char* tarea = string_new();

	while(!feof(archivo_tareas)){
		fscanf(archivo_tareas, "%s ", tarea);
		string_append_with_format(&tareas, "%s ", tarea);
	}
	printf("%s \n", tareas);
}


void mostrar_tripulante(t_tcb* tripulante) {

	printf("Id tripulante: %u \n", tripulante->id_tripulante);
	printf("Estado tripulante: %c \n", tripulante->estado_tripulante);
	printf("Posicion X: %i \n", tripulante->posicion_x);
	printf("Posicion Y: %i \n", tripulante->posicion_y);
	printf("Id proxima instruccion a realizar: %i \n\n", tripulante->id_proxima_instruccion);
}
/*
void liberar_tripulantes(uint32_t cantidad_tripulantes, t_tripulante** mensaje_tripulantes) {
	for(uint32_t c=1; c<=cantidad_tripulantes; c++){
		free(mensaje_tripulantes[c]);
	}
}*/

// Funcion para LISTAR TRIPULANTES
void listar_tripulantes() {

	printf("-------------------------------------------------------------------------\n");
	printf("Estado de la nave: %s \n", temporal_get_string_time("%d/%m/%y %H:%M:%S"));


	printf("Tripulante: ID_TRIPULANTE      Patota: NUM_PATOTA      Status:  STATUS \n");
	printf("--------------------------------------------------------------------------\n\n");
}



// Obtener la cantidad de argumentos de parte del parser_consola
uint32_t cantidad_argumentos_ingresados(char** parser_consola){  // la vamos a usar tanto para verifiaciones de ingresos
	int cantidad=0;									   			 // por consola, como para cantidad de tripu que arrancan en 0,0

	while(parser_consola[cantidad] != NULL){
		cantidad++;
	}
	return cantidad;
}

void procesar_mensajes(codigo_operacion operacion, int32_t conexion) {

}


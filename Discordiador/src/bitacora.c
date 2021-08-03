#include "bitacora.h"

void enviar_bitacora(bitacora* bitacora_tripu) {

	conexion_mongo_store = crear_conexion(IP_MONGO_STORE, PUERTO_MONGO_STORE);

	if(resultado_conexion(conexion_mongo_store, logger, "Mongo Store") == -1){
		log_error(logger_on, "No se pudo lograr la conexion con Mongo Store.\n");
		abort();
	}

	enviar_mensaje(bitacora_tripu, ACTUALIZACION_TRIPULANTE, conexion_mongo_store);

	close(conexion_mongo_store);
	free(bitacora_tripu);
}


void armar_bitacora(void* mensaje, codigo_bitacora codigo, uint32_t id_tripulante) {

	bitacora* bitacora_tripu;

	switch(codigo) {
		case MOVIMIENTO:
			bitacora_tripu = bitacora_movimiento(mensaje);
			break;

		case EJECUTA:
			bitacora_tripu = bitacora_ejecucion_tarea(mensaje);
			break;

		case TERMINA:
			bitacora_tripu = bitacora_termina_tarea(mensaje);
			break;

		case PANICO:
			bitacora_tripu = bitacora_corre_sabotaje(mensaje);
			break;

		case RESUELTO:
			bitacora_tripu = bitacora_resuelve_sabotaje(mensaje);
			break;

		default:
			break;
	}

	bitacora_tripu->id_tripulante = id_tripulante;

	enviar_bitacora(bitacora_tripu);
}


bitacora* bitacora_movimiento(bitacora_posiciones* posiciones) {

	bitacora* bitacora_tripu = malloc(sizeof(bitacora));

	char* mensaje = string_new();

	string_append_with_format(&mensaje, "Se mueve de %u|%u a %u|%u.", posiciones->posicion_anterior->posicion_x, posiciones->posicion_anterior->posicion_y, posiciones->posicion_nueva->posicion_x, posiciones->posicion_nueva->posicion_y);

	bitacora_tripu->tamanio_accion = strlen(mensaje);
	strcat(mensaje, "\0");
	bitacora_tripu->accion = malloc(bitacora_tripu->tamanio_accion+1);
	strcpy(bitacora_tripu->accion, mensaje);

	free(mensaje);
	return bitacora_tripu;
}


bitacora* bitacora_ejecucion_tarea(tripulante_plani* tripu) {

	bitacora* bitacora_tripu = malloc(sizeof(bitacora));

	char* mensaje = string_new();

	string_append_with_format(&mensaje, "Comienza ejecucion de tarea %u.", tripu->id_tarea_a_realizar);

	bitacora_tripu->tamanio_accion = strlen(mensaje);
	strcat(mensaje, "\0");
	bitacora_tripu->accion = malloc(bitacora_tripu->tamanio_accion+1);
	strcpy(bitacora_tripu->accion, mensaje);

	free(mensaje);
	return bitacora_tripu;
}


bitacora* bitacora_termina_tarea(tripulante_plani* tripu) {

	bitacora* bitacora_tripu = malloc(sizeof(bitacora));

	char* mensaje = string_new();

	string_append_with_format(&mensaje, "Se finaliza la tarea %u.", tripu->id_tarea_a_realizar);

	bitacora_tripu->tamanio_accion = strlen(mensaje);
	strcat(mensaje, "\0");
	bitacora_tripu->accion = malloc(bitacora_tripu->tamanio_accion+1);
	strcpy(bitacora_tripu->accion, mensaje);

	free(mensaje);
	return bitacora_tripu;
}

bitacora* bitacora_corre_sabotaje(char* valor) {

	bitacora* bitacora_tripu = malloc(sizeof(bitacora));

	char* mensaje = string_new();

	string_append_with_format(&mensaje, "Se corre en panico hacia la ubicacion del sabotaje.");

	bitacora_tripu->tamanio_accion = strlen(mensaje);
	strcat(mensaje, "\0");
	bitacora_tripu->accion = malloc(bitacora_tripu->tamanio_accion+1);
	strcpy(bitacora_tripu->accion, mensaje);

	free(mensaje);
	return bitacora_tripu;
}


bitacora* bitacora_resuelve_sabotaje(char* valor) {

	bitacora* bitacora_tripu = malloc(sizeof(bitacora));

	char* mensaje = string_new();

	string_append_with_format(&mensaje, "Se resuelve el sabotaje.");

	bitacora_tripu->tamanio_accion = strlen(mensaje);
	strcat(mensaje, "\0");
	bitacora_tripu->accion = malloc(bitacora_tripu->tamanio_accion+1);
	strcpy(bitacora_tripu->accion, mensaje);

	free(mensaje);
	return bitacora_tripu;
}






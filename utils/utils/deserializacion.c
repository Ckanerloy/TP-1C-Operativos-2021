#include "deserializacion.h"


void recibir_mensaje(void* mensaje, codigo_operacion operacion, int32_t conexion) {
	switch(operacion) {

		case INICIAR_PATOTA:
			deserializar_iniciar_patota(mensaje, conexion);
			break;

		case ACTUALIZAR_UBICACION_TRIPULANTE:
			deserializar_ubicacion_tripulante(mensaje, conexion);
			break;

		case PEDIR_UBICACION_TRIPULANTE:
			deserializar_tripulante(mensaje, conexion);
			break;

		case ACTUALIZAR_ESTADO_TRIPULANTE:
			deserializar_estado_tripulante(mensaje, conexion);
			break;

		case PEDIDO_TAREA:
			deserializar_tripulante(mensaje, conexion);
			break;

		case EXPULSAR_TRIPULANTE:
			deserializar_tripulante(mensaje, conexion);
			break;

		case OBTENER_BITACORA:
			deserializar_tripulante(mensaje, conexion);
			break;

		case RECIBIR_SABOTAJE:
			deserializar_sabotaje(mensaje,conexion);
			break;


		// Respuestas
		case RESPUESTA_INICIAR_PATOTA:
			deserializar_respuesta_patota(mensaje, conexion);
			break;

		case RESPUESTA_OK_UBICACION:
			deserializar_respuesta_tripulante(mensaje, conexion);
			break;

		case RESPUESTA_NUEVA_UBICACION:
			deserializar_respuesta_nueva_ubicacion(mensaje, conexion);
			break;

		case RESPUESTA_OK_ESTADO:
			deserializar_respuesta_tripulante(mensaje, conexion);
			break;

		case RESPUESTA_NUEVA_TAREA:
			deserializar_respuesta_nueva_tarea(mensaje, conexion);
			break;

		case RESPUESTA_TRIPULANTE_ELIMINADO:
			deserializar_respuesta_tripulante(mensaje, conexion);
			break;

		case CERRAR_MODULO:
			break;

		default:
			printf("404 operacion NOT FOUND.\n");
			break;

	}
}


void deserializar_iniciar_patota(t_iniciar_patota* mensaje, int32_t conexion) {
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	// Cantidad de Tripulantes
	memcpy(&(mensaje->cantidad_tripulantes), buffer_deserializar + desplazamiento, sizeof(mensaje->cantidad_tripulantes));
	desplazamiento += sizeof(mensaje->cantidad_tripulantes);

	// Tamanio de archivo de tareas
	memcpy(&(mensaje->tamanio_tareas), buffer_deserializar + desplazamiento, sizeof(mensaje->tamanio_tareas));
	desplazamiento += sizeof(mensaje->tamanio_tareas);

	mensaje->tareas_de_patota = malloc(mensaje->tamanio_tareas+1);

	// Archivo de tareas
	memcpy(mensaje->tareas_de_patota, buffer_deserializar + desplazamiento, mensaje->tamanio_tareas +1);
	desplazamiento += mensaje->tamanio_tareas+1;

	// Tamanio de posiciones
	memcpy(&(mensaje->tamanio_posiciones), buffer_deserializar + desplazamiento, sizeof(mensaje->tamanio_posiciones));
	desplazamiento += sizeof(mensaje->tamanio_posiciones);

	mensaje->posiciones = malloc(mensaje->tamanio_posiciones+1);

	// Posiciones de los tripulantes
	memcpy(mensaje->posiciones, buffer_deserializar + desplazamiento, mensaje->tamanio_posiciones+1);
	desplazamiento += mensaje->tamanio_posiciones+1;

	free(buffer_deserializar);
}


void deserializar_tripulante(t_tripulante* mensaje, int32_t conexion) {
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	// ID de Tripulante
	memcpy(&(mensaje->id_tripulante), buffer_deserializar + desplazamiento, sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	// ID de Patota
	memcpy(&(mensaje->id_patota), buffer_deserializar + desplazamiento, sizeof(mensaje->id_patota));
	desplazamiento += sizeof(mensaje->id_patota);

	free(buffer_deserializar);
}


void deserializar_ubicacion_tripulante(t_tripulante_ubicacion* mensaje, int32_t conexion) {
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	// ID de Tripulante
	memcpy(&(mensaje->id_tripulante), buffer_deserializar + desplazamiento, sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	// ID de Patota
	memcpy(&(mensaje->id_patota), buffer_deserializar + desplazamiento, sizeof(mensaje->id_patota));
	desplazamiento += sizeof(mensaje->id_patota);

	// Posicion X
	memcpy(&(mensaje->posicion_x), buffer_deserializar + desplazamiento, sizeof(mensaje->posicion_x));
	desplazamiento += sizeof(mensaje->posicion_x);

	// Posicion Y
	memcpy(&(mensaje->posicion_y), buffer_deserializar + desplazamiento, sizeof(mensaje->posicion_y));
	desplazamiento += sizeof(mensaje->posicion_y);

	free(buffer_deserializar);
}


void deserializar_estado_tripulante(t_tripulante_estado* mensaje, int32_t conexion) {
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	// ID de Tripulante
	memcpy(&(mensaje->id_tripulante), buffer_deserializar + desplazamiento, sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	// ID de Patota
	memcpy(&(mensaje->id_patota), buffer_deserializar + desplazamiento, sizeof(mensaje->id_patota));
	desplazamiento += sizeof(mensaje->id_patota);

	// Estado tripulante
	memcpy(&(mensaje->estado), buffer_deserializar + desplazamiento, sizeof(mensaje->estado));
	desplazamiento += sizeof(mensaje->estado);

	free(buffer_deserializar);
}


void deserializar_sabotaje(t_respuesta_mongo* mensaje,int32_t conexion) {
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	memcpy(&(mensaje->sabotaje_on), buffer_deserializar + desplazamiento, sizeof(mensaje->sabotaje_on));
	desplazamiento += sizeof(mensaje->sabotaje_on);

	// FALTARIA AGREGAR LAS POSICIONES

	free(buffer_deserializar);
}


// Respuestas
void deserializar_respuesta_patota(t_respuesta_iniciar_patota* mensaje, int32_t conexion) {
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	// Respuesta
	memcpy(&(mensaje->respuesta), buffer_deserializar + desplazamiento, sizeof(mensaje->respuesta));
	desplazamiento += sizeof(mensaje->respuesta);

	// Tamanio ids tripulantes
	memcpy(&(mensaje->tamanio_ids), buffer_deserializar + desplazamiento, sizeof(mensaje->tamanio_ids));
	desplazamiento += sizeof(mensaje->tamanio_ids);

	mensaje->ids_tripu=malloc(mensaje->tamanio_ids+1);

	// Ids de tripulantes
	memcpy(mensaje->ids_tripu, buffer_deserializar + desplazamiento, mensaje->tamanio_ids+1);
	desplazamiento += mensaje->tamanio_ids+1;

	// Numero o ID de patota
	memcpy(&(mensaje->numero_de_patota), buffer_deserializar + desplazamiento, sizeof(mensaje->numero_de_patota));
	desplazamiento += sizeof(mensaje->numero_de_patota);

	free(buffer_deserializar);
}


void deserializar_respuesta_tripulante(t_respuesta_tripulante* mensaje, int32_t conexion) {
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	// Respuesta
	memcpy(&(mensaje->respuesta), buffer_deserializar + desplazamiento, sizeof(mensaje->respuesta));
	desplazamiento += sizeof(mensaje->respuesta);

	// ID de tripulante
	memcpy(&(mensaje->id_tripulante), buffer_deserializar + desplazamiento, sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	free(buffer_deserializar);
}


void deserializar_respuesta_nueva_ubicacion(t_respuesta_tripulante_ubicacion* mensaje, int32_t conexion) {
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	// Respuesta
	memcpy(&(mensaje->respuesta), buffer_deserializar + desplazamiento, sizeof(mensaje->respuesta));
	desplazamiento += sizeof(mensaje->respuesta);

	// ID de tripulante
	memcpy(&(mensaje->id_tripulante), buffer_deserializar + desplazamiento, sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	// Posicion X
	memcpy(&(mensaje->posicion_x), buffer_deserializar + desplazamiento, sizeof(mensaje->posicion_x));
	desplazamiento += sizeof(mensaje->posicion_x);

	// Posicion Y
	memcpy(&(mensaje->posicion_y), buffer_deserializar + desplazamiento, sizeof(mensaje->posicion_y));
	desplazamiento += sizeof(mensaje->posicion_y);

	free(buffer_deserializar);
}


void deserializar_respuesta_nueva_tarea(t_respuesta_tarea_tripulante* mensaje, int32_t conexion) {
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	// Respuesta
	memcpy(&(mensaje->respuesta), buffer_deserializar + desplazamiento, sizeof(mensaje->respuesta));
	desplazamiento += sizeof(mensaje->respuesta);

	// ID de tripulante
	memcpy(&(mensaje->id_tripulante), buffer_deserializar + desplazamiento, sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	// Operacion de la tarea
	memcpy(&(mensaje->tarea->operacion), buffer_deserializar + desplazamiento, sizeof(mensaje->tarea->operacion));
	desplazamiento += sizeof(mensaje->tarea->operacion);

	// Cantidad de la tarea
	memcpy(&(mensaje->tarea->cantidad), buffer_deserializar + desplazamiento, sizeof(mensaje->tarea->cantidad));
	desplazamiento += sizeof(mensaje->tarea->cantidad);

	// Posicion X de la tarea
	memcpy(&(mensaje->tarea->posicion_x), buffer_deserializar + desplazamiento, sizeof(mensaje->tarea->posicion_x));
	desplazamiento += sizeof(mensaje->tarea->posicion_x);

	// Posicion Y de la tarea
	memcpy(&(mensaje->tarea->posicion_y), buffer_deserializar + desplazamiento, sizeof(mensaje->tarea->posicion_y));
	desplazamiento += sizeof(mensaje->tarea->posicion_y);

	// Tiempo de la tarea
	memcpy(&(mensaje->tarea->tiempo), buffer_deserializar + desplazamiento, sizeof(mensaje->tarea->tiempo));
	desplazamiento += sizeof(mensaje->tarea->tiempo);

	free(buffer_deserializar);
}

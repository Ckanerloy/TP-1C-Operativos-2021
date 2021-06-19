#include "deserializacion.h"


void deserializar_iniciar_patota(t_iniciar_patota* mensaje, int32_t conexion)
{
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


void deserializar_respuesta_patota(t_respuesta_iniciar_patota* mensaje, int32_t conexion) {
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	memcpy(&(mensaje->respuesta), buffer_deserializar + desplazamiento, sizeof(mensaje->respuesta));
	desplazamiento += sizeof(mensaje->respuesta);

	memcpy(&(mensaje->tamanio_ids), buffer_deserializar + desplazamiento, sizeof(mensaje->tamanio_ids));
	desplazamiento += sizeof(mensaje->tamanio_ids);

	mensaje->ids_tripu=malloc(mensaje->tamanio_ids+1);

	memcpy(mensaje->ids_tripu, buffer_deserializar + desplazamiento, mensaje->tamanio_ids+1);
	desplazamiento += mensaje->tamanio_ids+1;

	memcpy(&(mensaje->numero_de_patota), buffer_deserializar + desplazamiento, sizeof(mensaje->numero_de_patota));
	desplazamiento += sizeof(mensaje->numero_de_patota);
	free(buffer_deserializar);
}


void deserealizar_tripulante(t_tcb* mensaje, int32_t conexion) {
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	// Id de Tripulante
	memcpy(&(mensaje->id_tripulante), buffer_deserializar + desplazamiento, sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	// Estado de Tripulante
	memcpy(&(mensaje->estado_tripulante), buffer_deserializar + desplazamiento, sizeof(mensaje->estado_tripulante));
	desplazamiento += sizeof(mensaje->estado_tripulante);

	// Posicion X
	memcpy(&(mensaje->posicion_x), buffer_deserializar + desplazamiento, sizeof(mensaje->posicion_x));
	desplazamiento += sizeof(mensaje->posicion_x);

	// Posicion Y
	memcpy(&(mensaje->posicion_y), buffer_deserializar + desplazamiento, sizeof(mensaje->posicion_y));
	desplazamiento += sizeof(mensaje->posicion_y);

	// Id proxima instruccion
	memcpy(&(mensaje->id_proxima_instruccion), buffer_deserializar + desplazamiento, sizeof(mensaje->id_proxima_instruccion));
	desplazamiento += sizeof(mensaje->id_proxima_instruccion);

	// Puntero del PCB
	memcpy(&(mensaje->puntero_PCB), buffer_deserializar + desplazamiento, sizeof(mensaje->puntero_PCB));
	desplazamiento += sizeof(mensaje->puntero_PCB);

	free(buffer_deserializar);
}


void deseralizar_id_tripulante(t_tripulante* mensaje, int32_t conexion)
{
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	memcpy(&(mensaje->id_tripulante), buffer_deserializar + desplazamiento, sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	memcpy(&(mensaje->id_patota), buffer_deserializar + desplazamiento, sizeof(mensaje->id_patota));
	desplazamiento += sizeof(mensaje->id_patota);

	free(buffer_deserializar);
}

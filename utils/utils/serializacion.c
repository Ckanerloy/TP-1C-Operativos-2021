#include "serializacion.h"

void* serializar_paquete(t_paquete* paquete, void* mensaje, codigo_operacion operacion, uint32_t* tamanio_paquete)
{

	uint32_t tamanio_preparado = 0;
	paquete->op_code = operacion;

	void* buffer_serializar = malloc(sizeof(tamanio_paquete));

	switch(operacion) {

		case INICIAR_PATOTA:
			tamanio_preparado = serializar_paquete_iniciar_patota(paquete, mensaje);
			break;

		case INICIAR_TRIPULANTE:
			tamanio_preparado = serializar_paquete_tripulante(paquete, mensaje);
			break;

		case OBTENER_BITACORA:
			tamanio_preparado = serializar_paquete_id_tripulante(paquete, mensaje);
			break;

		case EXPULSAR_TRIPULANTE:
			tamanio_preparado = serializar_paquete_id_tripulante(paquete, mensaje);
			break;

		case RESPUESTA_INICIAR_PATOTA:
			tamanio_preparado = serializar_respuesta_iniciar_patota(paquete, mensaje);
			break;

		case CERRAR_MODULO:
			paquete->buffer->size = 0;
			paquete->buffer->stream = NULL;
			tamanio_preparado = sizeof(codigo_operacion) + sizeof(paquete->buffer->size) + paquete->buffer->size;
			break;

		default:
			printf("404 operacion NOT FOUND.\n");
			break;
	}

	buffer_serializar = malloc(tamanio_preparado);
	uint32_t desplazamiento = 0;

	memcpy(buffer_serializar + desplazamiento, &(paquete->op_code), sizeof(paquete->op_code));
	desplazamiento+= sizeof(paquete->op_code);

	memcpy(buffer_serializar + desplazamiento, &(paquete->buffer->size), sizeof(paquete->buffer->size));
	desplazamiento+= sizeof(paquete->buffer->size);

	memcpy(buffer_serializar + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	(*tamanio_paquete) = tamanio_preparado;

	return buffer_serializar;
}


uint32_t serializar_paquete_iniciar_patota(t_paquete* paquete, t_iniciar_patota* mensaje)
{
	uint32_t tamanio = 0;
	uint32_t desplazamiento = 0;
	uint32_t tamanio_a_enviar = 0;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(mensaje->cantidad_tripulantes) + sizeof(mensaje->tamanio_tareas) + strlen(mensaje->tareas_de_patota)+1 + sizeof(mensaje->tamanio_posiciones) + strlen(mensaje->posiciones)+1;
	//buffer->size = sizeof(uint32_t)*2 + strlen(mensaje->archivo_tareas)+1;

	void* stream_auxiliar = malloc(buffer->size);

	// Cantidad de Tripulantes
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->cantidad_tripulantes), sizeof(mensaje->cantidad_tripulantes));
	desplazamiento += sizeof(mensaje->cantidad_tripulantes);

	// Tamanio archivo de tareas
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->tamanio_tareas), sizeof(mensaje->tamanio_tareas));
	desplazamiento += sizeof(mensaje->tamanio_tareas);

	// Archivo de tareas
	memcpy(stream_auxiliar + desplazamiento, mensaje->tareas_de_patota, mensaje->tamanio_tareas +1);
	desplazamiento += mensaje->tamanio_tareas+1;

	// Tamanio de posiciones
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->tamanio_posiciones), sizeof(mensaje->tamanio_posiciones));
	desplazamiento += sizeof(mensaje->tamanio_posiciones);

	// Posiciones de los tripulantes
	memcpy(stream_auxiliar + desplazamiento, mensaje->posiciones, mensaje->tamanio_posiciones+1);
	desplazamiento += mensaje->tamanio_posiciones+1;

	tamanio_a_enviar = sizeof(mensaje->cantidad_tripulantes) + sizeof(mensaje->tamanio_tareas) + mensaje->tamanio_tareas+1 + sizeof(mensaje->tamanio_posiciones) + mensaje->tamanio_posiciones+1;

	if(desplazamiento != tamanio_a_enviar)
	{
		puts("ERROR. Tamanio diferentes.\n");
		log_error(logger, "Verificar datos que le estoy mandando. Kinda sus...");
		abort();
	}

	else
	{
		buffer->stream = stream_auxiliar;
		buffer->size = desplazamiento;
		paquete->buffer = buffer;

		tamanio = sizeof(codigo_operacion) + sizeof(paquete->buffer->size) + paquete->buffer->size;

		return tamanio;
	}

}


uint32_t serializar_respuesta_iniciar_patota(t_paquete* paquete, t_respuesta_iniciar_patota* mensaje)
{
	uint32_t tamanio = 0;
	uint32_t desplazamiento = 0;

	uint32_t tamanio_a_enviar = 0;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(mensaje->respuesta) + sizeof(mensaje->tamanio_ids) + strlen(mensaje->ids_tripu)+1 + sizeof(mensaje->numero_de_patota);

	void* stream_auxiliar = malloc(buffer->size);

	// Respuesta
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->respuesta), sizeof(mensaje->respuesta));
	desplazamiento += sizeof(mensaje->respuesta);

	// Tamanio ids
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->tamanio_ids), sizeof(mensaje->tamanio_ids));
	desplazamiento += sizeof(mensaje->tamanio_ids);

	// Cadena ids
	memcpy(stream_auxiliar + desplazamiento, mensaje->ids_tripu, mensaje->tamanio_ids+1);
	desplazamiento += mensaje->tamanio_ids+1;

	// Numero de Patota
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->numero_de_patota), sizeof(mensaje->numero_de_patota));
	desplazamiento += sizeof(mensaje->numero_de_patota);

	tamanio_a_enviar = sizeof(mensaje->respuesta) + sizeof(mensaje->tamanio_ids) + mensaje->tamanio_ids+1 + sizeof(mensaje->numero_de_patota);

	if(desplazamiento != tamanio_a_enviar)
	{
		puts("ERROR. Tamanio diferentes.\n");
		//log_error(logger);
		abort();
	}

	else
	{

		paquete->buffer = buffer;
		paquete->buffer->size = desplazamiento;
		paquete->buffer->stream = stream_auxiliar;

		tamanio = sizeof(codigo_operacion) + sizeof(paquete->buffer->size) + paquete->buffer->size;

		return tamanio;
	}
}

uint32_t serializar_paquete_tripulante(t_paquete* paquete, t_tcb* mensaje)
{
	uint32_t tamanio = 0;
	uint32_t desplazamiento = 0;

	uint32_t tamanio_a_enviar = 0;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(mensaje->id_tripulante) + sizeof(mensaje->estado_tripulante) + sizeof(mensaje->posicion_x) + sizeof(mensaje->posicion_y) + sizeof(mensaje->id_proxima_instruccion) + sizeof(mensaje->puntero_PCB);

	void* stream_auxiliar = malloc(buffer->size);

	// Id de Tripulante
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->id_tripulante), sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	// Estado de Tripulante
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->estado_tripulante), sizeof(mensaje->estado_tripulante));
	desplazamiento += sizeof(mensaje->estado_tripulante);

	// Posicion X
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->posicion_x), sizeof(mensaje->posicion_x));
	desplazamiento += sizeof(mensaje->posicion_x);

	// Posicion Y
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->posicion_y), sizeof(mensaje->posicion_y));
	desplazamiento += sizeof(mensaje->posicion_y);

	// Id proxima instruccion
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->id_proxima_instruccion), sizeof(mensaje->id_proxima_instruccion));
	desplazamiento += sizeof(mensaje->id_proxima_instruccion);

	// Puntero del PCB
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->puntero_PCB), sizeof(mensaje->puntero_PCB));
	desplazamiento += sizeof(mensaje->puntero_PCB);

	tamanio_a_enviar = sizeof(mensaje->id_tripulante) + sizeof(mensaje->estado_tripulante) + sizeof(mensaje->posicion_x) + sizeof(mensaje->posicion_y) + sizeof(mensaje->id_proxima_instruccion) + sizeof(mensaje->puntero_PCB);


	if(desplazamiento != tamanio_a_enviar)
	{
		puts("ERROR. Tamanio diferentes.\n");
		//log_error(logger);
		abort();
	}

	else
	{

		paquete->buffer = buffer;
		paquete->buffer->size = desplazamiento;
		paquete->buffer->stream = stream_auxiliar;

		tamanio = sizeof(codigo_operacion) + sizeof(paquete->buffer->size) + paquete->buffer->size;

		return tamanio;
	}
}



uint32_t serializar_paquete_id_tripulante(t_paquete* paquete, t_id_tripulante* mensaje)
{
	uint32_t tamanio = 0;
	uint32_t desplazamiento = 0;

	uint32_t tamanio_a_enviar = 0;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t);

	void* stream_auxiliar = malloc(buffer->size);

	// Id de Tripulante
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->id_tripulante), sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	tamanio_a_enviar = sizeof(mensaje->id_tripulante);

	if(desplazamiento != tamanio_a_enviar)
	{
		puts("ERROR. Tamanio diferentes.\n");
		//log_error(logger);
		abort();
	}

	else
	{

		paquete->buffer = buffer;
		paquete->buffer->size = desplazamiento;
		paquete->buffer->stream = stream_auxiliar;

		tamanio = sizeof(codigo_operacion) + sizeof(paquete->buffer->size) + paquete->buffer->size;

		return tamanio;
	}
}

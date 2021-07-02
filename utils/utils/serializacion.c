#include "serializacion.h"


void enviar_mensaje(void* mensaje, codigo_operacion operacion, int32_t conexion)
{
	t_paquete* paquete_a_armar = malloc(sizeof(t_paquete));
	crear_buffer(paquete_a_armar);
	uint32_t tamanio_paquete = 0;

	void* paquete_serializado = serializar_paquete(paquete_a_armar, mensaje, operacion, &tamanio_paquete);

	send(conexion, paquete_serializado, tamanio_paquete, 0);

	eliminar_paquete(paquete_a_armar);

	free(paquete_serializado);
}


void* serializar_paquete(t_paquete* paquete, void* mensaje, codigo_operacion operacion, uint32_t* tamanio_paquete)
{

	uint32_t tamanio_preparado = 0;
	paquete->op_code = operacion;

	void* buffer_serializar = malloc(sizeof(tamanio_paquete));

	switch(operacion) {

		case INICIAR_PATOTA:
			tamanio_preparado = serializar_paquete_iniciar_patota(paquete, mensaje);
			break;

		case ACTUALIZAR_UBICACION_TRIPULANTE:
			tamanio_preparado = serializar_paquete_ubicacion_tripulante(paquete, mensaje);
			break;

		case PEDIR_UBICACION_TRIPULANTE:
			tamanio_preparado = serializar_paquete_tripulante(paquete, mensaje);
			break;

		case ACTUALIZAR_ESTADO_TRIPULANTE:
			tamanio_preparado = serializar_paquete_estado_tripulante(paquete, mensaje);
			break;

		case PEDIDO_TAREA:
			tamanio_preparado = serializar_paquete_tripulante(paquete, mensaje);
			break;

		case EXPULSAR_TRIPULANTE:
			tamanio_preparado = serializar_paquete_tripulante(paquete, mensaje);
			break;

		case OBTENER_BITACORA:
			tamanio_preparado = serializar_paquete_tripulante(paquete, mensaje);
			break;

		case ENVIAR_SABOTAJE:
			break;


		// Respuestas
		case RESPUESTA_INICIAR_PATOTA:
			tamanio_preparado = serializar_respuesta_iniciar_patota(paquete, mensaje);
			break;

		case RESPUESTA_OK_UBICACION:
			tamanio_preparado = serializar_respuesta_tripulante(paquete, mensaje);
			break;

		case RESPUESTA_NUEVA_UBICACION:
			tamanio_preparado = serializar_respuesta_nueva_ubicacion(paquete, mensaje);
			break;

		case RESPUESTA_OK_ESTADO:
			tamanio_preparado = serializar_respuesta_tripulante(paquete, mensaje);
			break;

		case RESPUESTA_NUEVA_TAREA:
			tamanio_preparado = serializar_respuesta_tarea_tripulante(paquete, mensaje);
			break;

		case RESPUESTA_TRIPULANTE_ELIMINADO:
			tamanio_preparado = serializar_respuesta_tripulante(paquete, mensaje);
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
		//log_error(logger, "Verificar datos que le estoy mandando. Kinda sus...");
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


uint32_t serializar_paquete_tripulante(t_paquete* paquete, t_tripulante* mensaje)
{
	uint32_t tamanio = 0;
	uint32_t desplazamiento = 0;

	uint32_t tamanio_a_enviar = 0;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(mensaje->id_tripulante) + sizeof(mensaje->id_patota);

	void* stream_auxiliar = malloc(buffer->size);

	// ID de Tripulante
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->id_tripulante), sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	// ID de Patota
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->id_patota), sizeof(mensaje->id_patota));
	desplazamiento += sizeof(mensaje->id_patota);

	tamanio_a_enviar = sizeof(mensaje->id_tripulante) + sizeof(mensaje->id_patota);


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


uint32_t serializar_paquete_ubicacion_tripulante(t_paquete* paquete, t_tripulante_ubicacion* mensaje)
{
	uint32_t tamanio = 0;
	uint32_t desplazamiento = 0;

	uint32_t tamanio_a_enviar = 0;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(mensaje->id_tripulante) + sizeof(mensaje->id_patota) + sizeof(mensaje->posicion_x) + sizeof(mensaje->posicion_y);

	void* stream_auxiliar = malloc(buffer->size);

	// ID de Tripulante
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->id_tripulante), sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	// ID de Patota
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->id_patota), sizeof(mensaje->id_patota));
	desplazamiento += sizeof(mensaje->id_patota);

	// Posicion X
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->posicion_x), sizeof(mensaje->posicion_x));
	desplazamiento += sizeof(mensaje->posicion_x);

	// Posicion Y
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->posicion_y), sizeof(mensaje->posicion_y));
	desplazamiento += sizeof(mensaje->posicion_y);

	tamanio_a_enviar = sizeof(mensaje->id_tripulante) + sizeof(mensaje->id_patota) + sizeof(mensaje->posicion_x) + sizeof(mensaje->posicion_y);


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


uint32_t serializar_paquete_estado_tripulante(t_paquete* paquete, t_tripulante_estado* mensaje)
{
	uint32_t tamanio = 0;
	uint32_t desplazamiento = 0;

	uint32_t tamanio_a_enviar = 0;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(mensaje->id_tripulante) + sizeof(mensaje->id_patota) + sizeof(mensaje->estado);

	void* stream_auxiliar = malloc(buffer->size);

	// ID de Tripulante
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->id_tripulante), sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	// ID de Patota
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->id_patota), sizeof(mensaje->id_patota));
	desplazamiento += sizeof(mensaje->id_patota);

	// Estado tripulante
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->estado), sizeof(mensaje->estado));
	desplazamiento += sizeof(mensaje->estado);

	tamanio_a_enviar = sizeof(mensaje->id_tripulante) + sizeof(mensaje->id_patota) + sizeof(mensaje->estado);


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


//uint32_t serializar_paquete_sabotaje(t_paquete* paquete, tripulante_sabotaje* mensaje) {}





// Respuestas
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

uint32_t serializar_respuesta_tripulante(t_paquete* paquete, t_respuesta_tripulante* mensaje)
{
	uint32_t tamanio = 0;
	uint32_t desplazamiento = 0;

	uint32_t tamanio_a_enviar = 0;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(mensaje->respuesta) + sizeof(mensaje->id_tripulante);

	void* stream_auxiliar = malloc(buffer->size);

	// Respuesta
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->respuesta), sizeof(mensaje->respuesta));
	desplazamiento += sizeof(mensaje->respuesta);

	// ID tripulante
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->id_tripulante), sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	tamanio_a_enviar = sizeof(mensaje->respuesta) + sizeof(mensaje->id_tripulante);

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

uint32_t serializar_respuesta_nueva_ubicacion(t_paquete* paquete, t_respuesta_tripulante_ubicacion* mensaje)
{
	uint32_t tamanio = 0;
	uint32_t desplazamiento = 0;

	uint32_t tamanio_a_enviar = 0;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(mensaje->respuesta) + sizeof(mensaje->id_tripulante) + sizeof(mensaje->posicion_x) + sizeof(mensaje->posicion_y);

	void* stream_auxiliar = malloc(buffer->size);

	// Respuesta
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->respuesta), sizeof(mensaje->respuesta));
	desplazamiento += sizeof(mensaje->respuesta);

	// ID tripulante
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->id_tripulante), sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	// Posicion X
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->posicion_x), sizeof(mensaje->posicion_x));
	desplazamiento += sizeof(mensaje->posicion_x);

	// Posicion Y
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->posicion_y), sizeof(mensaje->posicion_y));
	desplazamiento += sizeof(mensaje->posicion_y);


	tamanio_a_enviar = sizeof(mensaje->respuesta) + sizeof(mensaje->id_tripulante) + sizeof(mensaje->posicion_x) + sizeof(mensaje->posicion_y);

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

uint32_t serializar_respuesta_tarea_tripulante(t_paquete* paquete, t_respuesta_tarea_tripulante* mensaje)
{
	uint32_t tamanio = 0;
	uint32_t desplazamiento = 0;

	uint32_t tamanio_a_enviar = 0;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(mensaje->respuesta) + sizeof(mensaje->id_tripulante) + sizeof(mensaje->tarea->cantidad) + sizeof(mensaje->tarea->operacion) + sizeof(mensaje->tarea->posicion_x) + sizeof(mensaje->tarea->posicion_y) + sizeof(mensaje->tarea->tiempo);

	void* stream_auxiliar = malloc(buffer->size);

	// Respuesta
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->respuesta), sizeof(mensaje->respuesta));
	desplazamiento += sizeof(mensaje->respuesta);

	// ID tripulante
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->id_tripulante), sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	// Operacion de la tarea
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->tarea->operacion), sizeof(mensaje->tarea->operacion));
	desplazamiento += sizeof(mensaje->tarea->operacion);

	// Cantidad de la tarea
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->tarea->cantidad), sizeof(mensaje->tarea->cantidad));
	desplazamiento += sizeof(mensaje->tarea->cantidad);

	// Posicion X de la tarea
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->tarea->posicion_x), sizeof(mensaje->tarea->posicion_x));
	desplazamiento += sizeof(mensaje->tarea->posicion_x);

	// Posicion Y de la tarea
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->tarea->posicion_y), sizeof(mensaje->tarea->posicion_y));
	desplazamiento += sizeof(mensaje->tarea->posicion_y);

	// Tiempo de la tarea
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->tarea->tiempo), sizeof(mensaje->tarea->tiempo));
	desplazamiento += sizeof(mensaje->tarea->tiempo);


	tamanio_a_enviar = sizeof(mensaje->respuesta) + sizeof(mensaje->id_tripulante) + sizeof(mensaje->tarea->cantidad) + sizeof(mensaje->tarea->operacion) + sizeof(mensaje->tarea->posicion_x) + sizeof(mensaje->tarea->posicion_y) + sizeof(mensaje->tarea->tiempo);

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

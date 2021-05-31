#include "sockets.h"


// FUNCIONES PARA MANEJAR CONEXIONES, SERVIDORES
int32_t crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info = malloc(sizeof(struct addrinfo));

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
		//printf("ERROR 404. CONNECTION NOT FOUND.\n");
		socket_cliente = -1;
	}
	freeaddrinfo(server_info);

	return socket_cliente;
}


int32_t resultado_conexion(int32_t conexion, t_log* logger, char* modulo) {

	if(conexion == -1)
	{
		log_warning(logger, "¡Error! No se ha podido conectar a %s. \n", modulo);
		return -1;
	}
	else
	{
		log_info(logger, "Se pudo conectar a %s. \n", modulo);
		return conexion;
	}
}

int32_t validacion_envio(int32_t conexion_cliente) {

	codigo_operacion cod_op;

	if(recv(conexion_cliente, &cod_op, sizeof(cod_op), MSG_WAITALL) < 1){
		 return 0;
	}
	else {
		return 1;
	}
}

void cerrar_conexion(t_log* logger, int32_t socket)
{
	if(socket <= 0 ) {
		log_error(logger, "No existe tal conexion.\n");
	}
	else {
		log_info(logger, "Cerrando conexion...\n");
		close(socket);
		sleep(1);
	}
}


int32_t iniciar_servidor(char* IP, char* PUERTO)
{
	int32_t socket_servidor;
	int32_t activo = 1;

    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(IP, PUERTO, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &activo,sizeof(activo));

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }

	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    //log_trace(logger, "Listo para escuchar a mi cliente");

    return socket_servidor;
}


int32_t* esperar_conexion(int32_t socket_servidor)
{
	int32_t* socket_cliente = malloc(sizeof(int32_t));
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	// Espero que se conecte alguien ...
	*socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	return socket_cliente;
}



void escuchar_conexion(int32_t* conexion_cliente)
{
	codigo_operacion operacion;

	recv(*conexion_cliente, &operacion, sizeof(operacion), MSG_WAITALL);

	procesar_mensajes(operacion, *conexion_cliente);
}

void obtener_operando(int32_t* conexion_cliente, t_paquete* paquete) {

	recv(*conexion_cliente, &(paquete->op_code), sizeof(paquete->op_code), MSG_WAITALL);

}


void recibir_operacion(int32_t socket_cliente, codigo_operacion operacion)
{
	recv(socket_cliente, &operacion, sizeof(operacion), MSG_WAITALL);
}


void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

/*
t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
	return NULL;
}*/



// FUNCIONES PARA RECIBIR UN MENSAJE
void recibir_mensaje(void* mensaje, codigo_operacion operacion, int32_t conexion)
{
	switch(operacion) {

		case INICIAR_PATOTA:
			deserializar_iniciar_patota(mensaje, conexion);
			break;

		case INICIAR_TRIPULANTE:
			deserealizar_tripulante(mensaje, conexion);
			break;

		case EXPULSAR_TRIPULANTE:
			deseralizar_id_tripulante(mensaje, conexion);
			break;

		case OBTENER_BITACORA:
			deseralizar_id_tripulante(mensaje, conexion);
			break;

		case RESPUESTA_INICIAR_PATOTA:
			deserializar_respuesta_patota(mensaje, conexion);
			break;

		default:
			printf("404 operacion NOT FOUND.\n");
			break;

	}
}


void* recibir_buffer(uint32_t* size, int32_t conexion_cliente)
{
	void * buffer;
	recv(conexion_cliente, size, sizeof(uint32_t), MSG_WAITALL);
	buffer = malloc(*size);
	recv(conexion_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}


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


void deserializar_respuesta_patota(t_respuesta* mensaje, int32_t conexion) {
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


void deseralizar_id_tripulante(t_id_tripulante* mensaje, int32_t conexion)
{
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	memcpy(&(mensaje->id_tripulante), buffer_deserializar + desplazamiento, sizeof(mensaje->id_tripulante));
	desplazamiento += sizeof(mensaje->id_tripulante);

	free(buffer_deserializar);
}




// FUNCIONES PARA ENVIAR UN MENSAJE
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


void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

// Serializaciones

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
			// Este lo enviaria al Mongo Store mepa
			break;

		case EXPULSAR_TRIPULANTE:
			tamanio_preparado = serializar_paquete_id_tripulante(paquete, mensaje);
			break;

		case RESPUESTA_INICIAR_PATOTA:
			tamanio_preparado = serializar_respuesta_iniciar_patota(paquete, mensaje);
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


uint32_t serializar_respuesta_iniciar_patota(t_paquete* paquete, t_respuesta* mensaje) {
	uint32_t tamanio = 0;
	uint32_t desplazamiento = 0;

	uint32_t tamanio_a_enviar = 0;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t)+ sizeof(mensaje->tamanio_ids) + strlen(mensaje->ids_tripu)+1;

	void* stream_auxiliar = malloc(buffer->size);

	// Respuesta
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->respuesta), sizeof(mensaje->respuesta));
	desplazamiento += sizeof(mensaje->respuesta);

	//Tamanio ids
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->tamanio_ids), sizeof(mensaje->tamanio_ids));
	desplazamiento += sizeof(mensaje->tamanio_ids);

	// Cadena ids
	memcpy(stream_auxiliar + desplazamiento, mensaje->ids_tripu, mensaje->tamanio_ids+1);
	desplazamiento += mensaje->tamanio_ids+1;

	tamanio_a_enviar = sizeof(mensaje->respuesta) + sizeof(mensaje->tamanio_ids) + mensaje->tamanio_ids+1;

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

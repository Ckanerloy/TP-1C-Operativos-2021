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
		printf("ERROR 404. CONNECTION NOT FOUND.\n");
		socket_cliente = -1;
	}
	freeaddrinfo(server_info);

	return socket_cliente;
}


void cerrar_conexion(int32_t socket)
{
	close(socket);
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





/*
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}*/








int32_t* esperar_conexion(int32_t socket_servidor)
{
	int32_t* socket_cliente = malloc(sizeof(int32_t));
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	// Espero que se conecte alguien ...
	*socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	return socket_cliente;
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

		case EXPULSAR_TRIPULANTE:
			deseralizar_id_tripulante(mensaje, conexion);
			break;

		case OBTENER_BITACORA:
			deseralizar_id_tripulante(mensaje, conexion);
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


void deserializar_iniciar_patota(t_patota* mensaje, int32_t conexion)
{
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	memcpy(&(mensaje->cantidad_tripulantes), buffer_deserializar + desplazamiento, sizeof(mensaje->cantidad_tripulantes));
	desplazamiento += sizeof(mensaje->cantidad_tripulantes);

	memcpy(&(mensaje->tamanio_tareas), buffer_deserializar + desplazamiento, sizeof(mensaje->tamanio_tareas));
	desplazamiento += sizeof(mensaje->tamanio_tareas);

	mensaje->archivo_tareas = malloc(mensaje->tamanio_tareas+1);

	memcpy(mensaje->archivo_tareas, buffer_deserializar + desplazamiento, mensaje->tamanio_tareas +1);
	desplazamiento += mensaje->tamanio_tareas+1;

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

		case OBTENER_BITACORA:
			tamanio_preparado = serializar_paquete_id_tripulante(paquete, mensaje);
			// Este lo enviaria al Mongo Store mepa
			break;

		case EXPULSAR_TRIPULANTE:
			tamanio_preparado = serializar_paquete_id_tripulante(paquete, mensaje);
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


uint32_t serializar_paquete_iniciar_patota(t_paquete* paquete, t_patota* mensaje)
{
	uint32_t tamanio = 0;
	uint32_t desplazamiento = 0;
	uint32_t tamanio_a_enviar = 0;

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = sizeof(mensaje->cantidad_tripulantes) + sizeof(mensaje->tamanio_tareas) + strlen(mensaje->archivo_tareas)+1;
	//buffer->size = sizeof(uint32_t)*2 + strlen(mensaje->archivo_tareas)+1;

	void* stream_auxiliar = malloc(buffer->size);

	// Cantidad de Tripulantes
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->cantidad_tripulantes), sizeof(mensaje->cantidad_tripulantes));
	desplazamiento += sizeof(mensaje->cantidad_tripulantes);

	// Tamanio archivo de tareas
	memcpy(stream_auxiliar + desplazamiento, &(mensaje->tamanio_tareas), sizeof(mensaje->tamanio_tareas));
	desplazamiento += sizeof(mensaje->tamanio_tareas);

	// Archivo de tareas
	memcpy(stream_auxiliar + desplazamiento, mensaje->archivo_tareas, mensaje->tamanio_tareas +1);
	desplazamiento += mensaje->tamanio_tareas+1;

	tamanio_a_enviar = sizeof(mensaje->cantidad_tripulantes) + sizeof(mensaje->tamanio_tareas) + mensaje->tamanio_tareas+1;

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

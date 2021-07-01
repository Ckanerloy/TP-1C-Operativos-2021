#include "sockets.h"


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

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		socket_cliente = -1;

	// crear un hilo por cada conexion que realizara

	freeaddrinfo(server_info);

	return socket_cliente;
}


int32_t resultado_conexion(int32_t conexion, t_log* logger, char* modulo) {

	if(conexion == -1)
	{
		log_error(logger, "¡Error! No se ha podido conectar a %s. \n", modulo);
		return -1;
	}
	else
	{
		//log_info(logger, "Se pudo conectar a %s. \n", modulo);
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
		//log_info(logger, "Cerrando conexion...\n");
		close(socket);
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

    return socket_servidor;
}


// Crearia el hilo en esta parte??
int32_t* esperar_conexion(int32_t socket_servidor)
{
	int32_t* socket_cliente = malloc(sizeof(int32_t));
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	// Espero que se conecte alguien ...
	*socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	log_info(logger, "¡Se creó una conexión!");

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


void* recibir_buffer(uint32_t* size, int32_t conexion_cliente)
{
	void * buffer;
	recv(conexion_cliente, size, sizeof(uint32_t), MSG_WAITALL);
	buffer = malloc(*size);
	recv(conexion_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}


void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

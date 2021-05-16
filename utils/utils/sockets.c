#include "sockets.h"


// FUNCIONES PARA MANEJAR CONEXIONES, SERVIDORES
int crear_conexion(char *ip, char* puerto)
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
		printf("Error al intentar conectarse.\n");
		socket_cliente = -1;
	}
	freeaddrinfo(server_info);

	return socket_cliente;
}


void cerrar_conexion(int socket) {
	close(socket);
}


int iniciar_servidor(char* IP, char* PUERTO)
{
	int socket_servidor;
	int activo = 1;

	t_log* logger;

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

    log_trace(logger, "Listo para escuchar a mi cliente");

    return socket_servidor;
}











void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}



void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}


void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}


void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}









int esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	t_log* logger;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	log_info(logger, "Se conecto un cliente!");
	puts("Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) != 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}





void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}



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
}



void enviar_mensaje(void* mensaje, codigo_operacion operacion, int conexion) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));
	uint32_t tamanio_paquete = 0;

	void* paquete_serializado = serializar_paquete(paquete, mensaje, operacion, &tamanio_paquete);

	bytesEnviados(send(socket, paquete_serializado, tamanio_paquete, 0));

	eliminar_paquete(paquete);
	free(paquete_serializado);
}



// Serializaciones


void* serializar_paquete(t_paquete* paquete, void* mensaje, codigo_operacion operacion, uint32_t tamanio_paquete)
{

	uint32_t tamanio_preparado = 0;
	paquete->op_code = operacion;

	void* buffer_serializar;


	switch(operacion) {

		case INICIAR_PATOTA:
			tamanio_preparado = serializar_paquete_por_patota(paquete, mensaje);
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


uint32_t serializar_paquete_por_patota(t_paquete* paquete, patota* mensaje) {

	uint32_t tamanio = 0;
	uint32_t desplazamiento = 0;

	uint32_t size = 0;
		uint32_t desplazamiento = 0;
		uint32_t pesoDeElementosAEnviar = 0;

		if(strlen(estructura->nombreRestaurante) != estructura->largoNombreRestaurante){
				   printf("Error en la serializacion de longitudes, sos pollo\n");
				   return -1;
				}

		if(strlen(estructura->nombrePlato) != estructura->largoNombrePlato){
				   printf("Error en la serializacion de longitudes, sos pollo\n");
				   return -1;
				}


		 //reservo memoria ESPECIFICAMENTE para el buffer de bytes (payload) que mi querido paquete va a contener
		t_buffer* buffer = malloc(sizeof(t_buffer));
		buffer->size = sizeof(uint32_t)*4
					 + strlen(estructura->nombreRestaurante)+1
					 + strlen(estructura->nombrePlato)+1;

		void* streamAuxiliar = malloc(buffer->size);

		//meto el largo del nombre del Restaurante
		memcpy(streamAuxiliar + desplazamiento, &(estructura->largoNombreRestaurante), sizeof(estructura->largoNombreRestaurante));
		desplazamiento += sizeof(estructura->largoNombreRestaurante);

		//meto el nombre del restaurante
		memcpy(streamAuxiliar + desplazamiento, estructura->nombreRestaurante, estructura->largoNombreRestaurante+1);
		desplazamiento += estructura->largoNombreRestaurante+1;

		//meto la ID del pedido
		memcpy(streamAuxiliar + desplazamiento, &(estructura->idPedido), sizeof(estructura->idPedido));
		desplazamiento += sizeof(estructura->idPedido);

		//meto el largo del nombre del plato a agregar al pedido
		memcpy(streamAuxiliar + desplazamiento, &(estructura->largoNombrePlato), sizeof(estructura->largoNombrePlato));
		desplazamiento += sizeof(estructura->largoNombrePlato);

		//meto el nombre del plato a agregar al pedido
		memcpy(streamAuxiliar + desplazamiento, estructura->nombrePlato, estructura->largoNombrePlato+1);
		desplazamiento += estructura->largoNombrePlato+1;

		//meto la cantidad de platos a agregar al pedido
		memcpy(streamAuxiliar + desplazamiento, &(estructura->cantidadPlatos), sizeof(estructura->cantidadPlatos));
		desplazamiento += sizeof(estructura->cantidadPlatos);

		//controlo que el desplazamiento sea = al peso de lo que mando
		pesoDeElementosAEnviar = sizeof(estructura->largoNombreRestaurante) + estructura->largoNombreRestaurante+1 + sizeof(estructura->idPedido) + sizeof(estructura->largoNombrePlato) + estructura->largoNombrePlato+1 + sizeof(estructura->cantidadPlatos);


}




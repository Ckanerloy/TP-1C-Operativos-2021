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

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		socket_cliente = -1;

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
		case RECIBIR_SABOTAJE:
			deserealizar_sabotaje(mensaje,conexion);
			break;

		case CERRAR_MODULO:
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


<<<<<<< HEAD
=======
void deserealizar_sabotaje(t_respuesta_mongo* mensaje,int32_t conexion){
	uint32_t tamanio;
	uint32_t desplazamiento = 0;
	void* buffer_deserializar;
	buffer_deserializar = recibir_buffer(&tamanio, conexion);

	memcpy(&(mensaje->sabotaje_on), buffer_deserializar + desplazamiento, sizeof(mensaje->sabotaje_on));
	desplazamiento += sizeof(mensaje->sabotaje_on);

	free(buffer_deserializar);

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


>>>>>>> planificacion


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



#include "Mongo-Store.h"


int main(void)
{


	config = crear_config(CONFIG_PATH);

	obtener_datos_de_config(config);

	logger = crear_log("mongo-store.log", "Mongo Store");
	log_info(logger, "Servidor Mongo Store activo...");


	int32_t conexion_servidor = iniciar_servidor(IP, PUERTO);
	//int prueba = existe_file_system();

	if (existe_file_system() == -1){

		printf("No se encontró el archivo Blocks.ims. Se inicializa un nuevo FileSystem \n");

		inicializar_file_system();

		//Abrir el blocks.ims, hacer copia, escribir esa copia y sincronizar cada TIEMPO_SINCRONIZACION (15 segs)
		//Hacer lo mismo con el FS existente

	}
		else{
			printf("Hay un FileSystem existente\n");
			iniciar_superbloque();//Inicio el FS existente
			crear_directorio_file_vacio("/Files/PRUEBA111.IMS");
	}

	/*// Recibe la señal para enviar sabotaje
	signal(SIGUSR1, iniciar_sabotaje);


	while(1)
	{
		int32_t* conexion_cliente = esperar_conexion(conexion_servidor);

		pthread_create(&hilo_recibir_mensajes, NULL, (void*)escuchar_conexion, conexion_cliente);
		pthread_detach(hilo_recibir_mensajes);

	}
*/
	terminar_programa(config, logger);
	return EXIT_SUCCESS;
}


void obtener_datos_de_config(t_config* config) {

	PUERTO = config_get_string_value(config, "PUERTO");
	PUNTO_MONTAJE = config_get_string_value(config, "PUNTO_MONTAJE");
	TIEMPO_SINCRONIZACION = config_get_int_value(config, "TIEMPO_SINCRONIZACION");
	POSICIONES_SABOTAJE = config_get_array_value(config, "POSICIONES_SABOTAJE");
	BLOCK_SIZE = config_get_int_value(config, "BLOCK_SIZE");
	BLOCKS= config_get_int_value(config, "BLOCKS");

}

void iniciar_sabotaje(void){
	//creo socket cliente, conexion,
	//conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);
	//enviar_mensaje(tripulante_estado, ACTUALIZAR_ESTADO_TRIPULANTE, conexion_mi_ram);
}


void procesar_mensajes(codigo_operacion operacion, int32_t conexion) {

	t_id_tripulante* tripulante_por_bitacora;

	switch(operacion)
{
			case OBTENER_BITACORA:
				tripulante_por_bitacora = malloc(sizeof(t_id_tripulante));
				recibir_mensaje(tripulante_por_bitacora, operacion, conexion);

				// Recibe un id tripulante, y le envia la BITACORA del tripulante buscado
				free(tripulante_por_bitacora);
				break;

			default:
				log_warning(logger, "Operacion desconocida. No quieras meter la pata");
				break;
			}
}

// Crear un nuevo archivo por defecto dentro de /Files
char* crear_directorio_file_vacio(char* path_archivo){


	char* path_completo = malloc(strlen(PUNTO_MONTAJE) + strlen(path_archivo) + 2);

	path_completo = concatenar_path(path_archivo);

	FILE* archivo = fopen( path_completo , "w" );

	if (archivo == NULL){
		printf("****************ERROR | No se pudo crear el archivo %s ****************\n", path_completo);
		exit(-1);
	}

	fclose(archivo);

	t_config* contenido_archivo = config_create(path_completo);

	// Valores default del archivo
	config_set_value(contenido_archivo, "SIZE", "0");
	config_set_value(contenido_archivo, "BLOCK_COUNT", "0");
	config_set_value(contenido_archivo, "BLOCKS", "0");
	config_set_value(contenido_archivo, "CARACTER_LLENADO", "0");
	config_set_value(contenido_archivo, "MD5_ARCHIVO", "0");

	config_save(contenido_archivo);

	config_destroy(contenido_archivo);

	return path_completo;
}

/*ejemplo:
	SIZE=132
	BLOCK_COUNT=3
	BLOCKS=[1,2,3]
	CARACTER_LLENADO=O
	MD5_ARCHIVO=BD1014D173BA92CC014850A7087E254E
*/

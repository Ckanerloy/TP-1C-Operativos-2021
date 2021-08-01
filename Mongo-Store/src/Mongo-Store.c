#include "Mongo-Store.h"


int main(void) {
	config = crear_config(CONFIG_PATH);

	obtener_datos_de_config(config);

	logger = crear_log("mongo-store.log", "Mongo Store");
	log_info(logger, "Servidor Mongo Store activo...");


	num_sabotaje = 0;
	// Recibe la señal para enviar sabotaje
	signal(SIGUSR1, (void*)iniciar_sabotaje);



	//char* un_bitarray = malloc(BLOCKS/8);
	//bitArraySB = crear_bitarray(un_bitarray);



	if (existe_file_system() == -1) {

		log_info(logger, "No se encontró el archivo Blocks.ims. Se inicializa un nuevo FileSystem \n");

		inicializar_file_system();
		//levantar_archivo_blocks();
		//Abrir el blocks.ims, hacer copia, escribir esa copia y sincronizar cada TIEMPO_SINCRONIZACION (15 segs)
		//Hacer lo mismo con el FS existente

	}
		else{
			log_info(logger, "Hay un FileSystem existente, el mismo no debe ser inicializado\n");
			//en caso de que ya exista FS, se levanta el Blocks.ims y poner contenido en un char*, se levanta SuperBloque.ims

			//Seccion Blocks
			char *direccion_blocks = concatenar_path("/Blocks.ims");
			archivo_blocks = open(direccion_blocks, O_RDWR, S_IRUSR|S_IWUSR);
			struct stat statbuf;
			ftruncate(archivo_blocks, BLOCK_SIZE*BLOCKS);
			fstat(archivo_blocks, &statbuf);
			blocks = mmap(NULL, statbuf.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, archivo_blocks, 0);
			levantar_archivo_blocks();
			escribir_archivo_blocks(3, "BLOQUE 3", strlen("BLOQUE 3"));


			//Seccion SuperBloque
			char *direccion_superBloque = concatenar_path("/SuperBloque.ims");
			struct stat statbuf_2;
			archivo = open(direccion_superBloque, O_RDWR, S_IRUSR|S_IWUSR);
			ftruncate(archivo, 2*sizeof(uint32_t)+BLOCKS/8);
			fstat(archivo, &statbuf_2);
			super_bloque = mmap(NULL, statbuf_2.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, archivo,0);
			levantar_archivo_superBloque();

			bitarray_set_bit(bitArraySB, 3);


		}


	pthread_create(&hilo_sincronizador, NULL, (void*)sincronizar, NULL);
	pthread_detach(hilo_sincronizador);

	int32_t conexion_servidor = iniciar_servidor(IP, PUERTO);

	while(1) {
		int32_t conexion_cliente = esperar_conexion(conexion_servidor);

		pthread_create(&hilo_recibir_mensajes, NULL, (void*)escuchar_conexion, (int32_t*)conexion_cliente);
		pthread_detach(hilo_recibir_mensajes);
	}

printf("TERMINO");
free(bitmap);

	return EXIT_SUCCESS;


}





void obtener_datos_de_config(t_config* config) {

	PUERTO_DISCORDIADOR = config_get_string_value(config, "PUERTO_DISCORDIADOR");
	PUERTO = config_get_string_value(config, "PUERTO");
	PUNTO_MONTAJE = config_get_string_value(config, "PUNTO_MONTAJE");
	TIEMPO_SINCRONIZACION = config_get_int_value(config, "TIEMPO_SINCRONIZACION");
	POSICIONES_SABOTAJE = config_get_array_value(config, "POSICIONES_SABOTAJE");
	BLOCK_SIZE = config_get_int_value(config, "BLOCK_SIZE");
	BLOCKS= config_get_int_value(config, "BLOCKS");

}

void procesar_mensajes(codigo_operacion operacion, int32_t conexion) {

	// OBTENER_BITACORA
	t_tripulante* tripulante_por_bitacora;

	// Tareas I/O
	archivo_tarea* tarea_io;

	switch(operacion) {

			case OBTENER_BITACORA:
				tripulante_por_bitacora = malloc(sizeof(t_tripulante));
				recibir_mensaje(tripulante_por_bitacora, operacion, conexion);

				// Recibe un id tripulante
				// Busca la bitacora (archivo) del tripulante
				// Envia la bitacora
				// Cuando envia, es un CHAR* enorme para que Discordiador y asi lo guarda en el logger

				// char* bitacora_tripulante;
				// uint32_t tamanio_bitacora_tripu;

				cerrar_conexion(logger, conexion);
				free(tripulante_por_bitacora);
				break;

			case GENERAR_INSUMO:
				tarea_io = malloc(sizeof(archivo_tarea));
				recibir_mensaje(tarea_io, operacion, conexion);

				printf("Llego la tarea de GENERAR_INSUMO\n");

				cerrar_conexion(logger, conexion);

				free(tarea_io->nombre_archivo);
				free(tarea_io);
				break;

			case CONSUMIR_INSUMO:
				tarea_io = malloc(sizeof(archivo_tarea));
				recibir_mensaje(tarea_io, operacion, conexion);

				printf("Llego la tarea de CONSUMIR_INSUMO\n");

				cerrar_conexion(logger, conexion);
				free(tarea_io->nombre_archivo);
				free(tarea_io);
				break;

			case TIRAR_BASURA:
				cerrar_conexion(logger, conexion);
				printf("Llego la tarea de DESCARTAR_BASURA\n");

				break;

			case ACTUALIZACION_TRIPULANTE:

				// Crea el archivo para el tripulante o le agrega los datos actualizados del tripulante al final

				break;

			case CERRAR_MODULO:
				cerrar_conexion(logger, conexion);
				printf("Terminando programa... \n");
				sleep(1);
				printf("-------------------------------------------------------------------------------------------------------------------------------------------------- \n");

				// SE LIBERA TODO LO QUE SE TENGA QUE LIBERAR DE MEMORIA
				//free(hilo_recibir_mensajes);
				log_info(logger, "Se ha cerrado el programa de forma exitosa.\n");
				terminar_programa(config, logger);
				exit(0);

			default:
				log_warning(logger, "Operacion desconocida. No quieras meter la pata");
				break;
			}
}


// Crear un nuevo archivo por defecto dentro de /Files
char* crear_archivo_metadata(char* path_archivo){


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

//PARA GENERAR EL MD5 https://askubuntu.com/questions/53846/how-to-get-the-md5-hash-of-a-string-directly-in-the-terminal
/* FUNCION PARA EL MD5
 * crear "archivo" con contenido de los bloques de Oxigeno.ims/Comida.ims/etc concatenado
 * usar system("md5sum Nuevo.txt > pruebamd5.txt"), en el segundo archivo se guarda el hash + espacio + nombre del primer archivo
 * ejemplo: d0a4a9d5eae1444b3285be84e98afcf8  Nuevo.txt
 * splintear para quedarse solamente con el hash y return char* para asignárselo al Oxigeno.ims/Comida.ims/etc
 * Mantener actualizado MD5 por cada modificacion que se haga
 *
 */



char* crear_archivo_bitacora(char* path_archivo){

	//PUNTO_MONTAJE/Files/Bitacoras/Tripulante1.ims
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
	config_set_value(contenido_archivo, "BLOCKS", "0");

	config_save(contenido_archivo);

	config_destroy(contenido_archivo);

	return path_completo;
}


void iniciar_sabotaje(void){

	int32_t cantidad_sabotajes = cantidad_posiciones(POSICIONES_SABOTAJE);

	if(num_sabotaje != cantidad_sabotajes) {
		char** posiciones_sabo = string_split(POSICIONES_SABOTAJE[num_sabotaje], "|");

		int32_t conexion_discordiador = crear_conexion(IP, PUERTO_DISCORDIADOR);

		posicion_sabotaje* posicion = malloc(sizeof(posicion_sabotaje));
		posicion->posicion_x = atoi(posiciones_sabo[0]);
		posicion->posicion_y = atoi(posiciones_sabo[1]);

  		if(resultado_conexion(conexion_discordiador, logger, "Discordiador") != -1) {
  			enviar_mensaje(posicion, SABOTAJE, conexion_discordiador);
 			cerrar_conexion(logger, conexion_discordiador);
 		}

  		log_info(logger, "Se enviaron las posiciones X: %u e Y: %u del Sabotaje.\n", posicion->posicion_x, posicion->posicion_y);

		num_sabotaje++;
		free(posicion);
		limpiar_parser(posiciones_sabo);
	}
	else {
		log_warning(logger, "No hay más sabotajes para realizar.\n");
	}
}


uint32_t cantidad_posiciones(char** parser) {

	int cantidad = 0;
	while(parser[cantidad] != NULL){
		cantidad++;
	}
	return cantidad;
}


void sincronizar(){
	while(1){
		sleep(TIEMPO_SINCRONIZACION);
		memcpy(blocks, informacion_blocks, BLOCK_SIZE*BLOCKS);
		msync(blocks, BLOCK_SIZE*BLOCKS, MS_SYNC);

		memcpy(super_bloque+sizeof(uint32_t)*2, bitmap, BLOCKS/8);
		msync(super_bloque, 2*sizeof(uint32_t)+BLOCKS/8, MS_SYNC);


	}


}






#include "Mongo-Store.h"


int main(void) {
	config = crear_config(CONFIG_PATH);

	obtener_datos_de_config(config);

	logger = crear_log("mongo-store.log", "Mongo Store");
	log_info(logger, "Servidor Mongo Store activo...");

	mutex_bitacora = malloc(sizeof(sem_t));
	sem_init(mutex_bitacora, 0, 1);

	num_sabotaje = 0;
	// Recibe la señal para enviar sabotaje
	signal(SIGUSR1, (void*)iniciar_sabotaje);

	//char* un_bitarray = malloc(BLOCKS/8);
	//bitArraySB = crear_bitarray(un_bitarray);

	if (existe_file_system() == -1) {

		log_info(logger, "No se encontró el archivo Blocks.ims. Se inicializa un nuevo FileSystem \n");

		inicializar_file_system();
		levantar_archivo_blocks();
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
		//	escribir_archivo_blocks(3, "BLOQUE 3", strlen("BLOQUE 3"));

			//Seccion SuperBloque
			char *direccion_superBloque = concatenar_path("/SuperBloque.ims");
			struct stat statbuf_2;
			archivo = open(direccion_superBloque, O_RDWR, S_IRUSR|S_IWUSR);
			ftruncate(archivo, 2*sizeof(uint32_t)+BLOCKS/8);
			fstat(archivo, &statbuf_2);
			super_bloque = mmap(NULL, statbuf_2.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, archivo,0);
			levantar_archivo_superBloque();
			//hash_MD5();
		}

	pthread_create(&hilo_sincronizador, NULL, (void*)sincronizar, NULL);
	pthread_detach(hilo_sincronizador);

	int32_t conexion_servidor = iniciar_servidor(IP, PUERTO);

	while(1) {
		int32_t conexion_cliente = esperar_conexion(conexion_servidor);

		pthread_create(&hilo_recibir_mensajes, NULL, (void*)escuchar_conexion, (int32_t*)conexion_cliente);
		pthread_detach(hilo_recibir_mensajes);
	}
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
	BLOCKS = config_get_int_value(config, "BLOCKS");

}

void procesar_mensajes(codigo_operacion operacion, int32_t conexion) {

	// OBTENER_BITACORA
	t_tripulante* tripulante_por_bitacora;

	// Tareas I/O
	archivo_tarea* tarea_io;

	// Actualizar Datos de Bitacora
	bitacora* bitacora_tripu;

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
				bitacora_tripu = malloc(sizeof(bitacora));
				recibir_mensaje(bitacora_tripu, operacion, conexion);

				string_trim(&bitacora_tripu->accion);
				printf("Tripu %u: %s\n", bitacora_tripu->id_tripulante, bitacora_tripu->accion);

				printf("Tamanio accion: %u \n", bitacora_tripu->tamanio_accion);
				char* path = string_new();
				string_append_with_format(&path, "/Files/Bitacoras/Tripulante%u.ims", bitacora_tripu->id_tripulante);

				char* path_completo = malloc(strlen(PUNTO_MONTAJE) + strlen(path) + 2);

				path_completo = concatenar_path(path);

				if(open(path_completo, O_RDWR, S_IRUSR|S_IWUSR) < 0) { //si me devuelve 0 es por que existe el doc y tengo que escribirlo
					log_info(logger, "No existe la bitacora del tripulante %u , se procede a crearla.\n",bitacora_tripu->id_tripulante);

					sem_wait(mutex_bitacora);
					crear_archivo_metadata_bitacora(path_completo);
					t_metadata_bitacora* metadata_bitacora = actualizar_archivo_metadata_bitacora(path_completo, bitacora_tripu->tamanio_accion);
					guardar_en_blocks(path_completo, bitacora_tripu, metadata_bitacora);

				}
				else {
					sem_wait(mutex_bitacora);
					t_metadata_bitacora* metadata_bitacora = actualizar_archivo_metadata_bitacora(path_completo, bitacora_tripu->tamanio_accion);
					guardar_en_blocks(path_completo, bitacora_tripu, metadata_bitacora);
				}

				/*
				 * Buscar si existe la Bitacora del Tripulante #ID
				 * 	- Si existe: actualizarla
				 * 	- Si NO existe: crearla y agregarle los datos en blocks.ims
				 */

				cerrar_conexion(logger, conexion);
				free(bitacora_tripu->accion);
				free(bitacora_tripu);
				break;

			case REALIZAR_SABOTAJE:
				cerrar_conexion(logger, conexion);

				printf("----------------------REALIZO EL SABOTAJE (FCSK)\n");

				/*
				 * Aca realizo todo lo necesario para resolver el sabotaje
				 */

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


int32_t cantidad_bloques_a_usar(uint32_t tamanio_a_guardar){

	int32_t cantidad_bloques = tamanio_a_guardar / BLOCK_SIZE;

	if (tamanio_a_guardar % BLOCK_SIZE != 0){
		cantidad_bloques++;
	}
	return cantidad_bloques;
}


t_list* obtener_array_bloques_a_usar(uint32_t tamanio_a_guardar){
	int32_t cantidad_bloques = cantidad_bloques_a_usar(tamanio_a_guardar);

	t_list* posiciones = list_create();

	for(int i=0; i<cantidad_bloques; i++){
		int posicion_bit_libre = posicionBitLibre();
		list_add(posiciones, (int) posicion_bit_libre);
		bitarray_set_bit(bitArraySB, posicion_bit_libre);
	}
	return posiciones;
}


void guardar_en_blocks(char* path_completo, bitacora* bitacora_tripu, t_metadata_bitacora* metadata_bitacora){
	FILE* archivo = fopen( path_completo , "r+" );

	if (archivo == NULL){
		log_error(logger, "No se pudo escribir en el archivo %s .\n", path_completo);
		exit(-1);
	}

	t_config* contenido_archivo = config_create(path_completo);

	int valor_size_nuevo = config_get_int_value(contenido_archivo, "SIZE");

	char** bloques_asignados_nuevo = config_get_array_value(contenido_archivo,"BLOCKS");

	uint32_t cant_bloq_asig_nuevos = cantidad_elementos(bloques_asignados_nuevo);

	uint32_t cant_bloq_asig_anterior = cantidad_elementos(metadata_bitacora->bloques_asignados_anterior);

	if (cant_bloq_asig_anterior == 0){

		int32_t desplazamiento = 0;

		for(int i=0; i<cant_bloq_asig_nuevos-1; i++) { //accion mas grande que el bloque

			uint32_t nro_bloque = atoi(bloques_asignados_nuevo[i]);
			uint32_t ubicacion_bloque = nro_bloque * BLOCK_SIZE;

			memcpy(informacion_blocks+ubicacion_bloque, bitacora_tripu->accion + desplazamiento, BLOCK_SIZE);
			desplazamiento += BLOCK_SIZE;
		}

		uint32_t nro_ultimo_bloque = atoi(bloques_asignados_nuevo[cant_bloq_asig_nuevos-1]);
		uint32_t espacio_libre_ultimo_bloque = (cant_bloq_asig_nuevos*BLOCK_SIZE - valor_size_nuevo);
		uint32_t cant_necesaria_ultimo_bloque = BLOCK_SIZE - espacio_libre_ultimo_bloque;

		uint32_t ubicacion_bloque = nro_ultimo_bloque * BLOCK_SIZE;

		memcpy(informacion_blocks+ubicacion_bloque, bitacora_tripu->accion+desplazamiento, cant_necesaria_ultimo_bloque);

	}else{
		uint32_t nro_ultimo_bloque = atoi(metadata_bitacora->bloques_asignados_anterior[cant_bloq_asig_anterior-1]);
		int32_t libre_ultimo_bloque = BLOCK_SIZE*cant_bloq_asig_anterior - metadata_bitacora->size;
		int32_t usado_ultimo_bloque = BLOCK_SIZE - libre_ultimo_bloque;

		int32_t desplazamiento = 0;

		if(libre_ultimo_bloque >= bitacora_tripu->tamanio_accion){
			uint32_t ubicacion_bloque = nro_ultimo_bloque * BLOCK_SIZE;
			memcpy(informacion_blocks + ubicacion_bloque + usado_ultimo_bloque, bitacora_tripu->accion, bitacora_tripu->tamanio_accion);
		}else{

			uint32_t ubicacion_bloque = nro_ultimo_bloque * BLOCK_SIZE;
			memcpy(informacion_blocks + ubicacion_bloque + usado_ultimo_bloque, bitacora_tripu->accion, libre_ultimo_bloque);
			desplazamiento += libre_ultimo_bloque;

			for(int i=0; i< (cant_bloq_asig_nuevos - cant_bloq_asig_anterior) - 1; i++) { //accion mas grande que el bloque

				uint32_t nro_bloque = atoi(bloques_asignados_nuevo[i+cant_bloq_asig_anterior]);
				uint32_t ubicacion_bloque = nro_bloque * BLOCK_SIZE;

				memcpy(informacion_blocks+ubicacion_bloque, bitacora_tripu->accion + desplazamiento, BLOCK_SIZE);
				desplazamiento += BLOCK_SIZE;
			}
			uint32_t nro_ultimo_bloque = atoi(bloques_asignados_nuevo[cant_bloq_asig_nuevos-1]);

			uint32_t espacio_libre_ultimo_bloque = (cant_bloq_asig_nuevos*BLOCK_SIZE - valor_size_nuevo);
			uint32_t cant_necesaria_ultimo_bloque = BLOCK_SIZE - espacio_libre_ultimo_bloque;

			ubicacion_bloque = nro_ultimo_bloque * BLOCK_SIZE;

			memcpy(informacion_blocks+ubicacion_bloque, bitacora_tripu->accion+desplazamiento, cant_necesaria_ultimo_bloque);

		}
	}
	sem_post(mutex_bitacora);
}



// Crear un nuevo archivo por defecto dentro de /Files
void crear_archivo_metadata_recurso(char* path_archivo){

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

}

//PARA GENERAR EL MD5 https://askubuntu.com/questions/53846/how-to-get-the-md5-hash-of-a-string-directly-in-the-terminal
/* FUNCION PARA EL MD5
 * ejemplo: d0a4a9d5eae1444b3285be84e98afcf8  Nuevo.txt
 * splintear para quedarse solamente con el hash y return char* para asignárselo al Oxigeno.ims/Comida.ims/etc
 * Mantener actualizado MD5 por cada modificacion que se haga
 *
 */

void hash_MD5(){
	char* comando = "/home/utnso/Escritorio/Nuevo.txt > /home/utnso/Escritorio/pruebamd5.txt";
	char* system_command = string_new();
	string_append_with_format(&system_command, "md5sum %s ", comando);
	system(system_command);
}


void crear_archivo_metadata_bitacora(char* path_completo){

	FILE* archivo = fopen( path_completo , "w" );

	if (archivo == NULL){
		log_error(logger, "No se pudo crear el archivo %s \n", path_completo);
		exit(-1);
	}

	fclose(archivo);

	t_config* contenido_archivo = config_create(path_completo);

	// Valores default del archivo
	config_set_value(contenido_archivo, "SIZE", "0");
	config_set_value(contenido_archivo, "BLOCKS", "[]");

	config_save(contenido_archivo);

	config_destroy(contenido_archivo);
}


t_metadata_bitacora* actualizar_archivo_metadata_bitacora(char* path, uint32_t tamanio_accion){

	t_metadata_bitacora* metadata_bitacora = malloc(sizeof(t_metadata_bitacora));
	metadata_bitacora->size = leer_size_archivo(path, "SIZE");

	int nuevo_valor_size = metadata_bitacora->size + tamanio_accion;
	guardar_nuevo_size_archivo(path, nuevo_valor_size, "SIZE");

	metadata_bitacora->bloques_asignados_anterior = leer_blocks_archivo(path, "BLOCKS");

	uint32_t cantidad_bloques_usados = cantidad_elementos(metadata_bitacora->bloques_asignados_anterior);

	if(cantidad_bloques_usados == 0){

		t_list* lista_posiciones = obtener_array_bloques_a_usar(tamanio_accion);
		char* bloques = string_new();
		string_append_with_format(&bloques,"[");

		for(int i=0; i<list_size(lista_posiciones); i++){
			int posicion = (int) list_get(lista_posiciones, i);
		//	printf("posicion a agregar en la lista %d \n",posicion);
			if(i == list_size(lista_posiciones)-1)
				string_append_with_format(&bloques,"%u", posicion);
			else{
				string_append_with_format(&bloques,"%u,", posicion);
			}
		}
		string_append_with_format(&bloques,"]");

		guardar_nuevos_blocks_archivo(path, bloques, "BLOCKS");
	}
	else{

		uint32_t fragmentacion_interna=cantidad_bloques_usados*BLOCK_SIZE - metadata_bitacora->size;

		t_list* lista_posiciones;
		if(tamanio_accion>fragmentacion_interna){
			lista_posiciones = obtener_array_bloques_a_usar(tamanio_accion-fragmentacion_interna);// 10 frag de 4  entocnes falta guardar 6
		}else{
			lista_posiciones = obtener_array_bloques_a_usar(0);
		}

		char* bloques = string_new();
		string_append_with_format(&bloques,"[");
		int recorrido=0;
		while(metadata_bitacora->bloques_asignados_anterior[recorrido]!=NULL){
			if(recorrido == cantidad_bloques_usados-1 && list_size(lista_posiciones)==0){
				string_append_with_format(&bloques,metadata_bitacora->bloques_asignados_anterior[recorrido]);

			}else{
				string_append_with_format(&bloques,metadata_bitacora->bloques_asignados_anterior[recorrido]);
				string_append_with_format(&bloques,",");
			}
			recorrido++;
		}
		for(int i=0; i<list_size(lista_posiciones); i++){
			int posicion = (int) list_get(lista_posiciones, i);
			if(i == list_size(lista_posiciones)-1)
				string_append_with_format(&bloques,"%u", posicion);
			else{
				string_append_with_format(&bloques,"%u,", posicion);
			}
		}
		string_append_with_format(&bloques,"]");
		guardar_nuevos_blocks_archivo(path, bloques, "BLOCKS");
	}

	return metadata_bitacora;
}



uint32_t cantidad_elementos(char** parser) {

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
		if(msync(blocks, BLOCK_SIZE*BLOCKS, MS_SYNC)<0){
			log_error(logger, "No se pudo sincronizar blocks.");
			return;
		}

		memcpy(super_bloque+sizeof(uint32_t)*2, bitmap, BLOCKS/8);
		if(msync(super_bloque, 2*sizeof(uint32_t)+BLOCKS/8, MS_SYNC)<0){
			log_error(logger, "No se pudo sincronizar el super bloque.");
			return;
		}
	}
}




// Tratamiento sobre Files (con Blocks y Size)
int leer_size_archivo(char* path_archivo, char* clave){

	t_config* datos_archivo = config_create(path_archivo);

	int size = config_get_int_value(datos_archivo, clave);

	config_destroy(datos_archivo);

	return size;
}


void guardar_nuevo_size_archivo(char* path_archivo, int valor, char* clave){

	t_config* datos_metadata = config_create(path_archivo);

	char* valor_a_string;

	asprintf(&valor_a_string, "%d", valor);

	config_set_value(datos_metadata, clave, valor_a_string);

	config_save(datos_metadata);

	config_destroy(datos_metadata);

	free(valor_a_string);
}


char** leer_blocks_archivo(char* path_archivo, char* clave) {

	t_config* datos_archivo = config_create(path_archivo);

	char** blocks = config_get_array_value(datos_archivo, clave);

	config_destroy(datos_archivo);

	return blocks;
}


void guardar_nuevos_blocks_archivo(char* path_archivo, char* valor, char* clave){

	t_config* datos_metadata = config_create(path_archivo);

	config_set_value(datos_metadata, clave, valor);

	config_save(datos_metadata);

	config_destroy(datos_metadata);

	free(valor);
}





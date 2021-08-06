#include "Mongo-Store.h"

int main(void) {
	config = crear_config(CONFIG_PATH);

	obtener_datos_de_config(config);

	logger = crear_log("mongo-store.log", "Mongo Store");
	log_info(logger, "Servidor Mongo Store activo...");

	inicializar_semaforos();

	num_sabotaje = 0;
	// Recibe la señal para enviar sabotaje
	signal(SIGUSR1, (void*)iniciar_sabotaje);

	if (existe_file_system() == -1) {

		log_info(logger, "No se encontró el archivo Blocks.ims. Se inicializa un nuevo FileSystem \n");

		inicializar_file_system();
		levantar_archivo_blocks();
		inicio_protocolo_fsck();

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

			//Seccion SuperBloque
			char *direccion_superBloque = concatenar_path("/SuperBloque.ims");
			struct stat statbuf_2;
			archivo = open(direccion_superBloque, O_RDWR, S_IRUSR|S_IWUSR);
			ftruncate(archivo, 2*sizeof(uint32_t)+BLOCKS/8);
			fstat(archivo, &statbuf_2);
			super_bloque = mmap(NULL, statbuf_2.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, archivo,0);
			levantar_archivo_superBloque();
			inicio_protocolo_fsck();
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

void inicializar_semaforos(void) {

	sem_oxigeno = malloc(sizeof(sem_t));
	sem_init(sem_oxigeno, 0, 1);

	sem_comida = malloc(sizeof(sem_t));
	sem_init(sem_comida, 0, 1);

	sem_basura = malloc(sizeof(sem_t));
	sem_init(sem_basura, 0, 1);

	mutex_blocks = malloc(sizeof(sem_t));
	sem_init(mutex_blocks, 0, 1);

	mutex_config = malloc(sizeof(sem_t));
	sem_init(mutex_config, 0, 1);

	mutex_bitacora = malloc(sizeof(sem_t));
	sem_init(mutex_bitacora, 0, 1);
}

void semaforo_recurso(recursos_archivos recurso, void(*funcion)(void*)) {

    if(recurso == OXIGENO) {
        funcion(sem_oxigeno);
    }
    else if(recurso == COMIDA) {
        funcion(sem_comida);
    }
    else {
        funcion(sem_basura);
    }
}

void procesar_mensajes(codigo_operacion operacion, int32_t conexion) {

	// OBTENER_BITACORA
	t_tripulante* tripulante_por_bitacora;

	// Tareas I/O
	archivo_tarea* tarea_io;

	// Actualizar Datos de Bitacora
	bitacora* bitacora_tripu;

	char* path;
	char* path_completo;
	char* recurso;

	switch(operacion) {

			case OBTENER_BITACORA:
				tripulante_por_bitacora = malloc(sizeof(t_tripulante));
				recibir_mensaje(tripulante_por_bitacora, operacion, conexion);

				path_completo = crear_ruta_bitacora(tripulante_por_bitacora->id_tripulante);

				if(open(path_completo, O_RDWR, S_IRUSR|S_IWUSR) < 0) {
					log_error(logger, "No existe la bitacora del tripulante %u", tripulante_por_bitacora->id_tripulante);
					break;
				}

				else {
					mensaje_bitacora* mensaje = malloc(sizeof(mensaje_bitacora));

					sem_wait(mutex_bitacora);
					int size_bitacora = leer_size_archivo(path_completo, "SIZE");
					char** bloques = leer_blocks_archivo(path_completo, "BLOCKS");
					sem_post(mutex_bitacora);
					char* bitacora = malloc(size_bitacora);

					uint32_t cant_bloques = cantidad_elementos(bloques);
					uint32_t desplazamiento = 0;

					for(int i=0; i<cant_bloques-1; i++) {

						uint32_t nro_bloque = atoi(bloques[i]);
						uint32_t ubicacion_bloque = nro_bloque * BLOCK_SIZE;
						sem_wait(mutex_blocks);
						memcpy(bitacora + desplazamiento, informacion_blocks + ubicacion_bloque, BLOCK_SIZE);
						sem_post(mutex_blocks);
						desplazamiento += BLOCK_SIZE;
					}

					uint32_t nro_ultimo_bloque = atoi(bloques[cant_bloques-1]);
					uint32_t espacio_libre_ultimo_bloque = (cant_bloques*BLOCK_SIZE - size_bitacora);
					uint32_t cant_necesaria_ultimo_bloque = BLOCK_SIZE - espacio_libre_ultimo_bloque;
					uint32_t ubicacion_bloque = nro_ultimo_bloque * BLOCK_SIZE;

					sem_wait(mutex_blocks);
					memcpy(bitacora + desplazamiento, informacion_blocks + ubicacion_bloque, cant_necesaria_ultimo_bloque);
					sem_post(mutex_blocks);

					mensaje->tamanio_bitacora = size_bitacora;
					mensaje->bitacora = string_substring(bitacora, 0, size_bitacora);

					enviar_mensaje(mensaje, RESPUESTA_BITACORA, conexion);

					limpiar_parser(bloques);
					free(bitacora);
					free(mensaje->bitacora);
					free(mensaje);
				}

				free(path_completo);
				cerrar_conexion(logger, conexion);
				free(tripulante_por_bitacora);
				break;

			case GENERAR_INSUMO:
				tarea_io = malloc(sizeof(archivo_tarea));
				recibir_mensaje(tarea_io, operacion, conexion);

				recurso = armar_recurso(tarea_io->caracter_llenado, tarea_io->cantidad);

				path_completo = crear_ruta_recurso(tarea_io->nombre_archivo);
				/*path = string_new();
				string_append_with_format(&path, "/Files/%s.ims", tarea_io->nombre_archivo);
				path_completo = malloc(strlen(PUNTO_MONTAJE) + strlen(path) + 2);
				path_completo = concatenar_path(path);*/
				printf("	Llego la tarea de Generar %s\n", tarea_io->nombre_archivo);

				if(open(path_completo, O_RDWR, S_IRUSR|S_IWUSR) < 0) { //si me devuelve 0 es por que existe el doc y tengo que escribirlo
					log_info(logger, "No existe el recurso %s , se procede a crearlo.\n", tarea_io->nombre_archivo);

					crear_archivo_metadata_recurso(path_completo);
					semaforo_recurso(mapeo_string_a_recurso(tarea_io->nombre_archivo), sem_wait);
					t_metadata* metadata_recurso = actualizar_archivo_metadata_recurso(path_completo, tarea_io->caracter_llenado, tarea_io->cantidad, tarea_io->nombre_archivo);
					semaforo_recurso(mapeo_string_a_recurso(tarea_io->nombre_archivo), sem_post);
					sem_wait(mutex_blocks);
					guardar_en_blocks(path_completo, recurso, metadata_recurso);
					sem_post(mutex_blocks);
					concatenar_contenido_blocks(metadata_recurso->bloques_asignados_anterior);
					log_info(logger, "Se generó %d cantidades de %s.\n", tarea_io->cantidad, tarea_io->nombre_archivo);
				}
				else {
					semaforo_recurso(mapeo_string_a_recurso(tarea_io->nombre_archivo), sem_wait);
					t_metadata* metadata_recurso = actualizar_archivo_metadata_recurso(path_completo, tarea_io->caracter_llenado, tarea_io->cantidad, tarea_io->nombre_archivo);
					semaforo_recurso(mapeo_string_a_recurso(tarea_io->nombre_archivo), sem_post);
					sem_wait(mutex_blocks);
					guardar_en_blocks(path_completo, recurso, metadata_recurso);
					sem_post(mutex_blocks);
					concatenar_contenido_blocks(metadata_recurso->bloques_asignados_anterior);
					log_info(logger, "Se generó %d cantidades de %s.\n", tarea_io->cantidad, tarea_io->nombre_archivo);
				}

				cerrar_conexion(logger, conexion);

				free(path_completo);
				free(tarea_io->nombre_archivo);
				free(tarea_io);
				free(recurso);
				break;

			case CONSUMIR_INSUMO:
				tarea_io = malloc(sizeof(archivo_tarea));
				recibir_mensaje(tarea_io, operacion, conexion);
				printf("	Llego la tarea de Consumir %s.\n", tarea_io->nombre_archivo);

				path_completo = crear_ruta_recurso(tarea_io->nombre_archivo);
				recurso = armar_recurso(tarea_io->caracter_llenado, tarea_io->cantidad);

				int size_recurso = leer_size_archivo(path_completo, "SIZE");
				char** bloques = leer_blocks_archivo(path_completo, "BLOCKS");

				t_metadata*	metadata_recurso = malloc(sizeof(t_metadata));
				metadata_recurso->bloques_asignados_anterior = bloques;
				metadata_recurso->size = size_recurso;

				if(open(path_completo, O_RDWR, S_IRUSR|S_IWUSR) < 0) { //si me devuelve 0 es por que existe el doc y tengo que escribirlo
					log_info(logger, "No existe el recurso %s.\n", tarea_io->nombre_archivo);
					cerrar_conexion(logger, conexion);
					free(tarea_io->nombre_archivo);
					free(tarea_io);
					break;
				}
				else {
					if(tarea_io->cantidad >= metadata_recurso->size) {
						sem_wait(mutex_blocks);
						eliminar_recurso_blocks(path_completo, metadata_recurso);
						sem_wait(mutex_blocks);
						log_info(logger, "Se consumió %d cantidades de %s, y el Archivo quedó vacío.\n", metadata_recurso->size, tarea_io->nombre_archivo);
					}
					else {
						semaforo_recurso(mapeo_string_a_recurso(tarea_io->nombre_archivo), sem_wait);
						eliminar_cantidad_recurso(metadata_recurso, tarea_io->cantidad);
						semaforo_recurso(mapeo_string_a_recurso(tarea_io->nombre_archivo), sem_post);
						log_info(logger, "Se consumió %d cantidades de %s.\n", tarea_io->cantidad, tarea_io->nombre_archivo);
					}
					semaforo_recurso(mapeo_string_a_recurso(tarea_io->nombre_archivo), sem_wait);
					t_metadata* metadata_recurso = actualizar_archivo_metadata_recurso(path_completo, tarea_io->caracter_llenado, -(tarea_io->cantidad), tarea_io->nombre_archivo);
					semaforo_recurso(mapeo_string_a_recurso(tarea_io->nombre_archivo), sem_post);
					sem_wait(mutex_blocks);
					eliminar_en_blocks(path_completo, recurso, metadata_recurso);
					sem_post(mutex_blocks);
					log_info(logger, "Se consumió %d cantidades de %s.\n", tarea_io->cantidad, tarea_io->nombre_archivo);

					/*
					 * Remover tantos caracteres como menciona la tarea
					 * - Si se quieren borrar mas caracteres de los que hay: log_info(logger, "Se quisieron eliminar más caracteres de los que hay.\n");
					 * 	 	y dejar el archivo vacío.
					 * - Si se pueden borrar caracteres, se borran y listo.
					 */
				}

				cerrar_conexion(logger, conexion);
				free(tarea_io->nombre_archivo);
				free(tarea_io);
				free(recurso);
				break;

			case TIRAR_BASURA:
				cerrar_conexion(logger, conexion);
				printf("Llego la tarea de DESCARTAR_BASURA\n");

				path_completo = crear_ruta_recurso("Basura");

				/*
				path = string_new();
				string_append_with_format(&path, "/Files/%s", "Basura.ims");
			    path_completo = malloc(strlen(PUNTO_MONTAJE) + strlen(path) + 2);
				path_completo = concatenar_path(path);*/

				if(open(path_completo, O_RDWR, S_IRUSR|S_IWUSR) < 0) { //si me devuelve 0 es por que existe el doc y tengo que escribirlo
					log_info(logger, "No existe el recurso %s.\n", "Basura.ims");
					break;
				}
				else {
					printf("Antes del semaforo \n");

					int size_recurso = leer_size_archivo(path_completo, "SIZE");
					char** bloques = leer_blocks_archivo(path_completo, "BLOCKS");

					t_metadata*	metadata_recurso = malloc(sizeof(t_metadata));
					metadata_recurso->bloques_asignados_anterior = bloques;
					metadata_recurso->size = size_recurso;
					printf("Entra a eliminar recurso \n");
					sem_wait(mutex_blocks);
					eliminar_recurso_blocks(path_completo,  metadata_recurso); //pisa toda la data de los blocks con 0 y setea los bits vacios
					sem_post(mutex_blocks);
					remove(path_completo);
				}
				printf("Fuera del if \n");
				break;

			case ACTUALIZACION_TRIPULANTE:
				bitacora_tripu = malloc(sizeof(bitacora));
				recibir_mensaje(bitacora_tripu, operacion, conexion);

				string_trim(&bitacora_tripu->accion);
				//strcat(bitacora_tripu->accion, "\0");
				printf("Tripu %u: %s\n", bitacora_tripu->id_tripulante, bitacora_tripu->accion);

				path_completo = crear_ruta_bitacora(bitacora_tripu->id_tripulante);

				if(open(path_completo, O_RDWR, S_IRUSR|S_IWUSR) < 0) { //si me devuelve 0 es por que existe el doc y tengo que escribirlo
					log_info(logger, "No existe la bitacora del tripulante %u , se procede a crearla.\n",bitacora_tripu->id_tripulante);

					crear_archivo_metadata_bitacora(path_completo);
					sem_wait(mutex_bitacora);
					t_metadata* metadata_bitacora = actualizar_archivo_metadata_bitacora(path_completo, bitacora_tripu->tamanio_accion);
					sem_post(mutex_bitacora);
					sem_wait(mutex_blocks);
					guardar_en_blocks(path_completo, bitacora_tripu->accion, metadata_bitacora);
					sem_post(mutex_blocks);
					log_info(logger, "Se creó la Bitacora del Tripulante %u exitosamente.\n", bitacora_tripu->id_tripulante);
				}
				else {
					sem_wait(mutex_bitacora);
					t_metadata* metadata_bitacora = actualizar_archivo_metadata_bitacora(path_completo, bitacora_tripu->tamanio_accion);
					sem_post(mutex_bitacora);
					sem_wait(mutex_blocks);
					guardar_en_blocks(path_completo, bitacora_tripu->accion, metadata_bitacora);
					sem_post(mutex_blocks);
					log_info(logger, "Se actualizó la Bitacora del Tripulante %u exitosamente.\n", bitacora_tripu->id_tripulante);
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

				printf("----------------------REALIZO EL SABOTAJE (FSCK)\n");

				inicio_protocolo_fsck();


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

recursos_archivos mapeo_string_a_recurso(char* recurso) {

    recursos_archivos recurso_enum;

    if(strcmp(recurso, "Oxigeno") == 0) {
        recurso_enum = OXIGENO;
    }
    if(strcmp(recurso, "Comida") == 0) {
        recurso_enum = COMIDA;
    }
    if(strcmp(recurso, "Basura") == 0) {
        recurso_enum = BASURA;
    }

    return recurso_enum;
}

bool existe_archivo(char* path){
	FILE* archivo = fopen(path, "r");

	if(archivo != NULL){
		fclose(archivo);
		return true;
	}
	else {
		return false;
	}
}

//[5,8,4,1] ultimo bloque posicion inicial  y lo que guardas en el ultimo bloque
//  posicionInicial+ size(cant ult bloque)

void eliminar_cantidad_recurso(t_metadata* metadata_recurso, uint32_t cantidad_a_eliminar) {
	uint32_t cant_bloques = cantidad_elementos(metadata_recurso->bloques_asignados_anterior);

	char* valor = armar_recurso('0', 1);

	uint32_t espacio_libre_ultimo_bloque = (cant_bloques*BLOCK_SIZE - (metadata_recurso->size));
	uint32_t cant_necesaria_ultimo_bloque = BLOCK_SIZE - espacio_libre_ultimo_bloque;
	uint32_t nro_bloque = atoi(metadata_recurso->bloques_asignados_anterior[cant_bloques-1]);
	uint32_t ubicacion_bloque = nro_bloque * BLOCK_SIZE;

	uint32_t ultimo_caracter = ubicacion_bloque + cant_necesaria_ultimo_bloque;
	//ultimo bloque
	for(int i=0; i<cant_necesaria_ultimo_bloque; i++){ //CCCCC000
		if (cantidad_a_eliminar !=0){
			if(cant_necesaria_ultimo_bloque !=0){
				memcpy(informacion_blocks + ultimo_caracter, valor, strlen(valor));
				ultimo_caracter --;
				cantidad_a_eliminar --;
				cant_necesaria_ultimo_bloque --;
			}
			else {
				bitarray_clean_bit(bitArraySB, cant_bloques-1);
				break;
			}
		}
		else{
			if(cant_necesaria_ultimo_bloque == 0 ){
				bitarray_clean_bit(bitArraySB, nro_bloque);
			}
			free(valor);
			return;
		}
	}
	for(int i=cant_bloques-2; i>=0; i--) {
		uint32_t nro_bloque = atoi(metadata_recurso->bloques_asignados_anterior[i]);

		uint32_t ubicacion_bloque = nro_bloque * BLOCK_SIZE;
		uint32_t ultimo_caracter = ubicacion_bloque + BLOCK_SIZE;

		uint32_t tamanio_bloque = BLOCK_SIZE;

		for(int j=0; j<BLOCK_SIZE; j++){ //CCCCC000
			if (cantidad_a_eliminar !=0){
				if(tamanio_bloque !=0){
					memcpy(informacion_blocks + ultimo_caracter, valor , strlen(valor));
					ultimo_caracter --;
					cantidad_a_eliminar --;
					tamanio_bloque --;
				}
				else {
					bitarray_clean_bit(bitArraySB, nro_bloque);
					break;
				}
			}
			else{
				if(cant_necesaria_ultimo_bloque == 0 ){
					bitarray_clean_bit(bitArraySB, nro_bloque);
				}
				free(valor);
				return;
			}
		}
	}
	free(valor);
}

void eliminar_recurso_blocks(char* path_completo, t_metadata* metadata_recurso){

	uint32_t cant_bloques = cantidad_elementos(metadata_recurso->bloques_asignados_anterior);

	int32_t desplazamiento = 0;  //[5,8,4,1] ultimo bloque posicion inicial  y lo que guardas en el ultimo bloque
	//                                       posicionInicial+ size(cant ult bloque)

	for(int i=0; i<cant_bloques-1; i++) {
		uint32_t nro_bloque = atoi(metadata_recurso->bloques_asignados_anterior[i]);
		bitarray_clean_bit(bitArraySB, nro_bloque);
		char* valor = armar_recurso('0', BLOCK_SIZE);

		uint32_t ubicacion_bloque = nro_bloque * BLOCK_SIZE;

		memcpy(informacion_blocks + ubicacion_bloque, valor + desplazamiento, BLOCK_SIZE);
		desplazamiento += BLOCK_SIZE;
	}

	uint32_t nro_bloque = atoi(metadata_recurso->bloques_asignados_anterior[cant_bloques-1]);
	bitarray_clean_bit(bitArraySB, nro_bloque);
	uint32_t nro_ultimo_bloque = atoi((metadata_recurso->bloques_asignados_anterior[cant_bloques-1]));
	uint32_t espacio_libre_ultimo_bloque = (cant_bloques*BLOCK_SIZE - (metadata_recurso->size));
	uint32_t cant_necesaria_ultimo_bloque = BLOCK_SIZE - espacio_libre_ultimo_bloque;
	char* valor_restante = armar_recurso('0', cant_necesaria_ultimo_bloque);

	uint32_t ubicacion_bloque = nro_ultimo_bloque * BLOCK_SIZE;

	memcpy(informacion_blocks+ubicacion_bloque, valor_restante + desplazamiento, cant_necesaria_ultimo_bloque);
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
		list_add(posiciones, posicion_bit_libre);
		bitarray_set_bit(bitArraySB, posicion_bit_libre);
	}
	return posiciones;
}


void guardar_en_blocks(char* path_completo, char* valor, t_metadata* metadata_bitacora){

	uint32_t tamanio_valor = strlen(valor);

	int valor_size_nuevo = leer_size_archivo(path_completo, "SIZE");

	char** bloques_asignados_nuevo = leer_blocks_archivo(path_completo, "BLOCKS");

	uint32_t cant_bloq_asig_nuevos = cantidad_elementos(bloques_asignados_nuevo);

	uint32_t cant_bloq_asig_anterior = cantidad_elementos(metadata_bitacora->bloques_asignados_anterior);


	if (cant_bloq_asig_anterior == 0){

		int32_t desplazamiento = 0;

		for(int i=0; i<cant_bloq_asig_nuevos-1; i++) { //accion mas grande que el bloque

			uint32_t nro_bloque = atoi(bloques_asignados_nuevo[i]);
			uint32_t ubicacion_bloque = nro_bloque * BLOCK_SIZE;
			memcpy(informacion_blocks + ubicacion_bloque, valor + desplazamiento, BLOCK_SIZE);
			desplazamiento += BLOCK_SIZE;
		}

		uint32_t nro_ultimo_bloque = atoi(bloques_asignados_nuevo[cant_bloq_asig_nuevos - 1]);
		uint32_t espacio_libre_ultimo_bloque = (cant_bloq_asig_nuevos * BLOCK_SIZE - valor_size_nuevo);
		uint32_t cant_necesaria_ultimo_bloque = BLOCK_SIZE - espacio_libre_ultimo_bloque;

		uint32_t ubicacion_bloque = nro_ultimo_bloque * BLOCK_SIZE;
		memcpy(informacion_blocks + ubicacion_bloque, valor + desplazamiento, cant_necesaria_ultimo_bloque);

	}
	else{
		uint32_t nro_ultimo_bloque = atoi(metadata_bitacora->bloques_asignados_anterior[cant_bloq_asig_anterior - 1]);
		int32_t libre_ultimo_bloque = BLOCK_SIZE * cant_bloq_asig_anterior - metadata_bitacora->size;
		int32_t usado_ultimo_bloque = BLOCK_SIZE - libre_ultimo_bloque;
		int32_t desplazamiento = 0;

		if(libre_ultimo_bloque >= tamanio_valor){

			uint32_t ubicacion_bloque = nro_ultimo_bloque * BLOCK_SIZE;
			memcpy(informacion_blocks + ubicacion_bloque + usado_ultimo_bloque, valor, tamanio_valor);

		}
		else{

			uint32_t ubicacion_bloque = nro_ultimo_bloque * BLOCK_SIZE;
			memcpy(informacion_blocks + ubicacion_bloque + usado_ultimo_bloque, valor, libre_ultimo_bloque);
			desplazamiento += libre_ultimo_bloque;

			for(int i=0; i< (cant_bloq_asig_nuevos - cant_bloq_asig_anterior) - 1; i++) { //accion mas grande que el bloque
				uint32_t nro_bloque = atoi(bloques_asignados_nuevo[cant_bloq_asig_anterior + i]);
				uint32_t ubicacion_bloque = nro_bloque * BLOCK_SIZE;
				memcpy(informacion_blocks + ubicacion_bloque, valor + desplazamiento, BLOCK_SIZE);
				desplazamiento += BLOCK_SIZE;
			}
			uint32_t nro_ultimo_bloque = atoi(bloques_asignados_nuevo[cant_bloq_asig_nuevos-1]);
			uint32_t espacio_libre_ultimo_bloque = (cant_bloq_asig_nuevos * BLOCK_SIZE - valor_size_nuevo);
			uint32_t cant_necesaria_ultimo_bloque = BLOCK_SIZE - espacio_libre_ultimo_bloque;
			ubicacion_bloque = nro_ultimo_bloque * BLOCK_SIZE;
			memcpy(informacion_blocks + ubicacion_bloque, valor + desplazamiento, cant_necesaria_ultimo_bloque);
		}
	}
}


void eliminar_en_blocks(char* path_completo, char* valor, t_metadata* metadata_bitacora) {

	uint32_t tamanio_valor = strlen(valor);

	int size_archivo = leer_size_archivo(path_completo, "SIZE");

	char** bloques_asignados_nuevo = leer_blocks_archivo(path_completo, "BLOCKS");

	uint32_t cant_bloq_asig_nuevos = cantidad_elementos(bloques_asignados_nuevo);

	uint32_t cant_bloq_asig_anterior = cantidad_elementos(metadata_bitacora->bloques_asignados_anterior);

	if(size_archivo >= tamanio_valor) {

		// Borrar como debe ser
		int32_t tamanio_restante = size_archivo - tamanio_valor;

		uint32_t nro_ultimo_bloque = atoi(bloques_asignados_nuevo[cant_bloq_asig_nuevos - 1]);
		uint32_t espacio_libre_ultimo_bloque = (cant_bloq_asig_nuevos * BLOCK_SIZE - size_archivo);

		uint32_t ubicacion_bloque = nro_ultimo_bloque * BLOCK_SIZE + tamanio_restante;
		memcpy(informacion_blocks + ubicacion_bloque, "0", tamanio_valor);
	}
	else {
		// Limpiar el blocks y retornar un aviso

		uint32_t nro_ultimo_bloque = atoi(bloques_asignados_nuevo[cant_bloq_asig_nuevos - 1]);
		uint32_t espacio_libre_ultimo_bloque = (cant_bloq_asig_nuevos * BLOCK_SIZE - size_archivo);

		uint32_t ubicacion_bloque = nro_ultimo_bloque * BLOCK_SIZE;
		memcpy(informacion_blocks + ubicacion_bloque, "0", size_archivo);
	}

}

//PARA GENERAR EL MD5 https://askubuntu.com/questions/53846/how-to-get-the-md5-hash-of-a-string-directly-in-the-terminal
/* FUNCION PARA EL MD5
 * ejemplo: d0a4a9d5eae1444b3285be84e98afcf8  Nuevo.txt
 * splintear para quedarse solamente con el hash y return char* para asignárselo al Oxigeno.ims/Comida.ims/etc
 * Mantener actualizado MD5 por cada modificacion que se haga
 *
 */

char* hash_MD5(char* cadena_a_hashear, char* nombre_archivo){
	char* path_archivo_hash_inicial = string_new();
	char* path_archivo_hash_final = string_new();

	string_append_with_format(&path_archivo_hash_inicial, "/Files/ArchivosHash/ArchivoAHashear%s.txt", nombre_archivo);
	string_append_with_format(&path_archivo_hash_final, "/Files/ArchivosHash/ArchivoHasheado%s.txt", nombre_archivo);

	char* path_inicial = malloc(strlen(PUNTO_MONTAJE) + strlen(path_archivo_hash_inicial) + 1);
	path_inicial = concatenar_path(path_archivo_hash_inicial);

	char* path_final = malloc(strlen(PUNTO_MONTAJE) + strlen(path_archivo_hash_final) + 1);
	path_final = concatenar_path(path_archivo_hash_final);

	FILE* archivo_inicial = fopen(path_inicial, "w" );
	if (archivo_inicial == NULL){
		log_error(logger, "No se pudo crear el archivo origen a hashear %s.\n", path_inicial);
		exit(EXIT_FAILURE);
	}

	fwrite(cadena_a_hashear, strlen(cadena_a_hashear), 1, archivo_inicial);
	fclose(archivo_inicial);

	char* comando = string_new();
	string_append_with_format(&comando, "md5sum %s >", path_inicial);
	string_append_with_format(&comando, "%s", path_final);

	system(comando);


	FILE* archivo_final = fopen(path_final, "r");
	if (archivo_final == NULL){
		log_error(logger, "No se pudo abrir el archivo hash en %s.\n", path_final);
		exit(EXIT_FAILURE);
	}
	long int tamanio_hash = 32;
	char* valor_hash = malloc(sizeof(char) * tamanio_hash);
	if(fread(valor_hash, tamanio_hash, 1, archivo_final) < 0) {
		log_error(logger, "No se pudo recuperar el Hash.\n");
	}
	fclose(archivo_final);

	char* hash_final = string_substring_until(valor_hash, tamanio_hash);
	strcat(hash_final, "\0");

	free(valor_hash);
	free(comando);
	free(path_inicial);
	free(path_final);
	free(path_archivo_hash_inicial);
	free(path_archivo_hash_final);
	return hash_final;
}




// Crear un nuevo archivo por defecto dentro de /Files
void crear_archivo_metadata_recurso(char* path_archivo){

	FILE* archivo = fopen( path_archivo , "w" );

	if (archivo == NULL){
		log_error(logger, "No se pudo crear el recurso %s.\n", path_archivo);
		exit(EXIT_FAILURE);
	}

	fclose(archivo);

	t_config* contenido_archivo = config_create(path_archivo);

	// Valores default del archivo
	config_set_value(contenido_archivo, "SIZE", "0");
	config_set_value(contenido_archivo, "BLOCK_COUNT", "0");
	config_set_value(contenido_archivo, "BLOCKS", "[]");
	config_set_value(contenido_archivo, "CARACTER_LLENADO", "0");
	config_set_value(contenido_archivo, "MD5_ARCHIVO", "0");

	config_save(contenido_archivo);

	config_destroy(contenido_archivo);

}


t_metadata* actualizar_archivo_metadata_recurso(char* path, char caracter_llenado, int32_t tamanio_recurso, char* nombre_recurso) {

	t_metadata* metadata_recurso = malloc(sizeof(t_metadata));
	metadata_recurso->size = leer_size_archivo(path, "SIZE");

	int nuevo_valor_size = metadata_recurso->size + tamanio_recurso;

	// TODO validar, si nuevo_valor_size == 0, entonces no tiene bloques
	// si tenia bloques, los tengo que liberar

	char* valor_string = string_new();
	asprintf(&valor_string, "%d", nuevo_valor_size);
	guardar_nuevos_datos_en_archivo(path, valor_string, "SIZE");

	char* caracter_string = string_new();
	string_append_with_format(&caracter_string, "%c", caracter_llenado);
	guardar_nuevos_datos_en_archivo(path, caracter_string, "CARACTER_LLENADO");

	metadata_recurso->bloques_asignados_anterior = leer_blocks_archivo(path, "BLOCKS");

	uint32_t cantidad_bloques_usados = cantidad_elementos(metadata_recurso->bloques_asignados_anterior);

	if(cantidad_bloques_usados == 0){

		t_list* lista_posiciones = obtener_array_bloques_a_usar(nuevo_valor_size);
		char* bloques = string_new();
		string_append_with_format(&bloques,"[");

		for(int i=0; i<list_size(lista_posiciones); i++){
			int posicion = (int) list_get(lista_posiciones, i);

			if(i == list_size(lista_posiciones)-1)
				string_append_with_format(&bloques,"%u", posicion);
			else{
				string_append_with_format(&bloques,"%u,", posicion);
			}
		}
		string_append_with_format(&bloques,"]");

		guardar_nuevos_datos_en_archivo(path, bloques, "BLOCKS");
	}
	else{

		uint32_t fragmentacion_interna = cantidad_bloques_usados*BLOCK_SIZE - metadata_recurso->size;

		t_list* lista_posiciones;
		if(nuevo_valor_size>fragmentacion_interna){
			lista_posiciones = obtener_array_bloques_a_usar(nuevo_valor_size-fragmentacion_interna);// 10 frag de 4  entocnes falta guardar 6
		}else{
			lista_posiciones = obtener_array_bloques_a_usar(0);
		}

		char* bloques = string_new();
		string_append_with_format(&bloques,"[");
		int recorrido=0;
		while(metadata_recurso->bloques_asignados_anterior[recorrido] != NULL){
			if(recorrido == cantidad_bloques_usados-1 && list_size(lista_posiciones)==0){
				string_append_with_format(&bloques, metadata_recurso->bloques_asignados_anterior[recorrido]);

			}else{
				string_append_with_format(&bloques, metadata_recurso->bloques_asignados_anterior[recorrido]);
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
		log_info(logger, "Se ocuparon los bloques %s\n", bloques);
		guardar_nuevos_datos_en_archivo(path, bloques, "BLOCKS");
	}


	char* cantidad_bloques_total = string_new();
	char** bloques_asignados = leer_blocks_archivo(path, "BLOCKS");
	asprintf(&cantidad_bloques_total, "%d", cantidad_elementos(bloques_asignados));
	guardar_nuevos_datos_en_archivo(path, cantidad_bloques_total, "BLOCK_COUNT");

	char* string_hash = string_new();

	asprintf(&string_hash, "%s", concatenar_contenido_blocks(metadata_recurso->bloques_asignados_anterior));

	printf("HASH A METER: %s\n", string_hash);
	printf("TAMAÑO HASH: %u\n", strlen(string_hash));

	char* hash = hash_MD5(string_hash, nombre_recurso);
	guardar_nuevos_datos_en_archivo(path, hash, "MD5_ARCHIVO");

	return metadata_recurso;
}


void crear_archivo_metadata_bitacora(char* path_completo){

	FILE* archivo = fopen( path_completo , "w" );

	if (archivo == NULL){
		log_error(logger, "No se pudo crear el archivo %s .\n", path_completo);
		exit(EXIT_FAILURE);
	}

	fclose(archivo);

	t_config* contenido_archivo = config_create(path_completo);

	// Valores default del archivo
	config_set_value(contenido_archivo, "SIZE", "0");
	config_set_value(contenido_archivo, "BLOCKS", "[]");

	config_save(contenido_archivo);
	config_destroy(contenido_archivo);

	contador_tripulantes++;

}


t_metadata* actualizar_archivo_metadata_bitacora(char* path, uint32_t tamanio_accion){

	t_metadata* metadata_bitacora = malloc(sizeof(t_metadata));
	metadata_bitacora->size = leer_size_archivo(path, "SIZE");

	int nuevo_valor_size = metadata_bitacora->size + tamanio_accion;
	char* valor_string;
	asprintf(&valor_string, "%d", nuevo_valor_size);
	guardar_nuevos_datos_en_archivo(path, valor_string, "SIZE");

	metadata_bitacora->bloques_asignados_anterior = leer_blocks_archivo(path, "BLOCKS");
	uint32_t cantidad_bloques_usados = cantidad_elementos(metadata_bitacora->bloques_asignados_anterior);

	if(cantidad_bloques_usados == 0){

		t_list* lista_posiciones = obtener_array_bloques_a_usar(tamanio_accion);
		char* bloques = string_new();
		string_append_with_format(&bloques,"[");

		for(int i=0; i<list_size(lista_posiciones); i++){
			int posicion = (int) list_get(lista_posiciones, i);

			if(i == list_size(lista_posiciones)-1)
				string_append_with_format(&bloques,"%u", posicion);
			else{
				string_append_with_format(&bloques,"%u,", posicion);
			}
		}
		string_append_with_format(&bloques,"]");

		guardar_nuevos_datos_en_archivo(path, bloques, "BLOCKS");
	}
	else{

		uint32_t fragmentacion_interna = cantidad_bloques_usados*BLOCK_SIZE - metadata_bitacora->size;

		t_list* lista_posiciones;
		if(tamanio_accion>fragmentacion_interna){
			lista_posiciones = obtener_array_bloques_a_usar(tamanio_accion-fragmentacion_interna);// 10 frag de 4  entocnes falta guardar 6
		}else{
			lista_posiciones = obtener_array_bloques_a_usar(0);
		}

		char* bloques = string_new();
		string_append_with_format(&bloques,"[");
		int recorrido=0;
		while(metadata_bitacora->bloques_asignados_anterior[recorrido] != NULL){
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
		log_info(logger, "Se ocuparon los bloques %s\n", bloques);
		guardar_nuevos_datos_en_archivo(path, bloques, "BLOCKS");
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


char* concatenar_contenido_blocks(char** lista_bloques){
    int ubicacion;
    int posicion = 0;
    int cantidad_bloques = cantidad_elementos(lista_bloques);
    int tamanio_contenido = cantidad_bloques * BLOCK_SIZE;
    char* contenido_concatenado = malloc(tamanio_contenido+1);
    int desplazamiento = 0;


    while(lista_bloques[posicion] != NULL){
        ubicacion = atoi(lista_bloques[posicion]) * BLOCK_SIZE;
        memcpy(contenido_concatenado + desplazamiento, informacion_blocks + ubicacion, BLOCK_SIZE);
        desplazamiento += BLOCK_SIZE;
        posicion++;
    }

	char* contenido_final = string_substring_until(contenido_concatenado, tamanio_contenido);
	strcat(contenido_final, "\0");

	printf("HASH A METER: %s\n", contenido_final);
	printf("TAMAÑO HASH: %u\n", strlen(contenido_final));

    return contenido_concatenado;
}


void sincronizar(){
	while(1){
		sleep(TIEMPO_SINCRONIZACION);

		memcpy(blocks, informacion_blocks, BLOCK_SIZE*BLOCKS);
		if(msync(blocks, BLOCK_SIZE*BLOCKS, MS_SYNC) < 0){
			log_error(logger, "No se pudo sincronizar blocks.\n");
			return;
		}

		memcpy(super_bloque+sizeof(uint32_t)*2, bitmap, BLOCKS/8);
		if(msync(super_bloque, 2*sizeof(uint32_t)+BLOCKS/8, MS_SYNC) < 0){
			log_error(logger, "No se pudo sincronizar el super bloque.\n");
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

char* leer_caracter_archivo(char* path_archivo, char* clave) {

    t_config* datos_archivo = config_create(path_archivo);

    char* caracter = config_get_string_value(datos_archivo, clave);

    config_destroy(datos_archivo);

    return caracter;
}


char** leer_blocks_archivo(char* path_archivo, char* clave) {

	t_config* datos_archivo = config_create(path_archivo);

	char** blocks = config_get_array_value(datos_archivo, clave);

	config_destroy(datos_archivo);

	return blocks;
}


void guardar_nuevos_datos_en_archivo(char* path_archivo, void* valor, char* clave) {

	t_config* datos_metadata = config_create(path_archivo);

	config_set_value(datos_metadata, clave, valor);

	sem_wait(mutex_config);
	config_save(datos_metadata);
	sem_post(mutex_config);

	config_destroy(datos_metadata);

	free(valor);
}


char* armar_recurso(char caracter_llenado, uint32_t cantidad) {

	char* recurso = string_new();

	for(int i=0; i<cantidad; i++) {
		string_append_with_format(&recurso, "%c", caracter_llenado);
	}
	return recurso;
}


void loggear_liberacion_archivo(char* nombre, int nro_bloque){
	log_info(logger, "Se libera el bloque %i de %s.\n", nro_bloque, nombre);
}


char* crear_ruta_recurso(char* nombre_recurso) {

	char* path = string_new();
	string_append_with_format(&path, "/Files/%s.ims", nombre_recurso);
	char* path_completo = malloc(strlen(PUNTO_MONTAJE) + strlen(path) + 2);
	path_completo = concatenar_path(path);

	free(path);
	return path_completo;
}


char* crear_ruta_bitacora(int32_t id_tripulante) {

    char* path_string = string_new();
	string_append_with_format(&path_string, "/Files/Bitacoras/Tripulante%u.ims", id_tripulante);
	char* path_completo = malloc(strlen(PUNTO_MONTAJE) + strlen(path_string) + 2);
	path_completo = concatenar_path(path_string);

	free(path_string);
	return path_completo;
}



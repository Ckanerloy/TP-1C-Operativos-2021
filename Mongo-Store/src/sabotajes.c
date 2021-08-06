#include "sabotajes.h"


void iniciar_sabotaje(void){

	int32_t cantidad_sabotajes = cantidad_elementos(POSICIONES_SABOTAJE);

	if(num_sabotaje != cantidad_sabotajes) {
		char** posiciones_sabo = string_split(POSICIONES_SABOTAJE[num_sabotaje], "|");

		int32_t conexion_discordiador = crear_conexion(IP, PUERTO_DISCORDIADOR);

		posiciones* posicion = malloc(sizeof(posiciones));
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


void inicio_protocolo_fsck(void) {
	t_list* recursos_disponibles = recursos_activos();
	bool sabotaje = false;

/*
	// Sabotaje en SUPERBLOQUE: Modifica Cantidad de Bloques
	if(sabotaje_superbloque_cantidad_bloques()){
		log_info(logger, "Se realizó un Sabotaje en la Cantidad de Bloques del SuperBloque.\n");
		sabotaje = true;
		reparacion_superBloque_cantidad_bloques();
		log_info(logger, "[SABOTAJE SOLUCIONADO] Se reparó la Cantidad de Bloques del SuperBloque.\n");
	}
*/


	// Sabotaje en SUPERBLOQUE: Modifica el Bitmap
	if(sabotaje_superBloque_bitmap()){
		log_info(logger, "Se realizó un Sabotaje en el Bitmap del SuperBloque.\n");
		sabotaje = true;
		reparacion_superBloque_bitmap();
		log_info(logger, "[SABOTAJE SOLUCIONADO] Se reparó el Bitmap del SuperBloque.\n");
	}


	// Sabotaje en directorio FILES
	for(int i=0; i<list_size(recursos_disponibles); i++) {
		recursos_archivos recurso = (recursos_archivos)list_get(recursos_disponibles, i);

		// Sabotaje en FILES: Modifica el Size
		if(!mismo_size_archivo(recurso)){
			log_info(logger, "Se realizó un Sabotaje en el Size del Archivo %s.ims.\n", mapeo_recurso_a_string(recurso));
			sabotaje = true;
			reparar_size(recurso);
			log_info(logger, "[SABOTAJE SOLUCIONADO] Se reparó el Size del Archivo %s.ims.\n", mapeo_recurso_a_string(recurso));
		}

		// Sabotaje en FILES: Modifica el Block_Count
		if(!mismo_block_count_archivo(recurso)){
			log_info(logger, "Se realizó un Sabotaje en el Block_Count del Archivo %s.ims.\n", mapeo_recurso_a_string(recurso));
			sabotaje = true;
			reparar_block_count(recurso);
			log_info(logger, "[SABOTAJE SOLUCIONADO] Se reparó el Block_Count del Archivo %s.ims.\n", mapeo_recurso_a_string(recurso));
		}

		// Sabotaje en FILES: Modifica el Orden de los Bloques
		if(!bloques_ordenados_archivo(recurso)) {
			log_info(logger, "Se realizó un Sabotaje en el Orden de los Bloques del Archivo %s.ims.\n", mapeo_recurso_a_string(recurso));
			sabotaje = true;
			reparar_orden_bloques(recurso);

		}
	}


	if(sabotaje == false) {
		log_info(logger, "¡Falsa Alarma! No hubo sabotaje.\n");
	}

}


t_list* recursos_activos(void){
	t_list* recursos_activos = list_create();

	char* ruta_oxigeno = crear_ruta_recurso("Oxigeno");
	char* ruta_comida = crear_ruta_recurso("Comida");
	char* ruta_basura = crear_ruta_recurso("Basura");

	if(existe_archivo(ruta_oxigeno)){
		list_add(recursos_activos, OXIGENO);
	}
	if(existe_archivo(ruta_comida)){
		list_add(recursos_activos, COMIDA);
	}
	if(existe_archivo(ruta_basura)){
		list_add(recursos_activos, BASURA);
	}

	free(ruta_oxigeno);
	free(ruta_comida);
	free(ruta_basura);

	return recursos_activos;
}


char* mapeo_recurso_a_string(recursos_archivos recurso) {

	char* recurso_string = string_new();

	switch(recurso) {
		case OXIGENO:
			string_append_with_format(&recurso_string, "%s", "Oxigeno");
			break;
		case COMIDA:
			string_append_with_format(&recurso_string, "%s", "Comida");
			break;
		case BASURA:
			string_append_with_format(&recurso_string, "%s", "Basura");
			break;
		default:
			break;
	}

	return recurso_string;
}



bool sabotaje_superbloque_cantidad_bloques(void) {

	char* path_blocks = concatenar_path("/Blocks.ims");
	int tamanio_archivo = calcular_tamanio_archivo(path_blocks);
	int cantidad_blocks_real;
	uint32_t cantidad_blocks_metadata;

	memcpy(&cantidad_blocks_metadata, super_bloque + sizeof(uint32_t), sizeof(uint32_t));
	cantidad_blocks_real = tamanio_archivo/BLOCK_SIZE;

	//Retorna true si las cantidades son distintas. Sabotaje hecho.
	return cantidad_blocks_real != cantidad_blocks_metadata;
}


int calcular_tamanio_archivo(char* path_archivo){

	int tamanio_archivo;
	FILE* archivo = fopen(path_archivo, "rb");
	fseek(archivo, 0, SEEK_END);
	tamanio_archivo = (int)ftell(archivo);
	fclose(archivo);
	return tamanio_archivo;

}




bool mismo_size_archivo(recursos_archivos recurso){

	int tamanio_en_blocks = 0;
	char* path_recurso = mapeo_recurso_a_string(recurso);
	char* path_archivo_recurso = crear_ruta_recurso(path_recurso);
	char** bloques_usados = leer_blocks_archivo(path_archivo_recurso, "BLOCKS");
	int tamanio = leer_size_archivo(path_archivo_recurso, "SIZE");
	char* caracter_recurso = leer_caracter_archivo(path_archivo_recurso, "CARACTER_LLENADO");
	int cantidad_blocks_ocupados = cantidad_elementos(bloques_usados);

	for(int i=0; i<cantidad_blocks_ocupados; i++){
			int nro_bloque = list_get(bloques_usados, i);

			for(int c=0; c<BLOCK_SIZE; c++){

				char* caracter_copia = malloc(sizeof(char));
				int ubicacion_bloque = nro_bloque * BLOCK_SIZE;
				char* caracter = string_substring(caracter_copia, 0, 1);

				memcpy(caracter_copia , informacion_blocks + ubicacion_bloque + c, sizeof(char));

				if(strcmp(caracter, caracter_recurso)==0){

					tamanio_en_blocks++;

				}
				free(caracter_copia);
				free(caracter);
			}

	}

	free(path_recurso);
	free(path_archivo_recurso);

	if(tamanio_en_blocks == tamanio){
		return true;
	} else{
		return false;
	}
}

void reparar_size(recursos_archivos recurso){

	int tamanio_en_blocks = 0;
	char* path_recurso = mapeo_recurso_a_string(recurso);
	char* path_archivo_recurso = crear_ruta_recurso(path_recurso);
	char** bloques_usados = leer_blocks_archivo(path_archivo_recurso, "BLOCKS");
	char* caracter_recurso = leer_caracter_archivo(path_archivo_recurso, "CARACTER_LLENADO");
	int cantidad_blocks_ocupados = cantidad_elementos(bloques_usados);

	for(int i=0; i<cantidad_blocks_ocupados; i++){
				int nro_bloque = list_get(bloques_usados, i);

				for(int c=0; c<BLOCK_SIZE; c++){

					char* caracter_copia = malloc(sizeof(char));
					int ubicacion_bloque = nro_bloque * BLOCK_SIZE;
					char* caracter = string_substring(caracter_copia, 0, 1);

					memcpy(caracter_copia , informacion_blocks + ubicacion_bloque + c, sizeof(char));

					if(strcmp(caracter, caracter_recurso)==0){

						tamanio_en_blocks++;

					}
					free(caracter_copia);
					free(caracter);
				}

		}


	char* valor_string = string_new();
	asprintf(&valor_string, "%d", tamanio_en_blocks);
	guardar_nuevos_datos_en_archivo(path_archivo_recurso, valor_string, "SIZE");

	free(path_recurso);
	free(path_archivo_recurso);
	free(valor_string);
}


bool mismo_block_count_archivo(recursos_archivos recurso){

	char* path_recurso = mapeo_recurso_a_string(recurso);
	char* path_archivo_recurso = crear_ruta_recurso(path_recurso);
	char** bloques_usados = leer_blocks_archivo(path_archivo_recurso, "BLOCKS");
	int cantidad_blocks_ocupados = cantidad_elementos(bloques_usados);
	int cantidad_blocks_archivo = leer_size_archivo(path_archivo_recurso, "BLOCK_COUNT");

	limpiar_parser(bloques_usados);
	free(path_recurso);
	free(path_archivo_recurso);

	if(cantidad_blocks_ocupados == cantidad_blocks_archivo){
		return true;
	} else{
		return false;
	}
}

void reparar_block_count(recursos_archivos recurso){
	char* path_recurso = mapeo_recurso_a_string(recurso);
	char* path_archivo_recurso = crear_ruta_recurso(path_recurso);
	char** bloques_usados = leer_blocks_archivo(path_archivo_recurso, "BLOCKS");
	int cantidad_blocks_ocupados = cantidad_elementos(bloques_usados);

	char* cantidad_bloques_total = string_new();
	asprintf(&cantidad_bloques_total, "%d", cantidad_blocks_ocupados);
	guardar_nuevos_datos_en_archivo(path_archivo_recurso, cantidad_bloques_total, "BLOCK_COUNT");

	limpiar_parser(bloques_usados);
	free(path_recurso);
	free(path_archivo_recurso);
}


bool bloques_ordenados_archivo(recursos_archivos recurso){

	char* path_recurso = mapeo_recurso_a_string(recurso);
	char* path_archivo_recurso = crear_ruta_recurso(path_recurso);
	char* md5_original = leer_caracter_archivo(path_archivo_recurso , "MD5_ARCHIVO");
	char** bloques_usados = leer_blocks_archivo(path_archivo_recurso, "BLOCKS");
	char* string_hash = concatenar_contenido_blocks(bloques_usados);
	char* md5_a_validar = hash_MD5(string_hash, path_recurso);

	limpiar_parser(bloques_usados);
	free(path_recurso);
	free(path_archivo_recurso);
	free(string_hash);

	if(md5_original == md5_a_validar){
		free(md5_original);
		free(md5_a_validar);
		return true;
	}
	else {
		free(md5_original);
		free(md5_a_validar);
		return false;
	}
}

void reparar_orden_bloques(recursos_archivos recurso){

	char* path_recurso = mapeo_recurso_a_string(recurso);
	char* path_archivo_recurso = crear_ruta_recurso(path_recurso);
	int tamanio = leer_size_archivo(path_archivo_recurso, "SIZE");
	char** bloques_usados = leer_blocks_archivo(path_archivo_recurso, "BLOCKS");
	int cantidad_blocks_ocupados = cantidad_elementos(bloques_usados);
	int cantidad_blocks_archivo = leer_size_archivo(path_archivo_recurso, "BLOCK_COUNT");
	char* caracter_recurso = leer_caracter_archivo(path_archivo_recurso, "CARACTER_LLENADO");

	if(cantidad_blocks_ocupados != cantidad_blocks_archivo){
		agregar_bloque_faltante(bloques_usados, path_archivo_recurso, caracter_recurso, tamanio);
		log_info(logger, "[SABOTAJE SOLUCIONADO] Se reparó el Bloque faltante del Archivo %s.ims.\n", mapeo_recurso_a_string(recurso));

	}
	else {
		reordenar_bloques(bloques_usados, path_archivo_recurso, caracter_recurso, tamanio);
		log_info(logger, "[SABOTAJE SOLUCIONADO] Se reparó el Orden de los Bloques del Archivo %s.ims.\n", mapeo_recurso_a_string(recurso));

	}
	limpiar_parser(bloques_usados);
	free(path_recurso);
	free(path_archivo_recurso);

}

void agregar_bloque_faltante(char** bloques, char* path, char* caracter, int size){

	int bloque_faltante;

	for(int i=0; i<BLOCKS; i++){
		if(!esta_presente_en_lista(bloques, i) && bitarray_test_bit(bitArraySB, i)){
			bloque_faltante = i;
		}
	}

	int cantidad_blocks_ocupados = cantidad_elementos(bloques);
	bloques[cantidad_blocks_ocupados] = bloque_faltante;

    char* lista_bloques = string_new();
    string_append_with_format(&lista_bloques,"[");

    for(int i=0; i<cantidad_blocks_ocupados+1; i++){
	   int posicion = atoi(bloques[i]);

	   if(i == atoi(bloques[i])-1)
		   string_append_with_format(&lista_bloques,"%u", posicion);
	   else{
		   string_append_with_format(&lista_bloques,"%u,", posicion);
	   }
    }
    string_append_with_format(&lista_bloques ,"]");
	guardar_nuevos_datos_en_archivo(path, lista_bloques, "BLOCKS");

	uint32_t desplazamiento = 0;
	for(int i=0; i<cantidad_blocks_ocupados; i++) {
		char* valor = armar_recurso(caracter, BLOCK_SIZE);
		uint32_t nro_bloque = atoi(bloques[i]);
        uint32_t ubicacion_bloque = nro_bloque * BLOCK_SIZE;
        memcpy(informacion_blocks + ubicacion_bloque, valor + desplazamiento, BLOCK_SIZE);
        desplazamiento += BLOCK_SIZE;
	 }

	 uint32_t nro_ultimo_bloque = atoi(bloques[cantidad_blocks_ocupados]);
	 uint32_t espacio_libre_ultimo_bloque = (cantidad_blocks_ocupados * BLOCK_SIZE - size);
	 uint32_t cant_necesaria_ultimo_bloque = BLOCK_SIZE - espacio_libre_ultimo_bloque;
	 char* valor = armar_recurso(caracter, cant_necesaria_ultimo_bloque);
	 uint32_t ubicacion_bloque = nro_ultimo_bloque * BLOCK_SIZE;
	 memcpy(informacion_blocks + ubicacion_bloque, valor + desplazamiento, cant_necesaria_ultimo_bloque);
	 desplazamiento += cant_necesaria_ultimo_bloque;
	 valor = armar_recurso("0", espacio_libre_ultimo_bloque);
	 ubicacion_bloque += desplazamiento;
	 memcpy(informacion_blocks + ubicacion_bloque, valor + desplazamiento, espacio_libre_ultimo_bloque);

}

void reordenar_bloques(char** bloques, char* path, char* caracter, int size){

	int cantidad_blocks_ocupados = cantidad_elementos(bloques);
	uint32_t desplazamiento = 0;

	for(int i=0; i<cantidad_blocks_ocupados - 1; i++) {
		char* valor = armar_recurso(caracter, BLOCK_SIZE);
		uint32_t nro_bloque = atoi(bloques[i]);
        uint32_t ubicacion_bloque = nro_bloque * BLOCK_SIZE;
        memcpy(informacion_blocks + ubicacion_bloque, valor + desplazamiento, BLOCK_SIZE);
        desplazamiento += BLOCK_SIZE;
	 }
	 uint32_t nro_ultimo_bloque = atoi(bloques[cantidad_blocks_ocupados - 1]);
	 uint32_t espacio_libre_ultimo_bloque = (cantidad_blocks_ocupados * BLOCK_SIZE - size);
	 uint32_t cant_necesaria_ultimo_bloque = BLOCK_SIZE - espacio_libre_ultimo_bloque;
	 char* valor = armar_recurso(caracter, cant_necesaria_ultimo_bloque);
	 uint32_t ubicacion_bloque = nro_ultimo_bloque * BLOCK_SIZE;
	 memcpy(informacion_blocks + ubicacion_bloque, valor + desplazamiento, cant_necesaria_ultimo_bloque);
	 desplazamiento += cant_necesaria_ultimo_bloque;
	 valor = armar_recurso("0", espacio_libre_ultimo_bloque);
	 ubicacion_bloque += desplazamiento;
	 memcpy(informacion_blocks + ubicacion_bloque, valor + desplazamiento, espacio_libre_ultimo_bloque);
}



void reparacion_superBloque_cantidad_bloques(void) {

	int cantidad_blocks_real;
	char* path_blocks = concatenar_path("/Blocks.ims");
	int tamanio_archivo = calcular_tamanio_archivo(path_blocks);
	cantidad_blocks_real = tamanio_archivo/BLOCK_SIZE;

	memcpy(super_bloque + sizeof(uint32_t), &cantidad_blocks_real, sizeof(uint32_t));

}

t_list* obtener_blocks_ocupados_total(){
	int posicion;
	t_list* lista_recursos = recursos_activos();
	t_list* lista_bloques_ocupados = list_create();

	for(int i=0; i<list_size(lista_recursos); i++){

		recursos_archivos recurso = (recursos_archivos) list_get(lista_recursos, i);
		char* path_recurso = mapeo_recurso_a_string(recurso);
		char* path_archivo_recurso = crear_ruta_recurso(path_recurso);
		char** bloques_usados = leer_blocks_archivo(path_archivo_recurso, "BLOCKS");

		posicion = 0;
		while(bloques_usados[posicion] != NULL){
			list_add(lista_bloques_ocupados, atoi(bloques_usados[posicion]));
			posicion++;
		}
		free(path_recurso);
		free(path_archivo_recurso);
		limpiar_parser(bloques_usados);
	}

	for(int c=1; c<=contador_tripulantes; c++){
		char* path_archivo_bitacora = crear_ruta_bitacora(c);
		char** bloques_usados_bitacora = leer_blocks_archivo(path_archivo_bitacora, "BLOCKS");

		posicion = 0;
		while(bloques_usados_bitacora[posicion] != NULL){
			list_add(lista_bloques_ocupados, atoi(bloques_usados_bitacora[posicion]));
			posicion++;
		}
	free(path_archivo_bitacora);
	limpiar_parser(bloques_usados_bitacora);

	}

	return lista_bloques_ocupados;
}



bool sabotaje_superBloque_bitmap(void) {

	char* bitmap_aux = malloc(BLOCKS/8);
	memcpy(bitmap_aux, super_bloque + 2*sizeof(uint32_t), BLOCKS/8);
	t_bitarray* bitmap_SB = bitarray_create_with_mode(bitmap_aux, BLOCKS/8, LSB_FIRST);
	t_list* lista_bloques_en_uso = obtener_blocks_ocupados_total();

	for(int i=0; i<BLOCKS; i++){

			//Si cumple cualquiera de las dos condiciones, bitmap saboteado.
			if((!esta_presente_en_lista(lista_bloques_en_uso, i) && bitarray_test_bit(bitmap_SB, i)) || (esta_presente_en_lista(lista_bloques_en_uso, i) && !bitarray_test_bit(bitmap_SB, i))){
				bitarray_destroy(bitmap_SB);
				list_destroy(lista_bloques_en_uso);
				free(bitmap_aux);
				return true;
			}
		}
		bitarray_destroy(bitmap_SB);
		list_destroy(lista_bloques_en_uso);
		free(bitmap_aux);
		return false;
}

bool esta_presente_en_lista(t_list* lista, int valor) {


	for(int i = 0; i<list_size(lista); i++) {
		if((int) list_get(lista, i) == valor) {
			return true;
		}
	}
	return false;
}




void reparacion_superBloque_bitmap(void) {

	vaciarBitArray(bitArraySB);
	t_list* lista_bloques_usados = obtener_blocks_ocupados_total();
	int cantidad_lista_bloques_usados = list_size(lista_bloques_usados);

	for(int i=0; i<cantidad_lista_bloques_usados; i++){
		bitarray_set_bit(bitArraySB, list_get(lista_bloques_usados, i));
	}
	memcpy(super_bloque+2*sizeof(uint32_t), bitmap, BLOCKS/8);
}



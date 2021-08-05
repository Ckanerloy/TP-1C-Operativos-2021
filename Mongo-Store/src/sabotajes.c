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



/*
SABOTAJE EN SUPERBLOQUE:
- CANTIDAD DE BLOQUES:
	. Cambiar el valor del campo Blocks del SuperBloque.ims
	. Como se resuelve? Constatar contra el tamaño de Blocks.ims y corregir el valor en caso que no concuerde
- BITMAP:
	. Cambiar el valor de algun bit del campo Bitmap (0 por 1 o 1 por 0)
	. Como se resuelve? Recorrer todos los archivos dentro de Files y Files/Bitacoras y obtener de todos los archivos los bloques usados


SABOTAJE EN FILES
- SIZE:
	. Cambiar el valor del campo SIZE del archivo
	. Como se resuelve? Reocrrer todos los bloques del archivo y obtener y asumir como correcto el tamaño encontrado dentro de los bloques recorriéndolos en orden
		cuando se consuma oxigeno, hay que poner una marca para avisar hasta donde es
- BLOCK_COUNT y BLOCKS:
	. El valor de BLOCK_COUNT y BLOCKS son inconsistentes
	. Actualizar el valor de BLOCK_COUNT en base a la lista de BLOCKS
- BLOCKS:
	. El valor de la lista de BLOCKS fue alterado y los bloques no están en orden
	. Validar con el valor de MD5_ARCHIVO y restaurar el archivo (escribir en archivo tantos caracteres de llenado como hagan falta hasta completar el size)
 */


void inicio_protocolo_fsck(void) {
	t_list* recursos_disponibles = recursos_activos();
	bool sabotaje = false;


	// Sabotaje en SUPERBLOQUE: Modifica Cantidad de Bloques
	if(sabotaje_superbloque_cantidad_bloques()){
		log_info(logger, "Se realizó un Sabotaje en la Cantidad de Bloques del SuperBloque.\n");
		sabotaje = true;
		reparacion_superbloque_cantidad_bloques();
		log_info(logger, "[SABOTAJE SOLUCIONADO] Se reparó la Cantidad de Bloques del SuperBloque.\n");
	}

	// Sabotaje en SUPERBLOQUE: Modifica el Bitmap
	if(sabotaje_superbloque_bitmap()){
		log_info(logger, "Se realizó un Sabotaje en el Bitmap del SuperBloque.\n");
		sabotaje = true;
		reparacion_superbloque_bitmap();
		log_info(logger, "[SABOTAJE SOLUCIONADO] Se reparó el Bitmap del SuperBloque.\n");
	}



	for(int i=0; i<list_size(recursos_disponibles); i++) {
		recursos_archivos recurso = (recursos_archivos)list_get(recursos_disponibles, i);

		// Sabotaje en FILES: Modifica el Size
		if(!mismo_size_archivo(recurso)){
			log_info(logger, "Se realizó un Sabotaje en el Size del Archivo %s.\n", mapeo_recurso_a_string(recurso));
			sabotaje = true;
			reparar_size(recurso);
			log_info(logger, "[SABOTAJE SOLUCIONADO] Se reparó el Size del Archivo %s.\n", mapeo_recurso_a_string(recurso));
		}

		// Sabotaje en FILES: Modifica el Block_Count
		if(!mismo_block_count_archivo(recurso)){
			log_info(logger, "Se realizó un Sabotaje en el Block_Count del Archivo %s.\n", mapeo_recurso_a_string(recurso));
			sabotaje = true;
			reparar_block_count(recurso);
			log_info(logger, "[SABOTAJE SOLUCIONADO] Se reparó el Block_Count del Archivo %s.\n", mapeo_recurso_a_string(recurso));
		}

		// Sabotaje en FILES: Modifica el Orden de los Bloques
		if(!bloques_ordenados_archivo(recurso)) {
			log_info(logger, "Se realizó un Sabotaje en el Orden de los Bloques del Archivo %s.\n", mapeo_recurso_a_string(recurso));
			sabotaje = true;
			reparar_orden_bloques(recurso);
			log_info(logger, "[SABOTAJE SOLUCIONADO] Se reparó el ORden de los Bloques del Archivo %s.\n", mapeo_recurso_a_string(recurso));
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
			string_append_with_format(&recurso_string, "%s", "Oxigeno.ims");
			break;
		case COMIDA:
			string_append_with_format(&recurso_string, "%s", "Comida.ims");
			break;
		case BASURA:
			string_append_with_format(&recurso_string, "%s", "Basura.ims");
			break;
		default:
			break;
	}

	return recurso_string;
}



bool sabotaje_superbloque_cantidad_bloques(void) {

	int BLOCK_SIZE * BLOCKS;

	int lengthOfFile = tamanioFile(rutaBlocks());
	uint32_t cantidadBloquesMetadata;
	memcpy(&cantidadBloquesMetadata, superbloqueMapeadoAMemoria + sizeof(uint32_t), sizeof(uint32_t));
	int cantidadBloquesCalculada = lengthOfFile/tamanioBloques;

	return cantidadBloquesCalculada != cantidadBloquesMetadata;
}

void reparacion_superbloque_cantidad_bloques(void) {

}


bool sabotaje_superbloque_bitmap(void) {

}

void reparacion_superbloque_bitmap(void) {

}

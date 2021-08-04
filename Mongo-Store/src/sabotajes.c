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



void inicio_rutina_fsck(void) {


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
}

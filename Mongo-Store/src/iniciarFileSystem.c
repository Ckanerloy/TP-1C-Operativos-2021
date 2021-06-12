#include "iniciarFileSystem.h"


void iniciarFileSystem(char* PUNTO_MONTAJE){

	//Primero debemos comprobar si existe un FS, sino recién ahí lo creo
	//Leer bien los siguientes issues
	//https://github.com/sisoputnfrba/foro/issues/2053
	//https://github.com/sisoputnfrba/foro/issues/2078
	//https://github.com/sisoputnfrba/foro/issues/2102


}



//Se valida la existencia del FileSystem mediante la comprobación de /Blocks.ims
int existeFileSystem(){

	int existeArchivo = 0;
	char* nombreArchivoValidacion= malloc(strlen(PUNTO_MONTAJE) + strlen("/Blocks.ims") + 1);

	strcpy(nombreArchivoValidacion, PUNTO_MONTAJE);
	strcat(nombreArchivoValidacion, "/Blocks.ims");

	FILE* archivoParaValidar = fopen(nombreArchivoValidacion, "r");


	if (archivoParaValidar == NULL){
		existeArchivo = 0;
	}
	else
		existeArchivo = 1;

		fclose(archivoParaValidar);

	free(archivoParaValidar);

	return existeArchivo;

}

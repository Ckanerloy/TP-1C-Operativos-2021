#include "iniciarFileSystem.h"
#include <sys/types.h>

 #include <fcntl.h>
#include <sys/mman.h>



void inicializar_file_system(uint32_t tamanio_bloque, uint32_t cantidad_bloques){

		//creacion_directorio(PUNTO_MONTAJE);
		crear_superbloque(tamanio_bloque,cantidad_bloques);
	//	crear_blocks(tamanio_bloque,cantidad_bloques);

		/*
		creacion_directorio(carpeta_files);
		creacion_directorio(carpeta_bitacoras);
		log_info(logger_i_mongo_store, "FILESYSTEM INICIALIZADO DE 0 CON EXITO");
		*/
}


char* creacion_directorio(char* direccion_puntoMontaje, char* nombre_carpeta){


	char* direccion_carpeta = malloc(strlen(direccion_puntoMontaje) + strlen(nombre_carpeta) + 1); //Asigno espacio de memoria para el nombre total del directorio

	strcpy(direccion_carpeta, direccion_puntoMontaje);
	strcat(direccion_carpeta, nombre_carpeta); //Concateno el nombre del directorio

	mkdir(direccion_carpeta, 0777); //Hago uso de la función mkdir para crear el directorio () en el modo predeterminado 0777 (acceso más amplio posible)

	return direccion_carpeta;
}





void crear_superbloque(uint32_t tamanio_bloque, uint32_t cantidad_bloques){

	char* direccion_superBloque= malloc(strlen(PUNTO_MONTAJE) + strlen("/SuperBloque.ims") + 1);

	strcpy(direccion_superBloque, PUNTO_MONTAJE);
	strcat(direccion_superBloque, "/SuperBloque.ims");

	int archivo = open(direccion_superBloque, O_CREAT | O_RDWR, 0777); //O_CREAT = si el fichero no existe, será creado. O_RDWR = lectura y escritura
	if(archivo == -1){
		printf("No se pudo abrir el archivo SuperBloque");
		exit(-1);
	}

	fallocate(archivo,0,0,2*sizeof(uint32_t)+cantidad_bloques);

	void *super_bloque = mmap(NULL, 2*sizeof(uint32_t)+cantidad_bloques, PROT_WRITE | PROT_READ, MAP_SHARED,archivo,0); //Mediante PROT_READ Y PROT_WRITE permitimos leer y escribir. MAP_SHARED permite uqe las operaciones realizadas en el área de mapeo se reflejen en el disco

	if(super_bloque == MAP_FAILED){

		printf("No se pudo mapear el archivo SuperBloque");

		exit(-1);
	}
	/*
	memcpy(super_bloque,&tamaño_bloque,sizeof(uint32_t));
	memcpy(super_bloque+sizeof(uint32_t),&cantidad_bloques,sizeof(uint32_t));

	int offset = 2*sizeof(uint32_t);
	bool estado = 0;
	for(int i = 0; i<block_amount;i++){
		memcpy(superbloque+offset,&estado,sizeof(bool));
		offset+=sizeof(bool);
	}
	msync(superbloque,2*sizeof(uint32_t)+block_amount,0);
	*/
}


//Se valida la existencia del FileSystem mediante la comprobación de /Blocks.ims
int existe_file_system(){

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

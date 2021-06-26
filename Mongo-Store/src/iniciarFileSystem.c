#include "iniciarFileSystem.h"
#include "memory.h"
#include <sys/types.h>

 #include <fcntl.h>
#include <sys/mman.h>




void inicializar_file_system(){

		//creacion_directorio(PUNTO_MONTAJE, "");
		crear_superbloque();
		//crear_blocks(tamanio_bloque,cantidad_bloques);

		/*
		creacion_directorio(PUNTO_MONTAJE, "Files");
		creacion_directorio(PUNTO_MONTAJE, "Bitacoras");
		log_info(logger_i_mongo_store, "FILESYSTEM INICIALIZADO DE 0 CON EXITO");
		*/
}


void creacion_directorio(char* direccion_punto_montaje, char* nombre_directorio){


	char* direccion_carpeta = malloc(strlen(direccion_punto_montaje) + strlen(nombre_directorio) + 2); //Asigno espacio de memoria para el nombre total del directorio

	strcpy(direccion_carpeta, direccion_punto_montaje);
	strcat(direccion_carpeta, "/");
	strcat(direccion_carpeta, nombre_directorio); //Concateno el nombre del directorio

	mkdir(direccion_carpeta, 0777); //Hago uso de la función mkdir para crear el directorio () en el modo predeterminado 0777 (acceso más amplio posible)
	//printf("%s", direccion_carpeta);
	//return direccion_carpeta; //TODO No olvidarme de liberar memoria en MongoStore
}





void crear_superbloque(){

	char* direccion_superBloque= malloc(strlen(PUNTO_MONTAJE) + strlen("/SuperBloque.ims") + 1);
	int flag = 1;
	int archivo;
	void *super_bloque;
	struct stat statbuf;



	strcpy(direccion_superBloque, PUNTO_MONTAJE);
	strcat(direccion_superBloque, "/SuperBloque.ims");


	bloque_t* superBloqueFile = malloc(sizeof(bloque_t));
	superBloqueFile->tamanioBloque = BLOCK_SIZE;
	superBloqueFile->cantidadBloques = BLOCKS;

	char *segmento_bitmap = malloc(superBloqueFile->cantidadBloques/8);
	t_bitarray *bitmap = bitarray_create_with_mode(segmento_bitmap, sizeof(superBloqueFile->cantidadBloques/8), "LSB_FIRST");
	superBloqueFile->bitmap = bitmap;
	//O_CREAT = si el fichero no existe, será creado. O_RDWR = lectura y escritura
	archivo = open(direccion_superBloque, O_CREAT | O_RDWR, S_IRUSR|S_IWUSR);

	if( -1 == archivo)
	  {
	    perror("Error al abrir el archivo \n");
	    exit(1);
	  }

	ftruncate(archivo, 2*sizeof(uint32_t)+superBloqueFile->cantidadBloques/8);//Asigno memoria al archivo
	fstat(archivo, &statbuf);




//Mediante PROT_READ Y PROT_WRITE permitimos leer y escribir. MAP_SHARED permite uqe las operaciones realizadas en el área de mapeo se reflejen en el disco
	super_bloque = mmap(NULL, statbuf.st_size + sizeof(uint32_t), PROT_WRITE | PROT_READ, MAP_SHARED,archivo,0);
	segmento_bitmap = mmap(NULL, statbuf.st_size + sizeof(uint32_t)+superBloqueFile->cantidadBloques/8, PROT_READ | PROT_WRITE, MAP_SHARED, archivo, sizeof(uint32_t) * 2);



	if(super_bloque == MAP_FAILED){
		perror("Error mapping \n");
		exit(1);

	}

	if(segmento_bitmap == MAP_FAILED){
			perror("Error mapping bitmap----------- \n");
			//exit(1);

		}


		if(flag == 1){
			msync(super_bloque, 2*sizeof(uint32_t)+superBloqueFile->cantidadBloques/8, MS_SYNC);


			memcpy(super_bloque, &(superBloqueFile->tamanioBloque), sizeof(uint32_t));
			memcpy(super_bloque+sizeof(uint32_t), &(superBloqueFile->cantidadBloques), sizeof(uint32_t));
			memcpy(super_bloque+sizeof(uint32_t)*2, &(superBloqueFile->bitmap), superBloqueFile->cantidadBloques/8);
//el bitmap se guarda en cantidad de bloques / 8 bytes

			//msync(super_bloque, 2*sizeof(uint32_t)+superBloqueFile->cantidadBloques, MS_SYNC);
			//msync(super_bloque, 2*sizeof(uint32_t)+superBloqueFile->cantidadBloques/8, MS_SYNC);



}

	//free(direccion_superBloque);



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




#include "iniciarFileSystem.h"
#include "memory.h"
#include <sys/types.h>

 #include <fcntl.h>
#include <sys/mman.h>


char *concatenar_path(char* path){

	char* path_completo= malloc(strlen(PUNTO_MONTAJE) + strlen(path) + 1);

	strcpy(path_completo, PUNTO_MONTAJE);
	strcat(path_completo, path);

	return path_completo;

}

//Se valida la existencia del FileSystem mediante la comprobación de /Blocks.ims
int existe_file_system(){

	int existeArchivo;
	char *nombreArchivoValidacion = concatenar_path("/Blocks.ims");

	//Intento abrir el archivo Blocks.ims
	//Si retorna -1 quiere decir que no lo pudo abrir
	existeArchivo = open(nombreArchivoValidacion, O_RDONLY, S_IRUSR);


	return existeArchivo;

}

void inicializar_file_system(){


		creacion_directorio(PUNTO_MONTAJE, "");//Se crea el path /home/utnso/polus TODO cambiar los parámetros que recibe para no poner ""
		creacion_directorio(PUNTO_MONTAJE, "Files");//Se crea el path /home/utnso/polus/Files
		creacion_directorio(PUNTO_MONTAJE, "Files/Bitacoras");//Se crea el path /home/utnso/polus/Files/Bitacoras
		iniciar_superbloque();
		crear_archivo_blocks();


		/*
		log_info(logger_i_mongo_store, "FILESYSTEM INICIALIZADO DE 0 CON EXITO");
		*/
}




void creacion_directorio(char* direccion_punto_montaje, char* nombre_directorio){


	char* direccion_carpeta = malloc(strlen(direccion_punto_montaje) + strlen(nombre_directorio) + 2); //Asigno espacio de memoria para el nombre total del directorio

	strcpy(direccion_carpeta, direccion_punto_montaje);
	strcat(direccion_carpeta, "/");
	strcat(direccion_carpeta, nombre_directorio); //Concateno el nombre del directorio

	//Hago uso de la función mkdir para crear el directorio en el modo predeterminado 0777 (acceso más amplio posible)
	mkdir(direccion_carpeta, 0777);

	//return direccion_carpeta; //TODO No olvidarme de liberar memoria en MongoStore
}





void iniciar_superbloque(){

	char *direccion_superBloque = concatenar_path("/SuperBloque.ims");
	int archivo;
	void *super_bloque;
	struct stat statbuf;
	char* un_bitarray = malloc(BLOCKS/8);
	t_bitarray* bitArraySB = crear_bitarray(un_bitarray);
	vaciarBitArray(bitArraySB);


	bloque_t* superBloqueFile = malloc(sizeof(bloque_t));
	superBloqueFile->tamanioBloque = BLOCK_SIZE;
	superBloqueFile->cantidadBloques = BLOCKS;


	//O_CREAT = si el fichero no existe, será creado. O_RDWR = lectura y escritura
	archivo = open(direccion_superBloque, O_CREAT | O_RDWR, S_IRUSR|S_IWUSR);

	if( -1 == archivo)
	  {
	    perror("Error al abrir el archivo SuperBloque.ims \n");//Revisar print del error
	    exit(1);
	  }

	//Trunco espacio al archivo
	ftruncate(archivo, 2*sizeof(uint32_t)+superBloqueFile->cantidadBloques/8);
	fstat(archivo, &statbuf);


	//Mediante PROT_READ Y PROT_WRITE permitimos leer y escribir. MAP_SHARED permite uqe las operaciones realizadas en el área de mapeo se reflejen en el disco
	super_bloque = mmap(NULL, statbuf.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, archivo,0);


	if(super_bloque == MAP_FAILED){
		perror("Error mapping \n");
		exit(1);

	}


			//msync(super_bloque, 2*sizeof(uint32_t)+superBloqueFile->cantidadBloques/8, MS_SYNC);

			memcpy(super_bloque, &(superBloqueFile->tamanioBloque), sizeof(uint32_t));
			memcpy(super_bloque+sizeof(uint32_t), &(superBloqueFile->cantidadBloques), sizeof(uint32_t));
			memcpy(super_bloque+sizeof(uint32_t)*2, un_bitarray, superBloqueFile->cantidadBloques/8);

			msync(super_bloque, 2*sizeof(uint32_t)+superBloqueFile->cantidadBloques/8, MS_SYNC);





	//free(direccion_superBloque);



}




void crear_archivo_blocks(){

	struct stat statbuf;
	int archivo_blocks;
	void *blocks;
	char *direccion_blocks = concatenar_path("/Blocks.ims");
	//O_CREAT = si el fichero no existe, será creado. O_RDWR = lectura y escritura
	archivo_blocks = open(direccion_blocks, O_CREAT | O_RDWR, S_IRUSR|S_IWUSR);

	ftruncate(archivo_blocks, BLOCK_SIZE*BLOCKS);
	/*fstat(archivo_blocks, &statbuf);

	blocks = mmap(NULL, statbuf.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, archivo_blocks, 0);*/


}







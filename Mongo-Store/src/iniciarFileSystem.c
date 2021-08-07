#include "iniciarFileSystem.h"
#include "memory.h"
#include <sys/types.h>

#include <fcntl.h>
#include <sys/mman.h>


char* concatenar_path(char* path){

	char* path_completo = malloc(strlen(PUNTO_MONTAJE) + strlen(path) + 1);

	strcpy(path_completo, PUNTO_MONTAJE);
	strcat(path_completo, path);

	return path_completo;
}


//Se valida la existencia del FileSystem mediante la comprobación de /Blocks.ims
int existe_file_system(void){

	int existe_archivo;
	char* archivo_validacion = concatenar_path("/Blocks.ims");

	//Intento abrir el archivo Blocks.ims
	//Si retorna -1 quiere decir que no lo pudo abrir
	existe_archivo = open(archivo_validacion, O_RDONLY, S_IRUSR);

	free(archivo_validacion);
	return existe_archivo;
}


void inicializar_file_system(void){

		creacion_directorio(PUNTO_MONTAJE, "");//Se crea el path /home/utnso/polus TODO cambiar los parámetros que recibe para no poner ""
		creacion_directorio(PUNTO_MONTAJE, "Files");//Se crea el path /home/utnso/polus/Files
		creacion_directorio(PUNTO_MONTAJE, "Files/ArchivosHash");//Se crea el path /home/utnso/polus/Files/ArchivosHash
		creacion_directorio(PUNTO_MONTAJE, "Files/Bitacoras");//Se crea el path /home/utnso/polus/Files/Bitacoras
		iniciar_superbloque();
		crear_archivo_blocks();

		log_info(logger, "FileSystem inicializado con éxito");
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


void iniciar_superbloque(void){

	char *direccion_superBloque = concatenar_path("/SuperBloque.ims");
	struct stat statbuf;
	char* bitmap = malloc(BLOCKS/8);

	//Metadata para el superbloque
	bloque_t* superBloqueFile = malloc(sizeof(bloque_t));
	superBloqueFile->tamanioBloque = BLOCK_SIZE;
	superBloqueFile->cantidadBloques = BLOCKS;

	//Creo bitmap y lo inicializo en 0
	bitArraySB = crear_bitarray(bitmap);
	vaciarBitArray(bitArraySB);

	//O_CREAT = si el fichero no existe, será creado. O_RDWR = lectura y escritura
	archivo = open(direccion_superBloque, O_CREAT | O_RDWR, S_IRUSR|S_IWUSR);

	if( -1 == archivo)
	  {
	    log_error(logger, "Error al abrir el archivo SuperBloque.ims \n");//Revisar print del error
	    exit(1);
	  }
	//Trunco espacio del archivo
	ftruncate(archivo, 2*sizeof(uint32_t)+superBloqueFile->cantidadBloques/8);
	fstat(archivo, &statbuf);

	//Mediante PROT_READ Y PROT_WRITE permitimos leer y escribir. MAP_SHARED permite uqe las operaciones realizadas en el área de mapeo se reflejen en el disco
	super_bloque = mmap(NULL, statbuf.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, archivo,0);
	if(super_bloque == MAP_FAILED){
		perror("Error mapping \n");
		exit(1);
	}
			memcpy(super_bloque, &(superBloqueFile->tamanioBloque), sizeof(uint32_t));
			memcpy(super_bloque+sizeof(uint32_t), &(superBloqueFile->cantidadBloques), sizeof(uint32_t));
			memcpy(super_bloque+sizeof(uint32_t)*2, bitArraySB->bitarray, superBloqueFile->cantidadBloques/8);

			if(msync(super_bloque, 2*sizeof(uint32_t)+superBloqueFile->cantidadBloques/8, MS_SYNC) < 0) {
				log_error(logger, "[msync] Error al sincronizar SuperBloque.ims.\n");
			}else{

				log_info(logger,"[msync] SuperBloque.ims sincronizado correctamente.\n");
			}
}


void levantar_archivo_blocks(void){

	informacion_blocks = malloc(BLOCKS*BLOCK_SIZE);
	uint32_t desplazamiento = 0;

	for(uint32_t i=0; i<BLOCKS; i++){

		memcpy(informacion_blocks + desplazamiento, blocks + desplazamiento, BLOCK_SIZE);
		desplazamiento += BLOCK_SIZE;
	}
}

void levantar_archivo_superBloque(void){

	char* bitmap = malloc(BLOCKS/8);
	bitArraySB = crear_bitarray(bitmap);
	memcpy(bitArraySB->bitarray, super_bloque+sizeof(uint32_t)*2, BLOCKS/8);
}

void crear_archivo_blocks(void){

	struct stat statbuf;
	char *direccion_blocks = concatenar_path("/Blocks.ims");
	int desplazamiento = 0;
	//O_CREAT = si el fichero no existe, será creado. O_RDWR = lectura y escritura
	archivo_blocks = open(direccion_blocks, O_CREAT | O_RDWR, S_IRUSR|S_IWUSR);

	ftruncate(archivo_blocks, BLOCK_SIZE*BLOCKS);
	fstat(archivo_blocks, &statbuf);

	blocks = mmap(NULL, statbuf.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, archivo_blocks, 0);

	//Se inicializa el archivo Blocks.ims en 0. Tomamos 0 como referencia para indicar que el bloque está vacío


	for(uint32_t i=0; i<BLOCK_SIZE*BLOCKS; i++){
		memcpy(blocks+desplazamiento, "0", 1);
		desplazamiento += 1;
	}

	if(msync(blocks, BLOCK_SIZE*BLOCKS, MS_SYNC) < 0) {
			log_error(logger,"[msync] Error al sincronizar Blocks.ims en su creación.\n");

		}else {

			log_info(logger,"[msync] Se creó y sincronizó el archivo Blocks.ims correctamente.\n");
		}
}


void escribir_archivo_blocks(uint32_t bloque, char* cadena_a_escribir, uint32_t longitud_cadena){

	//Multiplico por BLOCK_SIZE para ir al lugar en donde comienza el bloque
	uint32_t ubicacion_bloque = bloque * BLOCK_SIZE;

	memcpy(informacion_blocks+ubicacion_bloque, cadena_a_escribir, longitud_cadena);
}


#include "bitArray.h"

//Función que permite crear el bitarray/bitmap
t_bitarray* crear_bitarray(char* bitarray){
		return bitarray_create_with_mode(bitarray, BLOCKS/8, LSB_FIRST);

}

//Función que pone en 0 todos los bits del bitarray/bitmap
void vaciarBitArray(t_bitarray* bitarray){

	int i;

	for (i = 0; i < BLOCKS; i++){
		bitarray_clean_bit(bitarray, i);
	}
}

int posicionBitLibre (t_bitarray* bitarray){

	/*int i;

	for (i = 0; i < BLOCKS; i++){
		if( bitarray_clean_bit(bitarray, i);
	}
	bitarray_test_bit()*/
}

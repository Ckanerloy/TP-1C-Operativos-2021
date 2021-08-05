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

int posicionBitLibre (){

	for (int i = 0; i < BLOCKS; i++){
		int valorBit = bitarray_test_bit(bitArraySB, i);

		if( valorBit == 0){
			return i;
		}
	}
	return -1;

}



void llenarBitArray(t_bitarray* bitarray){

	int i;

	for (i = 0; i < 10; i++){
		bitarray_set_bit(bitarray, i);
	}
}


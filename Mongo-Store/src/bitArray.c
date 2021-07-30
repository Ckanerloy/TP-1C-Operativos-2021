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

	int bitLibre;

	for (int i = 0; i < BLOCKS; i++){
		int valorBit = bitarray_test_bit(bitarray, i);

		if( 0 ==  valorBit){
			bitLibre = i;
			i = BLOCKS+1;

		}

	}
	return bitLibre;

}



void llenarBitArray(t_bitarray* bitarray){

	int i;

	for (i = 0; i < 10; i++){
		bitarray_set_bit(bitarray, i);
	}
}


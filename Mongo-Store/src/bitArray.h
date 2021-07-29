#ifndef SRC_BITARRAY_H_
#define SRC_BITARRAY_H_

#include "Mongo-Store.h"
#include "commons/bitarray.h"



t_bitarray* crearBitArray(char*);
void vaciarBitArray(t_bitarray* bitarray);
int posicionBitLibre (t_bitarray* bitarray);
void llenarBitArray(t_bitarray* bitarray);

#endif /* SRC_BITARRAY_H_ */

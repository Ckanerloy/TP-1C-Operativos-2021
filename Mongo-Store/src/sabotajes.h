#ifndef SABOTAJES_H_
#define SABOTAJES_H_

#include "Mongo-Store.h"

char** POSICIONES_SABOTAJE;
int32_t num_sabotaje;

void iniciar_sabotaje(void);
void inicio_rutina_fsck(void);

#endif /* SABOTAJES_H_ */

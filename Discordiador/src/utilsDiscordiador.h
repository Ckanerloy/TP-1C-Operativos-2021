#ifndef UTILSDISCORDIADOR_H_
#define UTILSDISCORDIADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>

#include"utils/estructuras.h"

uint32_t mapeo_valor_consola(char* comando_ingresado);

#endif /* UTILSDISCORDIADOR_H_ */

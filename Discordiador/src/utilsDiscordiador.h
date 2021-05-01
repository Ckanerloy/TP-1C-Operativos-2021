#ifndef UTILSDISCORDIADOR_H_
#define UTILSDISCORDIADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>

int crear_conexion(char* ip, char* puerto);

#endif /* UTILSDISCORDIADOR_H_ */

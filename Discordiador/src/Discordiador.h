#ifndef DISCORDIADOR_H_
#define DISCORDIADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "utilsDiscordiador.h"

#define CONFIG_PATH "/home/utnso/tp-2021-1c-UTNIX/Discordiador/discordiador.config"

char* IP_MI_RAM;
char* PUERTO_MI_RAM;
char* IP_MONGO_STORE;
char* PUERTO_MONGO_STORE;

t_config* config;


t_log* iniciar_logger();
void obtenerDatosDeConfig(char*);
t_paquete* armar_paquete(void);

#endif /* DISCORDIADOR_H_ */

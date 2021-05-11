#ifndef MI_RAM_H_
#define MI_RAM_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "utils/sockets.h"

#define IP "127.0.0.1"
#define CONFIG_PATH "/home/utnso/tp-2021-1c-UTNIX/Mi-RAM-HQ/Mi-RAM.config"


// Patota Control Block (PCB)
typedef struct {
	uint32_t id_patota;
	void* direccion_tareas;
}pcb;


// Tripulante Control Block (TCB)
typedef struct {
	uint32_t id_tripulante;
	char estado_tripulante;
	uint32_t posicion_x;
	uint32_t posicion_y;
	uint32_t id_proxima_instruccion;
}tcb;


#endif /* MI_RAM_H_ */

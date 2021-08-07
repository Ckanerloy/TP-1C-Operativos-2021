#ifndef BITACORA_H_
#define BITACORA_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include "commons/string.h"
#include "utils/estructuras.h"
#include "Discordiador.h"


void armar_bitacora(void* mensaje, codigo_bitacora codigo, uint32_t id_tripulante);
void enviar_bitacora(bitacora* bitacora_tripu);
bitacora* bitacora_movimiento(bitacora_posiciones* posiciones);
bitacora* bitacora_ejecucion_tarea(tripulante_plani* tripu);
bitacora* bitacora_termina_tarea(tripulante_plani* tripu);
bitacora* bitacora_corre_sabotaje(char* valor);
bitacora* bitacora_resuelve_sabotaje(char* valor);

#endif /* BITACORA_H_ */

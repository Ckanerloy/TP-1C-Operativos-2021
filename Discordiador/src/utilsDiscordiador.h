#ifndef UTILSDISCORDIADOR_H_
#define UTILSDISCORDIADOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "commons/log.h"
#include "commons/string.h"
#include "commons/config.h"
#include "Discordiador.h"
#include "utils/estructuras.h"
#include "bitacora.h"

codigo_operacion mapeo_valor_consola(char* comando_ingresado);

uint32_t cantidad_argumentos_ingresados(char** parser_consola);

//t_tcb* crear_tripulante(t_datos_hilo* datos_hilo);

void terminar_tripulante(tripulante_plani* tripu);
void enviar_tarea_io(tripulante_plani* tripu, codigo_operacion op_code, char* nombre_archivo, char caracter);
bitacora_posiciones* preparar_bitacora_posi(posiciones* posicion_tripu, posiciones* posiciones_nuevas);

#endif /* UTILSDISCORDIADOR_H_ */

#include "utilsDiscordiador.h"


uint32_t mapeo_valor_consola(char* comando_ingresado) {

	uint32_t operacion;

	if(strcmp(comando_ingresado, "INICIAR_PLANIFICACION") == 0) {
		operacion = INICIAR_PLANIFICACION;
	}

	if(strcmp(comando_ingresado, "PAUSAR_PLANIFICACION") == 0) {
		operacion = PAUSAR_PLANIFICACION;
	}

	if(strcmp(comando_ingresado, "INICIAR_PATOTA") == 0) {
		operacion = INICIAR_PATOTA;
	}

	if(strcmp(comando_ingresado, "EXPULSAR_TRIPULANTE") == 0) {
		operacion = EXPULSAR_TRIPULANTE;
	}

	if(strcmp(comando_ingresado, "LISTAR_TRIPULANTES") == 0) {
		operacion = LISTAR_TRIPULANTES;
	}

	if(strcmp(comando_ingresado, "OBTENER_BITACORA") == 0) {
		operacion = OBTENER_BITACORA;
	}

	return operacion;
}


void terminar_programa(t_log* logger, t_config* config)
{
	config_destroy(config);
	printf("Puntero a archivo .config destruido.\n");
	log_destroy(logger);
	printf("Puntero al logger destruido.\n");
}

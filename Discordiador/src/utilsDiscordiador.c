#include "utilsDiscordiador.h"


codigo_operacion mapeo_valor_consola(char* comando_ingresado) {

	codigo_operacion operacion;

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


// Funcion para LISTAR TRIPULANTES
void listar_tripulantes() {
	printf("-------------------------------------------------------------------------\n");
	printf("Estado de la nave: %s \n", temporal_get_string_time("%d/%m/%y %H:%M:%S"));
	printf("Tripulante: ID_TRIPULANTE      Patota: NUM_PATOTA      Status:  STATUS \n");

	printf("--------------------------------------------------------------------------\n\n");
}


// Obtener la cantidad de argumentos de parte del parser_consola
uint32_t cantidad_argumentos_ingresados(char** parser_consola){  // la vamos a usar tanto para verifiaciones de ingresos
	int cantidad=0;									   			 // por consola, como para cantidad de tripu que arrancan en 0,0

	while(parser_consola[cantidad] != NULL){
		cantidad++;
	}
	return cantidad;
}



/*

int main(int n, char **args) {
	printf("FCFS: FIRST COME FIRST SERVERED");
	int np=11, procesos[10];
	double tf = 0, tp;// tiempo promedio.
	while (np > 10 || np <= 0) {
		printf("\nNumero de procesos: ");
		scanf("%d", &np);
	}
	//para i=0, mientras i<np, hacer:...
	// pedimos el tamaño de cada proceso.
	for(int i=0; i<np; i++) {
		printf("\nInserte el proceso %d :", i+1);
		scanf("%d", &procesos[i]);
	}
	// Algoritmo FCFS
	for(int i=0; i<np; i++) {
		tf += procesos[i];
		tp = tp + tf;
		printf("\nProceso %d, concluye en %2.1f", i+1, tf);
	}
	printf("\n-------------------------------");
	printf("\nLa suma de los procesos %2.1f", tp);
	tp = tp / np;
	printf("\n\nTiempo promedio en FCFS fue de: %2.2f:", tp);
	return 0;
}*/

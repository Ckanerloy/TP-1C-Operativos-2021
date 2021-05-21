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


t_tripulante* crear_tripulante(uint32_t id, char* posicion_x, char* posicion_y) {

	t_tripulante* tripulante = malloc(sizeof(t_tripulante));

	tripulante->id_tripulante = id;

	tripulante->tamanio_estado_tripulante = strlen("NUEVO");
	tripulante->estado_tripulante = malloc(tripulante->tamanio_estado_tripulante + 1);
	strcpy(tripulante->estado_tripulante, "NUEVO");

	tripulante->posicion_x = atoi(posicion_x);

	tripulante->posicion_y = atoi(posicion_y);

	tripulante->peso_tripulante = sizeof(tripulante->id_tripulante) + sizeof(tripulante->tamanio_estado_tripulante) + strlen(tripulante->estado_tripulante)+1 + sizeof(tripulante->posicion_x) + sizeof(tripulante->posicion_y);

	return tripulante;
}

void crear_tripulanteV2(t_datos_hilo* datos_hilo) {

	t_tcb* tripulante = malloc(sizeof(t_tcb));

	tripulante->id_tripulante = datos_hilo->id;

	//tripulante->estado_tripulante = malloc(sizeof(char));
	tripulante->estado_tripulante = 'N';

	tripulante->posicion_x = datos_hilo->posicion_x;

	tripulante->posicion_y = datos_hilo->posicion_y;

	tripulante->id_proxima_instruccion = 0;
}

void mostrar_tripulante(t_tripulante* tripulante) {

	printf("Id tripulante: %u \n", tripulante->id_tripulante);
	printf("Estado tripulante: %s \n", tripulante->estado_tripulante);
	printf("Posicion X: %i \n", tripulante->posicion_x);
	printf("Posicion Y: %i \n", tripulante->posicion_y);
}

void liberar_tripulantes(uint32_t cantidad_tripulantes, t_tripulante** mensaje_tripulantes) {
	for(uint32_t c=1; c<=cantidad_tripulantes; c++){
		free(mensaje_tripulantes[c]);
	}
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

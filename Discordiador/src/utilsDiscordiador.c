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

	if(strcmp(comando_ingresado, "TERMINAR_PROGRAMA") == 0) {
		operacion = TERMINAR_PROGRAMA;
	}

	return operacion;
}

void leer_archivo_tareas(FILE* archivo_tareas, char* tareas) {
	char* tarea = string_new();

	while(!feof(archivo_tareas)){
		fscanf(archivo_tareas, "%s ", tarea);
		string_append_with_format(&tareas, "%s ", tarea);
	}
	printf("%s \n", tareas);
}


void mostrar_tripulante(t_tcb* tripulante) {

	printf("Id tripulante: %u \n", tripulante->id_tripulante);
	printf("Estado tripulante: %c \n", tripulante->estado_tripulante);
	printf("Posicion X: %i \n", tripulante->posicion_x);
	printf("Posicion Y: %i \n", tripulante->posicion_y);
	printf("Id proxima instruccion a realizar: %i \n\n", tripulante->id_tarea_a_realizar);
}


// Obtener la cantidad de argumentos de parte del parser_consola
uint32_t cantidad_argumentos_ingresados(char** parser_consola){  // la vamos a usar tanto para verifiaciones de ingresos
	int cantidad=0;									   			 // por consola, como para cantidad de tripu que arrancan en 0,0

	while(parser_consola[cantidad] != NULL){
		cantidad++;
	}
	return cantidad;
}


void terminar_tripulante(tripulante_plani* tripu) {

	t_tripulante* tripulante_terminado = malloc(sizeof(t_tripulante));

	sem_wait(mutex_exit);
	queue_push(cola_exit, tripu);
	sem_post(mutex_exit);

	tripu->estado = 'T';

	free(tripu->tarea_a_realizar);
	free(tripu->sem_tripu);
	free(tripu->mutex_peticion);
	free(tripu->sem_planificacion);
	free(tripu->mutex_expulsado);

	conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);

	tripulante_terminado->id_patota = tripu->numero_patota;
	tripulante_terminado->id_tripulante = tripu->id_tripulante;

	if(resultado_conexion(conexion_mi_ram, logger, "Mi-RAM HQ") == -1){
		log_error(logger, "No se pudo lograr la conexion con Mi-RAM.\n");
		abort();
	}

	enviar_mensaje(tripulante_terminado, EXPULSAR_TRIPULANTE, conexion_mi_ram);

	log_info(logger_on, "Se expuls?? al Tripulante %u.\n", tripulante_terminado->id_tripulante);

	cerrar_conexion(logger,conexion_mi_ram);

	free(tripulante_terminado);
}


void enviar_tarea_io(tripulante_plani* tripu, codigo_operacion op_code, char* nombre_archivo, char caracter) {

	archivo_tarea* tarea_io = malloc(sizeof(archivo_tarea));

	strcat(nombre_archivo, "\0");

	tarea_io->cantidad = tripu->tarea_a_realizar->cantidad;
	tarea_io->tamanio_nombre = strlen(nombre_archivo);
	tarea_io->nombre_archivo = malloc(tarea_io->tamanio_nombre+1);
	strcpy(tarea_io->nombre_archivo, nombre_archivo);
	tarea_io->caracter_llenado = caracter;

	conexion_mongo_store = crear_conexion(IP_MONGO_STORE, PUERTO_MONGO_STORE);

	if(resultado_conexion(conexion_mongo_store, logger, "i-Mongo Store") == -1){
		log_error(logger, "No se pudo lograr la conexion con i-Mongo Store.\n");
		abort();
	}

	enviar_mensaje(tarea_io, op_code, conexion_mongo_store);

	free(tarea_io->nombre_archivo);
	free(tarea_io);
	close(conexion_mongo_store);
}


bitacora_posiciones* preparar_bitacora_posi(posiciones* posicion_tripu, posiciones* posiciones_nuevas) {

	bitacora_posiciones* bitacora_posi = malloc(sizeof(bitacora_posiciones));
	bitacora_posi->posicion_anterior = malloc(sizeof(posiciones));
	bitacora_posi->posicion_anterior->posicion_x = posicion_tripu->posicion_x;
	bitacora_posi->posicion_anterior->posicion_y = posicion_tripu->posicion_y;

	bitacora_posi->posicion_nueva = malloc(sizeof(posiciones));
	bitacora_posi->posicion_nueva->posicion_x = posiciones_nuevas->posicion_x;
	bitacora_posi->posicion_nueva->posicion_y = posiciones_nuevas->posicion_y;

	return bitacora_posi;
}



void sabotaje_resuelto(void) {

	conexion_mongo_store = crear_conexion(IP_MONGO_STORE, PUERTO_MONGO_STORE);

	if(resultado_conexion(conexion_mongo_store, logger, "i-Mongo Store") == -1){
		log_error(logger, "No se pudo lograr la conexion con i-Mongo Store.\n");
		abort();
	}

	enviar_mensaje("", REALIZAR_SABOTAJE, conexion_mongo_store);

	close(conexion_mongo_store);
}


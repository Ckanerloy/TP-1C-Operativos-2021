#include "planificador.h"

algoritmo_planificacion mapeo_algoritmo_planificacion(char* algoritmo) {

	algoritmo_planificacion algoritmo_elegido;

	//FIFO
	if(strcmp(algoritmo,"FIFO") == 0)
	{
		algoritmo_elegido = FIFO;
	}

	//RR
	if(strcmp(algoritmo,"RR") == 0)
	{
		algoritmo_elegido = RR;
	}
	return algoritmo_elegido;
}

void inicializar_semaforos_plani(){
	contador_tripulantes_en_new = malloc(sizeof(sem_t));
	sem_init(contador_tripulantes_en_new,0, 0);

	mutex_new = malloc(sizeof(sem_t));
	sem_init(mutex_new, 0 , 1);

	mutex_ready = malloc(sizeof(sem_t));
	sem_init(mutex_ready, 0, 1);

	planificacion_on = malloc(sizeof(sem_t));
	sem_init(planificacion_on, 0, 0);
}


void finalizar_semaforos_plani() {
	free(contador_tripulantes_en_new);
	free(mutex_new);
	free(mutex_ready);
	free(planificacion_on);
}


void obtener_planificacion_de_config(t_config* config){

	GRADO_MULTITAREA = config_get_int_value(config, "GRADO_MULTITAREA");
	ALGORITMO = config_get_string_value(config, "ALGORITMO");
	QUANTUM = config_get_int_value(config, "QUANTUM");
}


void elegir_algoritmo() {

	algoritmo_planificacion algoritmo_elegido;
	algoritmo_elegido = mapeo_algoritmo_planificacion(ALGORITMO);

	t_tcb* tripulante_planificado;

	switch(algoritmo_elegido){

		case FIFO:
			printf("Eligio el algoritmo FIFO.\n");
			sem_wait(sem_ready);
			tripulante_planificado = list_remove(cola_ready, 0);
			sem_post(sem_ready);
			break;

		case RR:
			printf("Eligio el algoritmo Round Robin con un Quantum de %u. \n", QUANTUM);
			//tripulante_planificado = obtener_siguiente_rr();
			break;

		default:
			printf("No se eligio ningún algoritmo.\n");
			break;
	}
}





void iniciar_planificacion() {

	cola_new = queue_create();
	//cola_ready = list_create();
	cola_ready = queue_create();
    //cola_block = list_create();
	cola_block = queue_create();

    inicializar_semaforos_plani();

    //pthread_t hilo_block_ready;
	//pthread_create(&hilo_block_ready, NULL, (void*)block_ready, NULL);

	pthread_t hilo_new_ready;
	pthread_create(&hilo_new_ready, NULL, (void*)new_ready, NULL);

	//pthread_t threadHilosMaestro;
	//pthread_create(&threadHilosMaestro, NULL, (void*)hiloCiclosMaestro, NULL);

	// Genero un hilo por cada estado exec
//	for (int i = 0; i < GRADO_MULTITAREA; i++){

	//	pthread_t hilo_execute;
		//int* numero_hilo = malloc(sizeof(int));

		//*numero_hilo = i;

		//pthread_create(&hilo_execute, NULL, (void*)execute, numero_hilo);
	//}

	//finalizar_semaforos_plani();
}

void execute(int* numero_hilo) {

}

/*
void execute(int* numero_hilo) {
	int numero_hilo_exec = *numero_hilo;

	while(1){

		t_tcb* tripulante_a_trabajar = obtener_siguiente_de_ready();

		if (tripulante_a_trabajar != NULL){
            sem_wait(semLog);
			log_info(logger, "[EXEC-%d] Ingresa el repartidor %d con el pedido de idGlobal: %d e idResto: %d."
					" Instruciones restantes para arribar al objetivo actual: %d",
					numHiloExec, pedidoAEjecutar->repartidorAsignado->numeroRepartidor
					,pedidoAEjecutar->pedidoIDGlobal, pedidoAEjecutar->pedidoID, pedidoAEjecutar->instruccionesTotales);
            sem_post(semLog);
			int cantidadCiclos = 1;

			// Calculo si tiene que desalojar o no y por que razon
			while (sigoEjecutando(tripulante_a_trabajar)){
				waitSemaforoHabilitarCicloExec(numero_hilo_exec);

				if(tripulante_a_trabajar->repartidorAsignado->movimientosRequeridosEnX == 0){

					if(tripulante_a_trabajar->repartidorAsignado->movimientosRequeridosEnY > 0){
						tripulante_a_trabajar->repartidorAsignado->movimientosRequeridosEnY--;
						tripulante_a_trabajar->repartidorAsignado->posY++;
					} else {
						tripulante_a_trabajar->repartidorAsignado->movimientosRequeridosEnY++;
						tripulante_a_trabajar->repartidorAsignado->posY--;
					}

				} else {

					if(tripulante_a_trabajar->repartidorAsignado->movimientosRequeridosEnX > 0){
						tripulante_a_trabajar->repartidorAsignado->movimientosRequeridosEnX--;
						tripulante_a_trabajar->repartidorAsignado->posX++;
					} else {
						tripulante_a_trabajar->repartidorAsignado->movimientosRequeridosEnX++;
						tripulante_a_trabajar->repartidorAsignado->posX--;
					}

				}
				tripulante_a_trabajar->instruccionesRealizadas++;
				tripulante_a_trabajar->repartidorAsignado->instruccionesRealizadas++;
				sem_wait(semLog);
				log_info(logger, "[EXEC-%d] Repartidor %d invierte ciclo en moverse hacia la posicion: [%d|%d] con el pedido de idGlobal: %d e idResto: %d."
						" Para llegar al objetivo actual, todavia requiere %d movimiento/s adicional/es."
						,numHiloExec , pedidoAEjecutar->repartidorAsignado->numeroRepartidor
						,pedidoAEjecutar->repartidorAsignado->posX, pedidoAEjecutar->repartidorAsignado->posY
						,pedidoAEjecutar->pedidoIDGlobal
					    ,pedidoAEjecutar->pedidoID,pedidoAEjecutar->instruccionesTotales - pedidoAEjecutar->instruccionesRealizadas);
                sem_post(semLog);
				signalSemaforoFinalizarCicloExec(numero_hilo_exec);
				cantidadCiclos++;
			}

			agregarABlock(tripulante_a_trabajar);

		} else {
			//*waitSemaforoHabilitarCicloExec(numHiloExec);
			sem_wait(semLog);
			log_trace(logger, "[EXEC-%d] Desperdicio un ciclo porque no hay nadie en ready.", numHiloExec);
			sem_post(semLog);
			signalSemaforoFinalizarCicloExec(numHiloExec);
		}

	}

}
*/


void new_ready() {
	while(1){
		sem_wait(planificacion_on);

		sem_wait(contador_tripulantes_en_new);

		sem_wait(mutex_new);


		//printf("%s","aca");
		tripulante_plani* tripulante_a_ready = malloc(sizeof(tripulante_plani));

		tripulante_a_ready = queue_pop(cola_new);
		queue_push(cola_ready, tripulante_a_ready);
		//free(tripulante_a_ready);
	//	printf("id %u",tripulante_a_ready->id_tripulante);

		sem_post(mutex_new);



	//	sem_wait(mutex_ready);

		//queue_push(cola_ready, tripulante_a_ready);

	//	sem_post(mutex_ready);

		sem_post(planificacion_on);

		//IDEAS PIOLAS A IMPLEMENTAR
		//char* tarea_nueva = string_new();
		//obtener_siguiente_tarea(tarea_nueva); // Busca en la lista de tareas de la patota, la tarea siguiente para ejecutar
		//char* tarea_nueva = queue_pop(colaNew);


		//sem_wait(contadorRepartidoresDisp);


		//asignar_tarea_a_tripulante(tarea_nueva); //Le paso una tarea como parametro?

		//cambiar_estado_del_tripulante NEW -> READY

	}
}

/*
void block_ready() {

}

t_tripulante* obtener_siguiente_ready() {

	t_tripulante* tripulante_planificado = NULL;

	if (list_size(cola_ready) > 0){

	elegir_algoritmo();

	}

	// Devuelve NULL si no hay nada en ready
	// Caso contrario devuelve el que tiene mas prioridad segun el algoritmo que se este empleando
	return tripulante_planificado;
}
*/

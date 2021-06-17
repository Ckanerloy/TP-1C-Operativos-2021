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

	planificacion_on_ready_running = malloc(sizeof(sem_t));
	sem_init(planificacion_on_ready_running, 0, 0);

	mutex_valorMultitarea = malloc(sizeof(sem_t));
	sem_init(mutex_valorMultitarea, 0 ,1);

	sem_a_block = malloc(sizeof(sem_t));
	sem_init(sem_a_block, 0, 0);

	ya_pase_a_block = malloc(sizeof(sem_t));
	sem_init(ya_pase_a_block, 0, 0);

	contador_tripulantes_en_ready = malloc(sizeof(sem_t));
	sem_init(contador_tripulantes_en_ready,0 ,0);

	mutex_exit = malloc(sizeof(sem_t));
	sem_init(mutex_exit, 0, 0);
}


void finalizar_semaforos_plani() {
	free(contador_tripulantes_en_new);
	free(mutex_new);
	free(mutex_ready);
	free(mutex_exit);
	free(planificacion_on);
	free(planificacion_on_ready_running);
	free(mutex_valorMultitarea);
	free(sem_a_block);
	free(contador_tripulantes_en_ready);
}


void obtener_planificacion_de_config(t_config* config){

	GRADO_MULTITAREA = config_get_int_value(config, "GRADO_MULTITAREA");
	ALGORITMO = config_get_string_value(config, "ALGORITMO");
	QUANTUM = config_get_int_value(config, "QUANTUM");
	RETARDO_CICLO_CPU = config_get_int_value(config, "RETARDO_CICLO_CPU");
}


void elegir_algoritmo() {

	algoritmo_planificacion algoritmo_elegido;
	algoritmo_elegido = mapeo_algoritmo_planificacion(ALGORITMO);

	switch(algoritmo_elegido){

		case FIFO:
			printf("Eligio el algoritmo FIFO.\n");
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

	cola_exit = queue_create();

	multitarea_Disponible = GRADO_MULTITAREA;

	lista_semaforos_tripulantes = list_create();

	inicializar_semaforos_plani();


	//finalizar_semaforos_plani();
}

void new_ready() {
	while(1){
		sem_wait(contador_tripulantes_en_new);

		sem_wait(planificacion_on);

		tripulante_plani* tripulante_a_ready = malloc(sizeof(tripulante_plani));

		sem_wait(mutex_new);
		tripulante_a_ready = queue_pop(cola_new);
		sem_post(mutex_new);

		sem_post(tripulante_a_ready->sem_tripu);

		sem_wait(mutex_ready);
		queue_push(cola_ready, tripulante_a_ready);
		sem_post(mutex_ready);

		//Actualizar el estado del tripulante (R) EN Mi-Ram

		sem_post(planificacion_on);
		sem_post(contador_tripulantes_en_ready);

	}
}

void ready_running() {
	while(1){

		sem_wait(planificacion_on_ready_running);

		sem_wait(contador_tripulantes_en_ready);

		tripulante_plani* tripulante_a_running = malloc(sizeof(tripulante_plani));

		if(multitarea_Disponible != 0) {
			sem_wait(mutex_ready);
			tripulante_a_running = queue_pop(cola_ready);
			sem_post(mutex_ready);

			sem_wait(mutex_valorMultitarea);
			multitarea_Disponible--;
			sem_post(mutex_valorMultitarea);

			//Actualizar el estado (a E) en Mi-Ram

			sem_post(tripulante_a_running->sem_tripu);
		}

		sem_post(planificacion_on_ready_running);
		sleep(RETARDO_CICLO_CPU);
	}
}

void running_ready(tripulante_plani* tripulante){

	sem_wait(mutex_ready);
	queue_push(cola_ready, tripulante);
	sem_post(mutex_ready);
	//Actualizar el estado del tripulante (R) EN Mi-Ram
}

void running_block(tripulante_plani* tripulante){
	//Actualizar el estado del tripulante (B) EN Mi-Ram
	sem_wait(mutex_valorMultitarea);
	multitarea_Disponible++;
	sem_post(mutex_valorMultitarea);
}

void block_ready(tripulante_plani* tripu){
	sem_wait(mutex_ready);
	queue_push(cola_ready, tripu);
	sem_post(mutex_ready);

	//Actualizar el estado del tripulante (R) EN Mi-Ram
}

void block_exit(tripulante_plani* tripu){
	sem_wait(mutex_exit);
	queue_push(cola_exit, tripu);
	sem_post(mutex_exit);

	//Actualizar el estado del tripulante (E) EN Mi-Ram
}

void tripulante_hilo(void* tripulante){
	tripulante_plani* tripu = tripulante;

	sem_wait(tripu->sem_tripu);

	printf("%u",tripu->id_tripulante);

	/*
	tripu->tarea_a_realizar= obtener_siguiente_tarea(tripu->numero_patota);

	posiciones* posicion_tripu;
	posicion_tripu = malloc(sizeof(posiciones));
	posicion_tripu = obtener_posiciones(tripu->id_tripulante);

	while(tripu->tarea_a_realizar != NULL){
		sem_wait(tripu->sem_tripu);
		//sem_wait(mutex_valorMultitarea);
		posiciones* posicion_tarea;
		posicion_tarea = malloc(sizeof(posiciones));
		posicion_tarea->posicion_x = tripu->tarea_a_realizar->posicion_x;
		posicion_tarea->posicion_y = tripu->tarea_a_realizar->posicion_y;

		uint32_t distancia = obtener_distancia(posicion_tripu,posicion_tarea);
		uint32_t cantidadRealizado = 0;

		while(distancia != 0){

			if(cantidadRealizado==QUANTUM){
				running_ready(tripu);
				sem_wait(mutex_valorMultitarea);
				multitarea_Disponible++;
				sem_post(mutex_valorMultitarea);
				cantidadRealizado=0;
				sem_wait(tripu->sem_tripu);
			}

			sem_wait(tripu->sem_tripu);
			sleep(RETARDO_CICLO_CPU);
			//posicion_tripu = obtener_nueva_posicion(posicion_tripu,posicion_tarea);  Hay que actualizar la ubicacion en Mi_Ram
			cantidadRealizado ++;
			distancia--;

			sem_post(tripu->sem_tripu);
		}
		if(cantidadRealizado==QUANTUM){
			running_ready(tripu);
			sem_wait(mutex_valorMultitarea);
			multitarea_Disponible++;
			sem_post(mutex_valorMultitarea);
			cantidadRealizado=0;
			sem_wait(tripu->sem_tripu);
		}
		realizar_tarea(tripu,&cantidadRealizado);
	}
*/
}

t_tarea* obtener_siguiente_tarea(uint32_t numero_patota){
	//le mandamos el numero de la patota a Mi-Ram y nos devuelve un char* tarea_tripulante
	//Si no hay una proxima tarea, devuelve un NULL

	//return obtener_la_tarea(tarea_tripulante);
}

posiciones* obtener_posiciones(uint32_t tripulante){
	//le mandamos el id del tripulante a Mi_Ram y nos dice su ubicacion

	//return posicion;
}

uint32_t obtener_distancia(posiciones* posicion_tripu, posiciones* posicion_tarea){
	return (abs(posicion_tripu->posicion_x - posicion_tarea->posicion_x) + abs(posicion_tripu->posicion_y - posicion_tarea->posicion_y) );
}

codigo_tarea mapeo_tareas_tripulantes(char* tarea) {

	codigo_tarea tarea_a_realizar;

	if(strcmp(tarea, "GENERAR_OXIGENO") == 0) {
		tarea_a_realizar = GENERAR_OXIGENO;
	}

	if(strcmp(tarea, "CONSUMIR_OXIGENO") == 0) {
		tarea_a_realizar = CONSUMIR_OXIGENO;
	}

	if(strcmp(tarea, "GENERAR_COMIDA") == 0) {
		tarea_a_realizar = GENERAR_COMIDA;
	}

	if(strcmp(tarea, "CONSUMIR_COMIDA") == 0) {
		tarea_a_realizar = CONSUMIR_COMIDA;
	}

	if(strcmp(tarea, "GENERAR_BASURA") == 0) {
		tarea_a_realizar = GENERAR_BASURA;
	}

	if(strcmp(tarea, "DESCARTAR_BASURA") == 0) {
		tarea_a_realizar = DESCARTAR_BASURA;
	}

	return tarea_a_realizar;
}


t_tarea* obtener_la_tarea(char* tarea_tripulante) {
	char** parser_tarea = string_split(tarea_tripulante, " ");

    char** parser_parametros = NULL;

    t_tarea* tarea_nueva = malloc(sizeof(t_tarea));

	tarea_nueva->operacion = mapeo_tareas_tripulantes(parser_tarea[0]);

    parser_parametros = string_split(parser_tarea[1], ";");

    tarea_nueva->cantidad = atoi(parser_parametros[0]);
    tarea_nueva->posicion_x = atoi(parser_parametros[1]);
    tarea_nueva->posicion_y = atoi(parser_parametros[2]);
    tarea_nueva->tiempo = atoi(parser_parametros[3]);

    free(parser_tarea);
    free(parser_parametros);
    return tarea_nueva;
}

void realizar_tarea(tripulante_plani* tripu, uint32_t* cantidadRealizado){

	switch(tripu->tarea_a_realizar->operacion) {

		case GENERAR_OXIGENO:
			generar_insumo("Oxigeno.ims", 'O', tripu);
			break;

		case CONSUMIR_OXIGENO:
			consumir_insumo("Oxigeno.ims", 'O', tripu);
			break;

		case GENERAR_COMIDA:
			generar_insumo("Comida.ims", 'C', tripu);
			break;

		case CONSUMIR_COMIDA:
			consumir_insumo("Comida.ims",'C', tripu);
			break;

		case GENERAR_BASURA:
			generar_insumo("Basura.ims", 'B', tripu);
			break;

		case DESCARTAR_BASURA:
			descartar_basura(tripu);
			break;

		default:
			otras_tareas(tripu,cantidadRealizado);
			break;
		}
	tripu->tarea_a_realizar= obtener_siguiente_tarea(tripu->numero_patota);
	if(tripu->tarea_a_realizar!=NULL){
		block_ready(tripu);
	}else{
		block_exit(tripu);
		sem_wait(tripu->sem_tripu);
	}
	//Es importante que sem_tripu quede en cero sino se autoejecuta.
}

void generar_insumo(char* nombre_archivo, char caracter_llenado,tripulante_plani* tripu) {

	sem_wait(tripu->sem_tripu);
	//llamar al i-mongo y gastar 1 ciclo de cpu
	sleep(RETARDO_CICLO_CPU);
	sem_post(tripu->sem_tripu);
	running_block(tripu);

	//if(SI ESTA EL ARCHIVO) {
	//	modificar_archivo(nombre_archivo, parametros->cantidad);
	//}
	//else {
	//	crear_archivo(nombre_archivo, caracter_llenado);
	//}

	uint32_t tiempo_restante = tripu->tarea_a_realizar->tiempo;
	//uint32_t tiempo_transcurrido = 0;

	while(tiempo_restante != 0){
		sem_wait(tripu->sem_tripu);
		sleep(RETARDO_CICLO_CPU);
		//tiempo_transcurrido++;
		tiempo_restante--;
		sem_post(tripu->sem_tripu);
	}

}

void consumir_insumo(char* nombre_archivo, char caracter_a_consumir,tripulante_plani* tripu) {

	sem_wait(tripu->sem_tripu);
	//llamar al i-mongo y gastar 1 ciclo de cpu
	sleep(RETARDO_CICLO_CPU);
	running_block(tripu);
	sem_post(tripu->sem_tripu);
	running_block(tripu);

	//if(SI ESTA EL ARCHIVO) {
		//modificar_archivo(nombre_archivo, parametros->cantidad);
	//}
	//else {
	//	crear_archivo(nombre_archivo, caracter_a_consumir);
	//}

	uint32_t tiempo_restante = tripu->tarea_a_realizar->tiempo;
	//uint32_t tiempo_transcurrido = 0;

	while(tiempo_restante != 0){
		sem_wait(tripu->sem_tripu);
		sleep(RETARDO_CICLO_CPU);
		//tiempo_transcurrido++;
		tiempo_restante--;
		sem_post(tripu->sem_tripu);
	}
}

void descartar_basura(tripulante_plani* tripu) {

	sem_wait(tripu->sem_tripu);
	//llamar al i-mongo y gastar 1 ciclo de cpu
	sleep(RETARDO_CICLO_CPU);
	running_block(tripu);
	sem_post(tripu->sem_tripu);
	running_block(tripu);

	//if(SI ESTA EL ARCHIVO) {
	//	eliminar_archivo("Basura.ims");
	//}
	//else {
	//	log_info(logger, "El archivo 'Basura.ims' no existe. \n");
	//}

	uint32_t tiempo_restante = tripu->tarea_a_realizar->tiempo;
	//uint32_t tiempo_transcurrido = 0;

	while(tiempo_restante != 0){
		sem_wait(tripu->sem_tripu);
		sleep(RETARDO_CICLO_CPU);
		//tiempo_transcurrido++;
		tiempo_restante--;
		sem_post(tripu->sem_tripu);
	}
}

void otras_tareas(tripulante_plani* tripu,uint32_t* cantidadRealizado){
	uint32_t tiempo_restante = tripu->tarea_a_realizar->tiempo;

	while(tiempo_restante != 0){
		sem_wait(tripu->sem_tripu);
		if(*cantidadRealizado==QUANTUM){
			running_ready(tripu);
			sem_wait(mutex_valorMultitarea);
			multitarea_Disponible++;
			sem_post(mutex_valorMultitarea);
			*cantidadRealizado=0;
			sem_wait(tripu->sem_tripu);
		}
		sleep(RETARDO_CICLO_CPU);
		tiempo_restante--;
		sem_post(tripu->sem_tripu);
	}
}

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

	mutex_running = malloc(sizeof(sem_t));
	sem_init(mutex_running, 0, 1);

	cantidad_running = malloc(sizeof(sem_t));
	sem_init(cantidad_running , 0 ,0);

	mutex_valorMultitarea = malloc(sizeof(sem_t));
	sem_init(mutex_valorMultitarea, 0 ,1);

	sem_tripu= malloc(sizeof(sem_t));
	sem_init(sem_tripu, 0, 1);
}


void finalizar_semaforos_plani() {
	free(contador_tripulantes_en_new);
	free(mutex_new);
	free(mutex_ready);
	free(planificacion_on);
	free(planificacion_on_ready_running);
	free(mutex_running);
	free(cantidad_running);
	free(sem_tripu);
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

	cola_running = queue_create();
	//cola_running;
    //cola_block = list_create();
	cola_block = queue_create();

	multitarea_Disponible = GRADO_MULTITAREA;

	inicializar_semaforos_plani();


	//}

	//finalizar_semaforos_plani();
}

void new_ready() {
	while(1){
		sem_wait(planificacion_on);

		sem_wait(contador_tripulantes_en_new);

		tripulante_plani* tripulante_a_ready = malloc(sizeof(tripulante_plani));

		sem_wait(mutex_new);
		tripulante_a_ready = queue_pop(cola_new);
		sem_post(mutex_new);

		tripulante_a_ready->tarea_a_realizar = obtener_siguiente_tarea(tripulante_a_ready->numero_patota);

		sem_wait(mutex_ready);
		queue_push(cola_ready, tripulante_a_ready);
		sem_post(mutex_ready);

		sem_post(planificacion_on);

	}
}

void ready_running() {
	while(1){

		sem_wait(planificacion_on_ready_running);


		tripulante_plani* tripulante_a_running = malloc(sizeof(tripulante_plani));

		sem_wait(mutex_ready);
		tripulante_a_running = queue_pop(cola_ready);
		sem_post(mutex_ready);

		sem_wait(mutex_running);
		queue_push(cola_running, tripulante_a_running);
		sem_post(mutex_running);

		sem_post(cantidad_running);
		sem_post(planificacion_on_ready_running);

	}
}

void running_ready(){
	while(1){

			tripulante_plani* tripulante_a_ready = malloc(sizeof(tripulante_plani));

			sem_wait(mutex_running);
			tripulante_a_ready = queue_pop(cola_running);
			sem_post(mutex_running);

			sem_wait(mutex_ready);
			queue_push(cola_ready, tripulante_a_ready);
			sem_post(mutex_ready);


	}
}

void running_block(){
	while(1){
	sem_wait(sem_a_block);
	//Buscar en la cola de running con una variable global y llevarlo a la de blocj dejando el mismo orden
	//Esto o una funcion que le mandas parametro y hace lo mismo
	}
}

void tripulante_hilo(void* tripulante){
	tripulante_plani* tripu = tripulante;

	sem_wait(sem_tripu);


	t_tarea* tarea_a_realizar= obtener_siguiente_tarea(tripu->numero_patota);

	posiciones* posicion_tripu;
	posicion_tripu = malloc(sizeof(posiciones));
	posicion_tripu = obtener_posiciones(tripu->id_tripulante);

	while(tarea_a_realizar != NULL){
		sem_wait(sem_tripu);
		//sem_wait(mutex_valorMultitarea);
		posiciones* posicion_tarea;
		posicion_tarea = malloc(sizeof(posiciones));
		posicion_tarea->posicion_x = tarea_a_realizar->parametros->posicion_x;
		posicion_tarea->posicion_y = tarea_a_realizar->parametros->posicion_y;

		uint32_t distancia = obtener_distancia(posicion_tripu,posicion_tarea);

		while(distancia != 0){

			sem_wait(sem_tripu);
			sleep(RETARDO_CICLO_CPU);
			//posicion_tripu = obtener_nueva_posicion(posicion_tripu,posicion_tarea);  Hay que actualizar la ubicacion en Mi_Ram
			//cantidadRealizado ++;
			distancia--;
			sem_wait(sem_tripu);
		}
		realizar_tarea(tarea_a_realizar, tripulante);
		tarea_a_realizar= obtener_siguiente_tarea(tripu->numero_patota);


		// Le aviso a Mi-Ram que cambio de estado a ready e incremento cantidad tripu a ready
		//sem_post();  El hilo que te saca de running
	}
	//sem_post(running_exit);
}

t_tarea* obtener_siguiente_tarea(uint32_t numero_patota){
	//le mandamos el numero de la patota a Mi-Ram y nos devuelve un char* tarea_tripulante
	//Si no hay una proxima tarea, devuelve un NULL

	return obtener_la_tarea(tarea_tripulante);
}

posiciones* obtener_posiciones(uint32_t tripulante){
	//le mandamos el id del tripulante a Mi_Ram y nos dice su ubicacion

	//return posicion;
}

uint32_t obtener_distancia(posiciones* posicion_tripu, posiciones* posicion_tarea){
	return (abs(posicion_tripu->posicion_x - posicion_tarea->posicion_x) + abs(posicion_tripu->posicion_y - posicion_tarea->posicion_y) );
}

t_tarea* obtener_la_tarea(char* tarea_tripulante) {
	char** parser_tarea = string_split(tarea_tripulante, " ");

	t_tarea* tarea_nueva = malloc(sizeof(t_tarea));

	tarea_nueva->operacion = mapeo_tareas_tripulantes(parser_tarea[0]);
	tarea_nueva->parametros = recibir_parametros(parser_tarea[1]);

	free(parser_tarea);
	return tarea_nueva;
}


t_parametros_tarea* recibir_parametros(char* parametros) {

	char** parser_parametros = NULL;

	t_parametros_tarea* estructura = malloc(sizeof(t_parametros_tarea));
	parser_parametros = string_split(parametros, ";");

	estructura->cantidad = atoi(parser_parametros[0]);
	estructura->posicion_x = atoi(parser_parametros[1]);
	estructura->posicion_y = atoi(parser_parametros[2]);
	estructura->tiempo = atoi(parser_parametros[3]);

	free(parser_parametros);
	return estructura;
}


void realizar_tarea(t_tarea* tarea, tripulante_plani tripulante){

	switch(tarea->operacion) {

		case GENERAR_OXIGENO:
			generar_insumo("Oxigeno.ims", 'O', tarea->parametros);
			break;

		case CONSUMIR_OXIGENO:
			consumir_insumo("Oxigeno.ims", 'O', tarea->parametros);
			break;

		case GENERAR_COMIDA:
			generar_insumo("Comida.ims", 'C', tarea->parametros);
			break;

		case CONSUMIR_COMIDA:
			consumir_insumo("Comida.ims",'C', tarea->parametros);
			break;

		case GENERAR_BASURA:
			generar_insumo("Basura.ims", 'B', tarea->parametros);
			break;

		case DESCARTAR_BASURA:
			descartar_basura();
			break;

		default:
			break;
		}
	//Es importante que sem_tripu quede en cero sino se autoejecuta.
}

void generar_insumo(char* nombre_archivo, char caracter_llenado, t_parametros_tarea* parametros,tripulante_plani tripulante) {

	sem_wait(sem_tripu);
	//llamar al i-mongo y gastar 1 ciclo de cpu
	sleep(RETARDO_CICLO_CPU);
	//running_block(tripulante->id_tripulante);
	sem_post(sem_a_block);
	sem_wait(ya_pase_a_block);

	if(SI ESTA EL ARCHIVO) {
		modificar_archivo(nombre_archivo, parametros->cantidad);
	}
	else {
		crear_archivo(nombre_archivo, caracter_llenado);
	}

	uint32_t tiempo_restante = parametros->tiempo;
	uint32_t tiempo_transcurrido = 0;

	while(tiempo_restante != 0){
		sem_wait(sem_tripu);
		sleep(RETARDO_CICLO_CPU);
		tiempo_transcurrido++;
		tiempo_restante--;
		sem_post(sem_tripu);
	}
}

void consumir_insumo(char* nombre_archivo, char caracter_a_consumir, t_parametros_tarea* parametros,tripulante_plani tripulante) {

	sem_wait(sem_tripu);
	//llamar al i-mongo y gastar 1 ciclo de cpu
	sleep(RETARDO_CICLO_CPU);
	//running_block(tripulante->id_tripulante);
	sem_post(sem_a_block);
	sem_wait(ya_pase_a_block);

	if(SI ESTA EL ARCHIVO) {
		modificar_archivo(nombre_archivo, parametros->cantidad);
	}
	else {
		crear_archivo(nombre_archivo, caracter_a_consumir);
	}

	uint32_t tiempo_restante = parametros->tiempo;
	uint32_t tiempo_transcurrido = 0;

	while(tiempo_restante != 0){
		sem_wait(sem_tripu);
		sleep(RETARDO_CICLO_CPU);
		tiempo_transcurrido++;
		tiempo_restante--;
		sem_post(sem_tripu);
	}
}

void descartar_basura(t_parametros_tarea* parametros,tripulante_plani tripulante) {

	sem_wait(sem_tripu);
	//llamar al i-mongo y gastar 1 ciclo de cpu
	sleep(RETARDO_CICLO_CPU);
	//running_block(tripulante->id_tripulante);
	sem_post(sem_a_block);
	sem_wait(ya_pase_a_block);

	if(SI ESTA EL ARCHIVO) {
		eliminar_archivo("Basura.ims");
	}
	else {
		log_info(logger, "El archivo 'Basura.ims' no existe. \n");
	}

	uint32_t tiempo_restante = parametros->tiempo;
	uint32_t tiempo_transcurrido = 0;

	while(tiempo_restante != 0){
		sem_wait(sem_tripu);
		sleep(RETARDO_CICLO_CPU);
		tiempo_transcurrido++;
		tiempo_restante--;
		sem_post(sem_tripu);
	}
}

#include "planificador.h"
#include <sys/types.h>

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
	//io
	contador_tripulantes_espera_io = malloc(sizeof(sem_t));
	sem_init(contador_tripulantes_espera_io,0, 0);

	planificacion_on_io= malloc(sizeof(sem_t));
	sem_init(planificacion_on_io, 0, 0);

	mutex_new = malloc(sizeof(sem_t));
	sem_init(mutex_new, 0 , 1);

	mutex_ready = malloc(sizeof(sem_t));
	sem_init(mutex_ready, 0, 1);

	planificacion_on = malloc(sizeof(sem_t));
	sem_init(planificacion_on, 0, 0);


	planificacion_on_ready_running = malloc(sizeof(sem_t));
	sem_init(planificacion_on_ready_running, 0, 0);

	contador_tripulantes_en_ready = malloc(sizeof(sem_t));
	sem_init(contador_tripulantes_en_ready,0 ,0);

	mutex_exit = malloc(sizeof(sem_t));
	sem_init(mutex_exit, 0, 1);

	multitarea_disponible = malloc(sizeof(sem_t));
	sem_init(multitarea_disponible, 0, GRADO_MULTITAREA);

	bloqueado_disponible=malloc(sizeof(sem_t));
	sem_init(bloqueado_disponible, 0, 1); //UNO SOLO A LA VEZ

	planificion_rafaga = malloc(sizeof(sem_t));
	sem_init(planificion_rafaga, 0, 0);

	mutex_ready_running = malloc(sizeof(sem_t));
	sem_init(mutex_ready_running, 0, 1);

	mutex_new_ready = malloc(sizeof(sem_t));
	sem_init(mutex_new_ready, 0, 1);

	mutex_io= malloc(sizeof(sem_t));
	sem_init(mutex_io, 0, 1);

	mutex_rafaga = malloc(sizeof(sem_t));
	sem_init(mutex_rafaga, 0, 1);
	//TODO mutex lista io
	mutex_cola_io=malloc(sizeof(sem_t));
	sem_init(mutex_cola_io, 0, 1);
}


void finalizar_semaforos_plani() {
	free(contador_tripulantes_en_new);
	free(contador_tripulantes_en_ready);

	free(mutex_new);
	free(mutex_ready);
	free(mutex_exit);
	free(mutex_ready_running);
	free(mutex_new_ready);
	free(mutex_rafaga);
	free(mutex_cola_io);
	free(planificacion_on);
	free(planificacion_on_ready_running);
	free(planificion_rafaga);

	free(multitarea_disponible);
}


void obtener_planificacion_de_config(t_config* config){

	GRADO_MULTITAREA = config_get_int_value(config, "GRADO_MULTITAREA");
	ALGORITMO = config_get_string_value(config, "ALGORITMO");
	QUANTUM = config_get_int_value(config, "QUANTUM");
	RETARDO_CICLO_CPU = config_get_int_value(config, "RETARDO_CICLO_CPU");
	DURACION_SABOTAJE = config_get_int_value(config, "DURACION_SABOTAJE");
}


void elegir_algoritmo() {

	algoritmo_elegido = mapeo_algoritmo_planificacion(ALGORITMO);

	switch(algoritmo_elegido){

		case FIFO:
			printf("Eligio el algoritmo FIFO.\n");
			break;

		case RR:
			printf("Eligio el algoritmo Round Robin con un Quantum de %u. \n", QUANTUM);
			break;

		default:
			printf("No se eligio ningún algoritmo.\n");
			break;
	}
}

void iniciar_planificacion() {

	cola_new = queue_create();



	cola_io = queue_create();

	cola_ready = queue_create();

	cola_exit = queue_create();

	cola_auxiliar_sabotaje = queue_create();

	tripulantes_exec_block = list_create();

	bloqueado_suspendido=list_create();

	bloqueado_suspendido_ready=list_create();

	inicializar_semaforos_plani();

	new_ready_off = 0;
	esperando_bloqueado = 0;

	ready_running_off = 0;

	dar_pulsos_off = 0;
	// esto tiene que ir en otra parte
	//finalizar_semaforos_plani();
}


// FUNCIONES PARA PEDIR DATOS A MI RAM
void actualizar_estado(tripulante_plani* tripu, char estado) {

    uint32_t conexion_mi_ram;

    t_tripulante_estado* tripulante_estado = malloc(sizeof(t_tripulante_estado));
    t_respuesta_tripulante* respuesta_estado = malloc(sizeof(t_respuesta_tripulante));

    tripu->estado = estado;

    tripulante_estado->id_tripulante = tripu->id_tripulante;
    tripulante_estado->id_patota = tripu->numero_patota;
    sem_wait(tripu->mutex_estado);
    tripulante_estado->estado = tripu->estado;
    sem_post(tripu->mutex_estado);

	conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);

	if(resultado_conexion(conexion_mi_ram, logger, "Mi-RAM HQ") == -1){
		log_error(logger, "No se pudo lograr la conexion con Mi-RAM.\n");
		abort();
	}

	enviar_mensaje(tripulante_estado, ACTUALIZAR_ESTADO_TRIPULANTE, conexion_mi_ram);

	if(validacion_envio(conexion_mi_ram) == 1) {
		recibir_mensaje(respuesta_estado, RESPUESTA_OK_ESTADO, conexion_mi_ram);

		if(respuesta_estado->respuesta != 1) {
			log_error(logger, "La respuesta fue negativa.");
			abort();
		}
		if(respuesta_estado->id_tripulante != tripu->id_tripulante) {
			log_error(logger, "¡No es el tripulante que estoy buscando!");
			abort();
		}
	}
	else {
		log_error(logger, "No se pudo enviar el mensaje a Mi-RAM. \n");
		abort();
	}

	close(conexion_mi_ram);

	free(tripulante_estado);
	free(respuesta_estado);
}


t_tarea* obtener_siguiente_tarea(uint32_t id_tripulante, uint32_t numero_patota){
/*
	t_tarea* tarea = malloc(sizeof(t_tarea));

	tarea->operacion = GENERAR_OXIGENO;
	tarea->cantidad = 5;
	tarea->posicion_x = 4;
	tarea->posicion_y = 4;
	tarea->tiempo = 5;
	return tarea;
*/

	uint32_t conexion_mi_ram;

	t_tripulante* tripulante_consulta = malloc(sizeof(t_tripulante));
	t_respuesta_tarea_tripulante* respuesta_tarea = malloc(sizeof(t_respuesta_tarea_tripulante));
	respuesta_tarea->tarea = malloc(sizeof(t_tarea));

	tripulante_consulta->id_patota = numero_patota;
	tripulante_consulta->id_tripulante = id_tripulante;

	conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);

	if(resultado_conexion(conexion_mi_ram, logger, "Mi-RAM HQ") == -1){
		log_error(logger, "No se pudo lograr la conexion con Mi-RAM.\n");
		abort();
	}

	enviar_mensaje(tripulante_consulta, PEDIDO_TAREA, conexion_mi_ram);

	if(validacion_envio(conexion_mi_ram) == 1) {
		recibir_mensaje(respuesta_tarea, RESPUESTA_NUEVA_TAREA, conexion_mi_ram);

		if(respuesta_tarea->respuesta != 1) {
			log_error(logger, "La respuesta fue negativa.");
			abort();
		}
		if(respuesta_tarea->id_tripulante != id_tripulante) {
			log_error(logger, "¡No es el tripulante que estoy buscando!");
			abort();
		}
		if(respuesta_tarea->tarea == NULL) {
			log_warning(logger, "No hay mas tareas para realizar.");
			return NULL;
		}
	}
	else {
		log_error(logger, "No se pudo enviar el mensaje a Mi-RAM. \n");
		abort();
	}

	t_tarea* tarea_buscada =malloc(sizeof(t_tarea));
	tarea_buscada = respuesta_tarea->tarea;

	close(conexion_mi_ram);

	free(tripulante_consulta);
	respuesta_tarea->tarea=NULL;
	free(respuesta_tarea->tarea);
	free(respuesta_tarea);

	if(tarea_buscada->operacion == TAREA_VACIA) {
		free(tarea_buscada);
		return NULL;
	}
	else {
		return tarea_buscada;
	}


}

void obtener_posiciones(posiciones* posiciones_buscadas,uint32_t id_tripulante, uint32_t numero_patota){

	uint32_t conexion_mi_ram;

	t_tripulante* posiciones_tripulante = malloc(sizeof(t_tripulante));
	t_respuesta_tripulante_ubicacion* respuesta_posiciones_tripu = malloc(sizeof(t_respuesta_tripulante_ubicacion));

	posiciones_tripulante->id_patota = numero_patota;
	posiciones_tripulante->id_tripulante = id_tripulante;

	//posiciones* posiciones_buscadas = malloc(sizeof(posiciones));

	conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);

	if(resultado_conexion(conexion_mi_ram, logger, "Mi-RAM HQ") == -1){
		log_error(logger, "No se pudo lograr la conexion con Mi-RAM.\n");
		abort();
	}

	enviar_mensaje(posiciones_tripulante, PEDIR_UBICACION_TRIPULANTE, conexion_mi_ram);

	if(validacion_envio(conexion_mi_ram) == 1) {
		recibir_mensaje(respuesta_posiciones_tripu, RESPUESTA_NUEVA_UBICACION, conexion_mi_ram);

		if(respuesta_posiciones_tripu->respuesta != 1) {
			log_error(logger, "La respuesta fue negativa.");
			abort();
		}
		if(respuesta_posiciones_tripu->id_tripulante != id_tripulante) {
			log_error(logger, "¡No es el tripulante que estoy buscando!");
			abort();
		}
	}
	else {
		log_error(logger, "No se pudo enviar el mensaje a Mi-RAM. \n");
		abort();
	}

	posiciones_buscadas->posicion_x = respuesta_posiciones_tripu->posicion_x;
	posiciones_buscadas->posicion_y = respuesta_posiciones_tripu->posicion_y;

	close(conexion_mi_ram);

	free(posiciones_tripulante);
	free(respuesta_posiciones_tripu);

	//return posiciones_buscadas;
}


void actualizar_posiciones_en_memoria(posiciones* posiciones_tripu, tripulante_plani* tripu) {

	t_tripulante_ubicacion* ubicaciones_a_enviar = malloc(sizeof(t_tripulante_ubicacion));
	//t_respuesta_tripulante* respuesta_ok_ubicacion = malloc(sizeof(t_respuesta_tripulante));

	ubicaciones_a_enviar->id_patota = tripu->numero_patota;
	ubicaciones_a_enviar->id_tripulante = tripu->id_tripulante;
	ubicaciones_a_enviar->posicion_x = posiciones_tripu->posicion_x;
	ubicaciones_a_enviar->posicion_y = posiciones_tripu->posicion_y;

	conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);

	if(resultado_conexion(conexion_mi_ram, logger, "Mi-RAM HQ") == -1){
		log_error(logger, "No se pudo lograr la conexion con Mi-RAM.\n");
		abort();
	}

	enviar_mensaje(ubicaciones_a_enviar, ACTUALIZAR_UBICACION_TRIPULANTE, conexion_mi_ram);
	/*
	if(validacion_envio(conexion_mi_ram) == 1) {
		recibir_mensaje(respuesta_ok_ubicacion, RESPUESTA_OK_UBICACION, conexion_mi_ram);

		if(respuesta_ok_ubicacion->respuesta != 1) {
			log_error(logger, "La respuesta fue negativa.");
			abort();
		}
		if(respuesta_ok_ubicacion->id_tripulante != tripu->id_tripulante) {
			log_error(logger, "¡No es el tripulante que estoy buscando!");
			abort();
		}
	}
	else {
		log_error(logger, "No se pudo enviar el mensaje a Mi-RAM. \n");
		abort();
	}
	*/
	free(ubicaciones_a_enviar);

}

uint32_t obtener_distancia(posiciones* posicion_tripu, posiciones* posicion_tarea){

	return (abs(posicion_tripu->posicion_x - posicion_tarea->posicion_x) + abs(posicion_tripu->posicion_y - posicion_tarea->posicion_y) );
}


void esperandoIo_bloqueado(){
	while(1){

		sem_wait(contador_tripulantes_espera_io);
		sem_wait(planificacion_on_io);
		sem_wait(bloqueado_disponible);

		tripulante_plani* tripulante_a_block = malloc(sizeof(tripulante_plani));

		sem_wait(mutex_cola_io);
		tripulante_a_block= queue_pop(cola_io);
		sem_post(mutex_cola_io);

		tripulante_a_block->puedo_ejecutar_io=1;

		sem_wait(mutex_io);
		if(new_ready_off){
			sem_post(mutex_io);
			sem_wait(planificacion_on_io);

		}else{
			sem_post(mutex_io);
		}

		sem_post(planificacion_on_io);
		tripulante_a_block=NULL;
		free(tripulante_a_block);
	}
}

void new_ready() {

	while(1){
		sem_wait(contador_tripulantes_en_new);

		sem_wait(planificacion_on);

		tripulante_plani* tripulante_a_ready = malloc(sizeof(tripulante_plani));

		sem_wait(mutex_new);
		tripulante_a_ready = queue_pop(cola_new);
		sem_post(mutex_new);

		sem_wait(mutex_ready);
		queue_push(cola_ready, tripulante_a_ready);
		sem_post(mutex_ready);

		sem_post(tripulante_a_ready->sem_planificacion);

		actualizar_estado(tripulante_a_ready, 'R');

		sem_post(contador_tripulantes_en_ready);

		sem_wait(mutex_new_ready);
		if(new_ready_off){
			sem_post(mutex_new_ready);
			sem_wait(planificacion_on);

		}else{
			sem_post(mutex_new_ready);
		}

		sem_post(planificacion_on);


		tripulante_a_ready=NULL;
		free(tripulante_a_ready);


	}
}



void ready_running() {
    while(1){

        sem_wait(planificacion_on_ready_running);

        sem_wait(contador_tripulantes_en_ready);

        sem_wait(multitarea_disponible);

        tripulante_plani* tripulante_a_running = malloc(sizeof(tripulante_plani));

        sem_wait(mutex_ready);
        tripulante_a_running = queue_pop(cola_ready);
        sem_post(mutex_ready);

        actualizar_estado(tripulante_a_running, 'E');

        //if(tripulante_a_running->estado_anterior=='E'){
        //	sem_post(tripulante_a_running->sem_tripu);

        //}else{
            sem_post(tripulante_a_running->sem_planificacion);
        //}



		sem_wait(mutex_ready_running);

		if(ready_running_off){
			sem_post(mutex_ready_running);
			sem_wait(planificacion_on_ready_running);

		}else{
			sem_post(mutex_ready_running);
		}

		sem_post(planificacion_on_ready_running);
        tripulante_a_running = NULL;

        free(tripulante_a_running);
    }

}


void running_ready(tripulante_plani* tripu){

	sem_wait(mutex_ready);
	queue_push(cola_ready, tripu);
	sem_post(mutex_ready);

	actualizar_estado(tripu, 'R');
	sem_post(contador_tripulantes_en_ready);

	sem_post(multitarea_disponible);

}




void running_block(tripulante_plani* tripu){

	actualizar_estado(tripu, 'B');
	sem_wait(mutex_cola_io);
	queue_push(cola_io,tripu);
	sem_post(mutex_cola_io);
	sem_post(contador_tripulantes_espera_io);

	sem_post(multitarea_disponible);
}


void block_ready(tripulante_plani* tripu){
	sem_wait(mutex_ready);
	queue_push(cola_ready, tripu);
	sem_post(mutex_ready);

	tripu->puedo_ejecutar_io=0;
	actualizar_estado(tripu, 'R');

	sem_post(contador_tripulantes_en_ready);
}


void block_exit(tripulante_plani* tripu){

	sem_wait(mutex_exit);
	queue_push(cola_exit, tripu);
	sem_post(mutex_exit);

	actualizar_estado(tripu, 'T');
}

// Creo que este ya no va
void new_exit(tripulante_plani* tripu){
	sem_wait(mutex_exit);
	queue_push(cola_exit, tripu);
	sem_post(mutex_exit);
	sem_wait(contador_tripulantes_en_new);

	actualizar_estado(tripu, 'T');
}


void running_exit(tripulante_plani* tripu){

	sem_wait(mutex_exit);
	queue_push(cola_exit, tripu);
	sem_post(mutex_exit);

	actualizar_estado(tripu, 'T');

	sem_post(multitarea_disponible);
}


void suspendido_ready(tripulante_plani* tripu){

	sem_wait(mutex_ready);
	queue_push(cola_ready, tripu);
	sem_post(mutex_ready);

	actualizar_estado(tripu, 'R');

	sem_post(contador_tripulantes_en_ready);
}


void ready_exit(tripulante_plani* tripu){
	int largo;

	tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));

	sem_wait(mutex_ready);

	largo = queue_size(cola_ready);
	for(int i=0;i<largo;i++){

		tripulante = queue_pop(cola_ready);

		if(tripu->id_tripulante == tripulante->id_tripulante){

			sem_wait(mutex_exit);
			queue_push(cola_exit, tripulante);
			sem_post(mutex_exit);
			actualizar_estado(tripu, 'T');

		} else {
			queue_push(cola_auxiliar_sabotaje, tripulante);
		}

	}


	int aux_largo = queue_size(cola_auxiliar_sabotaje);

	for(int i=0;i<aux_largo;i++){

		tripulante = queue_pop(cola_auxiliar_sabotaje);

		queue_push(cola_ready, tripulante);
	}

	//recorro la lista de tripulantes igual q donde es listar

	sem_wait(contador_tripulantes_en_ready);
	sem_post(mutex_ready);

	tripulante=NULL;
	free(tripulante);
}


void running_suspendido(tripulante_plani* tripu){

	actualizar_estado(tripu, 'S');
	sem_post(multitarea_disponible);

}


void ready_suspendido(tripulante_plani* tripu){

	actualizar_estado(tripu, 'S');
	//int a;
	//sem_getvalue(contador_tripulantes_en_ready,&a);
	//printf("valor semaforo %u",a);
	//fflush(stdout);
	sem_wait(contador_tripulantes_en_ready);
}




void tripulante_hilo(void* tripulante){
	tripulante_plani* tripu = tripulante;

	sem_wait(tripu->sem_planificacion);

	sem_wait(tripu->mutex_expulsado);
	if(tripu->expulsado){
		sem_post(tripu->mutex_expulsado);
		return;
	}else{
		sem_post(tripu->mutex_expulsado);
	}

	cambios_de_tarea(tripu);

	posiciones* posicion_tripu = malloc(sizeof(posiciones));

	obtener_posiciones(posicion_tripu,tripu->id_tripulante, tripu->numero_patota);

	posiciones* posicion_tarea;
	posicion_tarea = malloc(sizeof(posiciones));

	while(tripu->tarea_a_realizar != NULL){



		//Entra a exec



		posicion_tarea->posicion_x = tripu->tarea_a_realizar->posicion_x;
		posicion_tarea->posicion_y = tripu->tarea_a_realizar->posicion_y;

		uint32_t distancia = obtener_distancia(posicion_tripu,posicion_tarea);
		tripu->cantidad_realizada = 0;

		sem_wait(tripu->mutex_expulsado);
		if(tripu->expulsado){
			sem_post(tripu->mutex_expulsado);
			return;

		}else{
			sem_post(tripu->mutex_expulsado);
		}
		sem_wait(tripu->sem_planificacion); //Queda trabado hasta que el hilo de ready_running le hace el post (TRABADO EN READY)

		while(distancia > 0 && !tripu->elegido_sabotaje){ //Cambiar condicion con variable goblal hay_sabotaje
			sem_wait(tripu->sem_tripu);

			obtener_nueva_posicion(posicion_tripu, posicion_tarea, tripu);  //Hay que actualizar la ubicacion en Mi_Ram
			tripu->cantidad_realizada= tripu->cantidad_realizada+1;
			distancia--;

			sem_wait(tripu->mutex_expulsado);
			if(tripu->expulsado){
				//sem_post(tripu->mutex_expulsado);
				return;
			}else{
				sem_post(tripu->mutex_expulsado);
			}

			if(!(tripu->elegido_sabotaje) && !(tripu->fui_elegido_antes)){
				if(algoritmo_elegido == RR){
					if(tripu->cantidad_realizada == QUANTUM){
						running_ready(tripu);
						sem_wait(tripu->sem_planificacion);
					}
				}
			}

			         //Trabado por el pulso (rafaga), te lo dan siempre que estes en Exec
			fflush(stdout);

		}

		sem_wait(tripu->mutex_expulsado);
		if(tripu->expulsado){
			sem_post(tripu->mutex_expulsado);
			return;
		}else{
			sem_post(tripu->mutex_expulsado);
		}


		if(!(tripu->elegido_sabotaje) && !(tripu->fui_elegido_antes)){
			if(algoritmo_elegido == RR){
				if(tripu->cantidad_realizada == QUANTUM){
					running_ready(tripu);
					tripu->cantidad_realizada = 0;
					sem_wait(tripu->sem_planificacion);
				}
			}
		}

		realizar_tarea(tripu);
	}
}

void rafaga_cpu(t_list* lista_todos_tripulantes){
	while(1){

	sem_wait(planificion_rafaga);

	bool esta_exec_o_block(void* tripulante){

		//TODO posible mutex
		sem_wait(((tripulante_plani*)tripulante)->mutex_estado);
		int valor = ((tripulante_plani*)tripulante)->estado == 'E' ||( ((tripulante_plani*)tripulante)->estado == 'B' && ((tripulante_plani*)tripulante)->puedo_ejecutar_io == 1);
		sem_post(((tripulante_plani*)tripulante)->mutex_estado);

		return valor;
	}

	tripulantes_exec_block = list_filter(lista_todos_tripulantes,(void*) esta_exec_o_block);

	//int largo=list_size(tripulantes_exec_block);

	list_iterate(tripulantes_exec_block, (void*) poner_en_uno_semaforo);

	sleep(RETARDO_CICLO_CPU);


	sem_wait(mutex_rafaga);

	if(dar_pulsos_off){
		sem_post(mutex_rafaga);
		sem_wait(planificion_rafaga);

	}else{
		sem_post(mutex_rafaga);
	}

	sem_post(planificion_rafaga);
	}
}

void poner_en_uno_semaforo(tripulante_plani* tripulante){
	sem_post(tripulante->sem_tripu);
}

void obtener_nueva_posicion(posiciones* posicion_tripu, posiciones* posicion_tarea, tripulante_plani* tripu){

	while(posicion_tripu->posicion_x != posicion_tarea->posicion_x){
		if(posicion_tripu->posicion_x > posicion_tarea->posicion_x){
			//posicion_tripu->posicion_x = posicion_tripu->posicion_x - 1 ;
			posicion_tripu->posicion_x--;
			actualizar_posiciones_en_memoria(posicion_tripu, tripu);
			return;
		}

		if(posicion_tripu->posicion_x < posicion_tarea->posicion_x){
			//posicion_tripu->posicion_x = posicion_tripu->posicion_x + 1 ;
			posicion_tripu->posicion_x++;
			// actualizar en mi ram
			actualizar_posiciones_en_memoria(posicion_tripu, tripu);
			return;
		}
	}

	while(posicion_tripu->posicion_y != posicion_tarea->posicion_y){
			if(posicion_tripu->posicion_y > posicion_tarea->posicion_y){
				//posicion_tripu->posicion_y = posicion_tripu->posicion_y - 1 ;
				posicion_tripu->posicion_y--;
				// actualizar en mi ram
				actualizar_posiciones_en_memoria(posicion_tripu, tripu);
				return;
			}

			if(posicion_tripu->posicion_y < posicion_tarea->posicion_y){
				//posicion_tripu->posicion_y = posicion_tripu->posicion_y + 1 ;
				posicion_tripu->posicion_y++;
				// actualizar en mi ram
				actualizar_posiciones_en_memoria(posicion_tripu, tripu);
				return;
			}
	}
	return;
}


void realizar_tarea(tripulante_plani* tripu){

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

		case REALIZAR_TAREA_SABOTAJE:
			realizar_tarea_sabotaje(tripu);
			break;

		default:
			otras_tareas(tripu);
			break;
		}

}


void generar_insumo(char* nombre_archivo, char caracter_llenado, tripulante_plani* tripu) {


	sem_wait(tripu->sem_tripu);


	sem_wait(tripu->mutex_expulsado);
	if(tripu->expulsado){
		sem_post(tripu->mutex_expulsado);
		return;
	}else{
		sem_post(tripu->mutex_expulsado);
	}


	if(!(tripu->elegido_sabotaje)&&!(tripu->fui_elegido_antes)){ //NI Q SEA CUANDO LO ESTAS HACIENDO

		//cambio de estado
		//running_esperaIo
		running_block(tripu);
		//running_espera_io(tripu);


		//sem_wait(tripu->sem_planificacion);

		//CAMBIAR ESTADO A UNO NUEVO
		//AGREGARTE A LA COLA IO
		//TRABARTE CON SEM PLANI ??

		enviar_tarea_io(tripu, GENERAR_INSUMO, nombre_archivo, caracter_llenado);




	}



	uint32_t tiempo_restante = tripu->tarea_a_realizar->tiempo;


	while(tiempo_restante != 0 && !(tripu->elegido_sabotaje)){
		sem_wait(tripu->sem_tripu);

		tiempo_restante--;

		//aca hay q pregunta rsi esta encargado de sabotjae, si es asi breck

		sem_wait(tripu->mutex_expulsado);
		if(tripu->expulsado){
			sem_post(tripu->mutex_expulsado);
			return;
		}else{
			sem_post(tripu->mutex_expulsado);
		}

	}
	sem_post(bloqueado_disponible); //termino de hacer su io
	//incrementas lugar libre de bloqueo
	cambios_de_tarea(tripu);
	//tripu->tarea_a_realizar=NULL;

	if(!(tripu->fui_elegido_antes)){
		if(tripu->tarea_a_realizar!=NULL){
			block_ready(tripu);
		}else{
			block_exit(tripu);
			terminar_tripulante(tripu);
		}
	}

		//Es importante que sem_tripu quede en cero sino se autoejecuta.
}

void consumir_insumo(char* nombre_archivo, char caracter_a_consumir, tripulante_plani* tripu) {

	sem_wait(tripu->sem_tripu);
	//llamar al i-mongo y gastar 1 ciclo de cpu

	sem_wait(tripu->mutex_expulsado);
	if(tripu->expulsado){
		sem_post(tripu->mutex_expulsado);
		return;
	}else{
		sem_post(tripu->mutex_expulsado);
	}

	if(!(tripu->elegido_sabotaje)&&!(tripu->fui_elegido_antes)){

		running_block(tripu);

		//running_block(tripu);este no se usaria nunca
		enviar_tarea_io(tripu, CONSUMIR_INSUMO, nombre_archivo, caracter_a_consumir);

	}



	uint32_t tiempo_restante = tripu->tarea_a_realizar->tiempo;


	while(tiempo_restante != 0 && !(tripu->elegido_sabotaje)){
		sem_wait(tripu->sem_tripu);

		tiempo_restante--;

		sem_wait(tripu->mutex_expulsado);
		if(tripu->expulsado){
			sem_post(tripu->mutex_expulsado);
			return;
		}else{
			sem_post(tripu->mutex_expulsado);
		}
	}
	sem_post(bloqueado_disponible); //termino de hacer su io

	cambios_de_tarea(tripu);
	//tripu->tarea_a_realizar= NULL;

	if(!(tripu->elegido_sabotaje)){
		if(tripu->tarea_a_realizar!=NULL){
			block_ready(tripu);
		}else{
			block_exit(tripu);
			terminar_tripulante(tripu);
		}
	}
}

void descartar_basura(tripulante_plani* tripu) {

	sem_wait(tripu->sem_tripu);
	//llamar al i-mongo y gastar 1 ciclo de cpu

	sem_wait(tripu->mutex_expulsado);
	if(tripu->expulsado){
		sem_post(tripu->mutex_expulsado);
		return;
	}else{
		sem_post(tripu->mutex_expulsado);
	}
	if(!(tripu->elegido_sabotaje)&&!(tripu->fui_elegido_antes)){
		//running_espera_io(tripu);

		running_block(tripu);
		enviar_tarea_io(tripu, TIRAR_BASURA, "", ' ');
	}


	uint32_t tiempo_restante = tripu->tarea_a_realizar->tiempo;

	while(tiempo_restante != 0 && !(tripu->elegido_sabotaje)){
		sem_wait(tripu->sem_tripu);

		tiempo_restante--;

		sem_wait(tripu->mutex_expulsado);
		if(tripu->expulsado){
			sem_post(tripu->mutex_expulsado);
			return;
		}else{
			sem_post(tripu->mutex_expulsado);
		}
	}
	sem_post(bloqueado_disponible); //termino de hacer su io
	cambios_de_tarea(tripu);
	//tripu->tarea_a_realizar= NULL;

	if(!(tripu->elegido_sabotaje)){
		if(tripu->tarea_a_realizar != NULL){
			block_ready(tripu);
		}else{
			block_exit(tripu);
			terminar_tripulante(tripu);
		}
	}
}

void otras_tareas(tripulante_plani* tripu){

	uint32_t tiempo_restante = tripu->tarea_a_realizar->tiempo;

	while(tiempo_restante > 0 && !(tripu->elegido_sabotaje)){
		if(tripu->cantidad_realizada == QUANTUM){
			running_ready(tripu);

			sem_wait(tripu->sem_planificacion);

			sem_wait(tripu->mutex_expulsado);
			if(tripu->expulsado){
				sem_post(tripu->mutex_expulsado);
				return;
			}else{
				sem_post(tripu->mutex_expulsado);
			}
		}

		sem_wait(tripu->sem_tripu);

		tiempo_restante--;

		sem_wait(tripu->mutex_expulsado);
		if(tripu->expulsado){
			sem_post(tripu->mutex_expulsado);
			return;
		}else{
			sem_post(tripu->mutex_expulsado);
		}

	}
	cambios_de_tarea(tripu);

	//tripu->tarea_a_realizar= NULL;

	if(!(tripu->elegido_sabotaje)){
			if(tripu->tarea_a_realizar!=NULL){
				running_ready(tripu);
			}else{
				running_exit(tripu);
				terminar_tripulante(tripu);
			}
	}
}


void realizar_tarea_sabotaje(tripulante_plani* tripu){
	//Avisar a I-mongo que estoy en la posicion de la tarea sabotaje
	//Lo pasamos a la lista de bloqueados suspendidos
	tripu->estado_anterior='R';

	sleep(DURACION_SABOTAJE);

	list_add(bloqueado_suspendido,tripu);
	actualizar_estado(tripu, 'S');

	cambios_de_tarea(tripu);
}


void cambios_de_tarea(tripulante_plani* tripu) {

	if(tripu->fui_elegido_antes){ //para hacer el intercambio en la vuelta q esta arreglando el sabotaje
		tripu->fui_elegido_antes=0;
		tripu->tarea_a_realizar->operacion=tripu->tarea_auxiliar->operacion;
		tripu->tarea_a_realizar->cantidad=tripu->tarea_auxiliar->cantidad;
		tripu->tarea_a_realizar->posicion_x=tripu->tarea_auxiliar->posicion_x;
		tripu->tarea_a_realizar->posicion_y=tripu->tarea_auxiliar->posicion_y;
		sem_post(termine_sabotaje);
	}else{

		if(tripu->elegido_sabotaje){

			tripu->fui_elegido_antes=1;
			tripu->elegido_sabotaje=0;

			t_tarea* aux_intercambio=malloc(sizeof(t_tarea));

			aux_intercambio->operacion=tripu->tarea_a_realizar->operacion;
			aux_intercambio->cantidad=tripu->tarea_a_realizar->cantidad;
			aux_intercambio->posicion_x=tripu->tarea_a_realizar->posicion_x;
			aux_intercambio->posicion_y=tripu->tarea_a_realizar->posicion_y;

			tripu->tarea_a_realizar->operacion=tripu->tarea_auxiliar->operacion;
			tripu->tarea_a_realizar->cantidad=tripu->tarea_auxiliar->cantidad;
			tripu->tarea_a_realizar->posicion_x=tripu->tarea_auxiliar->posicion_x;
			tripu->tarea_a_realizar->posicion_y=tripu->tarea_auxiliar->posicion_y;

			tripu->tarea_auxiliar->operacion=aux_intercambio->operacion;
			tripu->tarea_auxiliar->cantidad=aux_intercambio->cantidad;
			tripu->tarea_auxiliar->posicion_x=aux_intercambio->posicion_x;
			tripu->tarea_auxiliar->posicion_y=aux_intercambio->posicion_y;

			sem_post(tripu->sem_planificacion);
			aux_intercambio=NULL;
			free(aux_intercambio);

		}else{

			//tripu->tarea_a_realizar= NULL;
			tripu->tarea_a_realizar= obtener_siguiente_tarea(tripu->id_tripulante, tripu->numero_patota);
		}
	}
}


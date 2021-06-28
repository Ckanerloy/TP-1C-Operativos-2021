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
	sem_init(mutex_valorMultitarea, 0 , 1);

	contador_tripulantes_en_ready = malloc(sizeof(sem_t));
	sem_init(contador_tripulantes_en_ready,0 ,0);

	mutex_exit = malloc(sizeof(sem_t));
	sem_init(mutex_exit, 0, 1);

	mutex_expulsado = malloc(sizeof(sem_t));
	sem_init(mutex_expulsado,0, 1);

	mutex_tripulante_estado = malloc(sizeof(sem_t));
	sem_init(mutex_tripulante_estado, 0, 1);
}


void finalizar_semaforos_plani() {
	free(contador_tripulantes_en_new);
	free(contador_tripulantes_en_ready);

	free(mutex_new);
	free(mutex_ready);
	free(mutex_exit);
	free(mutex_valorMultitarea);
	free(mutex_expulsado);

	free(planificacion_on);
	free(planificacion_on_ready_running);
}


void obtener_planificacion_de_config(t_config* config){

	GRADO_MULTITAREA = config_get_int_value(config, "GRADO_MULTITAREA");
	ALGORITMO = config_get_string_value(config, "ALGORITMO");
	QUANTUM = config_get_int_value(config, "QUANTUM");
	RETARDO_CICLO_CPU = config_get_int_value(config, "RETARDO_CICLO_CPU");
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

	cola_ready = queue_create();

	cola_exit = queue_create();

	cola_auxiliar_sabotaje = queue_create();

	multitarea_Disponible = GRADO_MULTITAREA;

	lista_semaforos_tripulantes = list_create();



	inicializar_semaforos_plani();

	// esto tiene que ir en otra parte
	//finalizar_semaforos_plani();
}
void actualizar_estado(tripulante_plani* tripu, char estado) {

    uint32_t conexion_mi_ram;

    t_tripulante_estado* tripulante_estado = malloc(sizeof(t_tripulante_estado));
    t_respuesta_tripulante* respuesta_estado = malloc(sizeof(t_respuesta_tripulante));

    tripu->estado = estado;

    tripulante_estado->id_tripulante = tripu->id_tripulante;
    tripulante_estado->id_patota = tripu->numero_patota;
    tripulante_estado->estado = tripu->estado;

    conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);

    if(resultado_conexion(conexion_mi_ram, logger, "Mi-RAM HQ") == -1){
        log_error(logger, "No se pudo lograr la conexion con Mi-RAM.\n");
        abort();
    }

    sem_wait(mutex_tripulante_estado);
    enviar_mensaje(tripulante_estado, ACTUALIZAR_ESTADO_TRIPULANTE, conexion_mi_ram);
    sem_post(mutex_tripulante_estado);

    if(validacion_envio(conexion_mi_ram) == 1) {

        sem_wait(mutex_tripulante_estado);
        recibir_mensaje(respuesta_estado, RESPUESTA_OK_ESTADO, conexion_mi_ram);
        sem_post(mutex_tripulante_estado);

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
		//tripulante_a_ready->estado = 'R';



		actualizar_estado(tripulante_a_ready, 'R');


		sem_post(planificacion_on);

		sem_post(contador_tripulantes_en_ready);

	}
}



void ready_running() {
    while(1){

        sem_wait(planificacion_on_ready_running);

        sem_wait(contador_tripulantes_en_ready);

        sem_wait(mutex_valorMultitarea);

        if(multitarea_Disponible > 0) {

        	sem_post(mutex_valorMultitarea);
            tripulante_plani* tripulante_a_running = malloc(sizeof(tripulante_plani));

            sem_wait(mutex_ready);
            tripulante_a_running = queue_pop(cola_ready);
            sem_post(mutex_ready);

            sem_wait(mutex_valorMultitarea);
            multitarea_Disponible--;
            sem_post(mutex_valorMultitarea);

            //tripulante_a_running->estado = 'E';

            actualizar_estado(tripulante_a_running, 'E');

            sem_post(tripulante_a_running->sem_planificacion);

        } else {
                sem_post(mutex_valorMultitarea);
                sem_post(contador_tripulantes_en_ready);
        }

        sem_post(planificacion_on_ready_running);
    }
}

void running_ready(tripulante_plani* tripu){

	sem_wait(mutex_valorMultitarea);
	multitarea_Disponible++;
	sem_post(mutex_valorMultitarea);

	sem_wait(mutex_ready);
	queue_push(cola_ready, tripu);
	sem_post(mutex_ready);

	//tripu->estado = 'R';
	actualizar_estado(tripu, 'R');
	sem_post(contador_tripulantes_en_ready);

	//Actualizar el estado del tripulante (R) EN Mi-Ram

}

void running_block(tripulante_plani* tripu){

	sem_wait(mutex_valorMultitarea);
	multitarea_Disponible++;
	sem_post(mutex_valorMultitarea);

	//Actualizar el estado del tripulante (B) EN Mi-Ram
	//tripu->estado = 'B';
	actualizar_estado(tripu, 'B');

}

void block_ready(tripulante_plani* tripu){
	sem_wait(mutex_ready);
	queue_push(cola_ready, tripu);
	sem_post(mutex_ready);


	//tripu->estado = 'R';
	//Actualizar el estado del tripulante (R) EN Mi-Ram
	actualizar_estado(tripu, 'R');
}

void block_exit(tripulante_plani* tripu){

	sem_wait(mutex_exit);
	queue_push(cola_exit, tripu);
	sem_post(mutex_exit);

	//tripu->estado = 'T';
	//Actualizar el estado del tripulante (T) EN Mi-Ram
	actualizar_estado(tripu, 'T');
}

void running_exit(tripulante_plani* tripu){

	sem_wait(mutex_exit);
	queue_push(cola_exit, tripu);
	sem_post(mutex_exit);

	sem_wait(mutex_valorMultitarea);
	multitarea_Disponible++;
	sem_post(mutex_valorMultitarea);

	//tripu->estado = 'T';
	//Actualizar el estado del tripulante (T) EN Mi-Ram
	actualizar_estado(tripu, 'T');
}

void ready_exit(tripulante_plani* tripu){
	int largo;

	tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));

	sem_wait(mutex_ready);

	largo = queue_size(cola_ready);
	for(int i=0;i<largo;i++){

		tripulante = queue_pop(cola_ready);
		//printf("id del tripulante %u \n",tripulante->id_tripulante);
		//printf("nro pato tripu %u \n",tripulante->numero_patota);
		//fflush(stdout);
		if(tripu->id_tripulante == tripulante->id_tripulante){

			sem_wait(mutex_exit);
			queue_push(cola_exit, tripulante);
			sem_post(mutex_exit);
			actualizar_estado(tripu, 'T');
			//tripulante->estado ='T';

		}else{
			queue_push(cola_auxiliar_sabotaje, tripulante);
		}

	}


	int aux_largo = queue_size(cola_auxiliar_sabotaje);

	for(int i=0;i<aux_largo;i++){

		tripulante = queue_pop(cola_auxiliar_sabotaje);
		//printf("id del tripulante %u \n",tripulante->id_tripulante);
		//printf("nro pato tripu %u \n",tripulante->numero_patota);
		fflush(stdout);

		queue_push(cola_ready, tripulante);

	}

	//recorro la lista de tripulantes igual q donde es listar



	sem_wait(contador_tripulantes_en_ready);
	sem_post(mutex_ready);



	//free(tripulante);
}

void tripulante_hilo(void* tripulante){
	tripulante_plani* tripu = tripulante;

	sem_wait(tripu->sem_planificacion);

	tripu->tarea_a_realizar = obtener_siguiente_tarea(tripu->numero_patota);

	posiciones* posicion_tripu;
	posicion_tripu = malloc(sizeof(posiciones));
	posicion_tripu = obtener_posiciones(tripu->id_tripulante,tripu->numero_patota);

	while(tripu->tarea_a_realizar != NULL){
		sem_wait(tripu->sem_planificacion); //Le hacemos el signal para que no quede trabado


		sem_wait(mutex_expulsado);
		if(tripu->expulsado){
			sem_post(mutex_expulsado);
			return;
		}else{
			sem_post(mutex_expulsado);
		}

		posiciones* posicion_tarea;
		posicion_tarea = malloc(sizeof(posiciones));
		posicion_tarea->posicion_x = tripu->tarea_a_realizar->posicion_x;
		posicion_tarea->posicion_y = tripu->tarea_a_realizar->posicion_y;

		uint32_t distancia = obtener_distancia(posicion_tripu,posicion_tarea);
		uint32_t cantidadRealizado = 0;

		while(distancia > 0){

			sem_wait(mutex_expulsado);
			if(tripu->expulsado){
				sem_post(mutex_expulsado);
				return;
			}else{
				sem_post(mutex_expulsado);
			}

			if(algoritmo_elegido==RR){

				if(cantidadRealizado==QUANTUM){
					running_ready(tripu);
					cantidadRealizado=0;
					sem_wait(tripu->sem_planificacion);
				}

			}
			//printf("antes del wait/n");
			//fflush(stdout);
			sem_wait(tripu->sem_tripu);
			//printf("despues del wait");
			sleep(RETARDO_CICLO_CPU); //vale 0
			//posicion_tripu = obtener_nueva_posicion(posicion_tripu,posicion_tarea);  Hay que actualizar la ubicacion en Mi_Ram
			cantidadRealizado ++;

			distancia--;

			sem_post(tripu->sem_tripu); //vale 1 0
		}


		sem_wait(mutex_expulsado);

		if(tripu->expulsado){
			sem_post(mutex_expulsado);
			return;
		}else{
			sem_post(mutex_expulsado);
		}

		if(algoritmo_elegido==RR){
			if(cantidadRealizado==QUANTUM){
				running_ready(tripu);

				cantidadRealizado=0;
				sem_wait(tripu->sem_planificacion);
			}
		}

		sem_wait(mutex_expulsado);
		if(tripu->expulsado){
			sem_post(mutex_expulsado);
			return;
		}else{
			sem_post(mutex_expulsado);
		}
		realizar_tarea(tripu,&cantidadRealizado);
	}
	//sem_wait(tripu->sem_tripu);
}
/*
//TAREA SABOTAJE
void hilo_tripulante_sabotaje(tripulante_sabotaje* tripu){

	posiciones* posicion_tarea;
	posicion_tarea = malloc(sizeof(posiciones));
	posicion_tarea->posicion_x = tripu->posicion_sabotaje->posicion_x;
	posicion_tarea->posicion_y = tripu->posicion_sabotaje->posicion_y;


	posiciones* posicion_tripu;
	posicion_tripu = malloc(sizeof(posiciones));
	posicion_tripu = obtener_posiciones(tripu->id_tripulante,tripu->id_patota);


	posiciones* posicion_tripu_inicial;
	posicion_tripu_inicial = malloc(sizeof(posiciones));
	posicion_tripu_inicial->posicion_x=posicion_tripu->posicion_x;
	posicion_tripu_inicial->posicion_y=posicion_tripu->posicion_y;


	uint32_t distancia = obtener_distancia(posicion_tripu,posicion_tarea);

	while(distancia > 0){
		//posicion_tripu = obtener_nueva_posicion(posicion_tripu,posicion_tarea);  Hay que actualizar la ubicacion en Mi_Ram
		sleep(RETARDO_CICLO_CPU);
		distancia--;
	}
	//avisar q estas en posicion de resolucion de sabotaje y mandar orden de ejecucion a imongo
	sleep(DURACION_SABOTAJE);


}
*/

t_tarea* obtener_siguiente_tarea(uint32_t numero_patota){

	t_tarea* tarea = malloc(sizeof(t_tarea));

	tarea->operacion = GENERAR_OXIGENO;
	tarea->cantidad = 5;
	tarea->posicion_x = 4;
	tarea->posicion_y = 4;
	tarea->tiempo = 5;
	return tarea;


	// enviar_mensaje(t_tripulante, PEDIDO_TAREA, conexion_mi_ram);

	// recibir_mensaje(t_respuesta_tarea_tripulante, RESPUESTA_NUEVA_TAREA, conexion_mi_ram);

	//le mandamos el numero de la patota a Mi-Ram y nos devuelve un char* tarea_tripulante
	//Si no hay una proxima tarea, devuelve un NULL

	//return obtener_la_tarea(tarea_tripulante);
}

posiciones* obtener_posiciones(uint32_t id_tripulante,uint32_t id_patota){
	//le mandamos el id del tripulante a Mi_Ram y nos dice su ubicacion
	posiciones* posicion = malloc(sizeof(posiciones));
	posicion->posicion_x = 1;
	posicion->posicion_y = 1;
	return posicion;
}

uint32_t obtener_distancia(posiciones* posicion_tripu, posiciones* posicion_tarea){
	return (abs(posicion_tripu->posicion_x - posicion_tarea->posicion_x) + abs(posicion_tripu->posicion_y - posicion_tarea->posicion_y) );
}



void realizar_tarea(tripulante_plani* tripu, uint32_t* cantidadRealizado){


	sem_wait(tripu->sem_tripu);
	sem_post(tripu->sem_tripu);
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
/*
	//sem_wait(mutex_sabotaje);
	int valor=valor_sabotaje;
	//sem_post(mutex_sabotaje);

	if(valor==1){
		sem_wait(tripu->sem_tripu);
		sem_wait(tripu->sem_tripu);
	}
*/

}

void generar_insumo(char* nombre_archivo, char caracter_llenado,tripulante_plani* tripu) {

	//Aca iria un if preguntando si esta expulsado
	sem_wait(tripu->sem_tripu);
	//llamar al i-mongo y gastar 1 ciclo de cpu
	sleep(RETARDO_CICLO_CPU);
	sem_post(tripu->sem_tripu);

	sem_wait(mutex_expulsado);
	if(tripu->expulsado){
		sem_post(mutex_expulsado);
		return;
	}else{
		sem_post(mutex_expulsado);
	}

	running_block(tripu);

	//if(SI ESTA EL ARCHIVO) {
	//	modificar_archivo(nombre_archivo, parametros->cantidad);
	//}
	//else {
	//	crear_archivo(nombre_archivo, caracter_llenado);
	//}

	uint32_t tiempo_restante = tripu->tarea_a_realizar->tiempo;


	while(tiempo_restante != 0){
		sem_wait(tripu->sem_tripu);
		//Aca iria un if preguntando si esta expulsado con BREAK
		sleep(RETARDO_CICLO_CPU);
		tiempo_restante--;
		sem_post(tripu->sem_tripu);

		sem_wait(mutex_expulsado);
		if(tripu->expulsado){
			sem_post(mutex_expulsado);
			return;
		}else{
			sem_post(mutex_expulsado);
		}
	}
	//tripu->tarea_a_realizar= obtener_siguiente_tarea(tripu->numero_patota);

	tripu->tarea_a_realizar= NULL;

	if(tripu->tarea_a_realizar!=NULL){
		block_ready(tripu);
	}else{
		block_exit(tripu);
	}

		//Es importante que sem_tripu quede en cero sino se autoejecuta.
}

void consumir_insumo(char* nombre_archivo, char caracter_a_consumir,tripulante_plani* tripu) {

	sem_wait(tripu->sem_tripu);
	//llamar al i-mongo y gastar 1 ciclo de cpu
	sleep(RETARDO_CICLO_CPU);
	sem_post(tripu->sem_tripu);

	sem_wait(mutex_expulsado);
	if(tripu->expulsado){
		sem_post(mutex_expulsado);
		return;
	}else{
		sem_post(mutex_expulsado);
	}

	running_block(tripu);

	//if(SI ESTA EL ARCHIVO) {
		//modificar_archivo(nombre_archivo, parametros->cantidad);
	//}
	//else {
	//	crear_archivo(nombre_archivo, caracter_a_consumir);
	//}

	uint32_t tiempo_restante = tripu->tarea_a_realizar->tiempo;


	while(tiempo_restante != 0){
		sem_wait(tripu->sem_tripu);
		sleep(RETARDO_CICLO_CPU);
		tiempo_restante--;
		sem_post(tripu->sem_tripu);

		sem_wait(mutex_expulsado);
		if(tripu->expulsado){
			sem_post(mutex_expulsado);
			return;
		}else{
			sem_post(mutex_expulsado);
		}
	}

	//tripu->tarea_a_realizar= obtener_siguiente_tarea(tripu->numero_patota);

	tripu->tarea_a_realizar= NULL;

	if(tripu->tarea_a_realizar!=NULL){
		block_ready(tripu);
	}else{
		block_exit(tripu);
	}
}

void descartar_basura(tripulante_plani* tripu) {

	sem_wait(tripu->sem_tripu);
	//llamar al i-mongo y gastar 1 ciclo de cpu
	sleep(RETARDO_CICLO_CPU);
	sem_post(tripu->sem_tripu);

	sem_wait(mutex_expulsado);
	if(tripu->expulsado){
		sem_post(mutex_expulsado);
		return;
	}else{
		sem_post(mutex_expulsado);
	}

	running_block(tripu);

	//if(SI ESTA EL ARCHIVO) {
	//	eliminar_archivo("Basura.ims");
	//}
	//else {
	//	log_info(logger, "El archivo 'Basura.ims' no existe. \n");
	//}

	uint32_t tiempo_restante = tripu->tarea_a_realizar->tiempo;

	while(tiempo_restante != 0){
		sem_wait(tripu->sem_tripu);
		sleep(RETARDO_CICLO_CPU);
		tiempo_restante--;
		sem_post(tripu->sem_tripu);

		sem_wait(mutex_expulsado);
		if(tripu->expulsado){
			sem_post(mutex_expulsado);
			return;
		}else{
			sem_post(mutex_expulsado);
		}
	}

	//tripu->tarea_a_realizar= obtener_siguiente_tarea(tripu->numero_patota);

	tripu->tarea_a_realizar= NULL;

	if(tripu->tarea_a_realizar!=NULL){
		block_ready(tripu);
	}else{
		block_exit(tripu);
	}
}

void otras_tareas(tripulante_plani* tripu,uint32_t* cantidadRealizado){

	uint32_t tiempo_restante = tripu->tarea_a_realizar->tiempo;

	while(tiempo_restante > 0){
		if(*cantidadRealizado==QUANTUM){
			running_ready(tripu);

			*cantidadRealizado=0;
			sem_wait(tripu->sem_planificacion);

			sem_wait(mutex_expulsado);
			if(tripu->expulsado){
				sem_post(mutex_expulsado);
				return;
			}else{
				sem_post(mutex_expulsado);
			}
		}
		sem_wait(tripu->sem_tripu);
		sleep(RETARDO_CICLO_CPU);
		tiempo_restante--;
		sem_post(tripu->sem_tripu);

		sem_wait(mutex_expulsado);
		if(tripu->expulsado){
			sem_post(mutex_expulsado);
			return;
		}else{
			sem_post(mutex_expulsado);
		}
	}
	//tripu->tarea_a_realizar= obtener_siguiente_tarea(tripu->numero_patota);

	tripu->tarea_a_realizar= NULL;

	if(tripu->tarea_a_realizar!=NULL){
		running_ready(tripu);
	}else{
		running_exit(tripu);
	}
}

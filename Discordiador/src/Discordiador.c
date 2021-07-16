#include "Discordiador.h"

int main(void) {


	logger = crear_log("discordiador.log", "Discordiador");
	logger_sin_pantalla =  log_create("discordiador.log", "logger_sin_pantalla" , 0, LOG_LEVEL_INFO);;

	config = crear_config(CONFIG_PATH);
	obtener_datos_de_config(config);
	obtener_planificacion_de_config(config);

	elegir_algoritmo();
	inicializar_semaforos();
	inicializar_listas();
	iniciar_planificacion();

	crear_hilos();

	//while(1){
	sem_wait(finalizar_programa);
	//DISCORDIADOR COMO SERVIDOR DE MONGO-STORE
	// Conexion de escucha con MongoStore?  POR SABOTAJE
	//sem_wait(sabotaje);
	//pthread_create(&hilo_sabotaje, NULL, (void*)iniciar_escucha_sabotaje, NULL);

	//pthread_detach(hilo_sabotaje);
	//}
	return EXIT_SUCCESS;
}

void inicializar_listas(){
	lista_semaforos_tripulantes = list_create();

	lista_tripulantes = list_create();
}

void inicializar_semaforos() {
	mutex_sabotaje = malloc(sizeof(sem_t));
	sem_init(mutex_sabotaje, 0, 1);

	comando_para_ejecutar = malloc(sizeof(sem_t));
	sem_init(comando_para_ejecutar, 0, 1);

	termino_operacion = malloc(sizeof(sem_t));
	sem_init(termino_operacion, 0, 1);

	finalizar_programa=malloc(sizeof(sem_t));
	sem_init(finalizar_programa, 0, 0);

	termine_sabotaje = malloc(sizeof(sem_t));
	sem_init(termine_sabotaje, 0, 0);
}


void finalizar_semaforos() {
	//free(sabotaje);
	free(termino_operacion);
	free(comando_para_ejecutar);
}


void crear_hilos(){
	pthread_create(&hilo_consola, NULL,(void*)iniciar_escucha_por_consola, NULL);
	pthread_detach(hilo_consola);

	//pthread_create(&hilo_sabotaje, NULL,(void*)iniciar_escucha_sabotaje, NULL);
	//pthread_detach(hilo_sabotaje);

	pthread_create(&hilo_new_ready, NULL,(void*)new_ready, NULL);
	pthread_detach(hilo_new_ready);

	pthread_create(&hilo_ready_running, NULL,(void*)ready_running, NULL);
	pthread_detach(hilo_ready_running);

	pthread_create(&hilo_creador_rafagas, NULL,(void*)rafaga_cpu, lista_tripulantes);
	pthread_detach(hilo_creador_rafagas);

}

void iniciar_escucha_sabotaje(void){

	//conexion_sabotaje = iniciar_servidor(IP_MONGO_STORE, PUERTO_MONGO_STORE);



	//int32_t* conexion_cliente = esperar_conexion(conexion_sabotaje);

	//codigo_operacion operacion;

	//recv(*conexion_cliente, &operacion, sizeof(operacion), MSG_WAITALL);

	//switch(operacion){
	//	case posiciones:

    //}


	//
	//
	//while(1){

		t_respuesta_mongo* respuesta=malloc(sizeof(t_respuesta_mongo));
		//recibir_mensaje(respuesta, RECIBIR_SABOTAJE, conexion_sabotaje);



		//la señal te llega  filesystem tiene ip y puerto de disc

		int largo;

		tripulante_plani* tripu_mas_cercano = malloc(sizeof(tripulante_plani));

		tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));

		//Para parar los hilos de planificacion

		sem_wait(mutex_new_ready);
		new_ready_off = 1;
		sem_post(mutex_new_ready);

		sem_wait(mutex_ready_running);
		ready_running_off = 1;
		sem_post(mutex_ready_running);

		sem_wait(mutex_rafaga);
		dar_pulsos_off = 1;
		sem_post(mutex_rafaga);


		largo = list_size(lista_tripulantes);
		for(int i=0;i<largo;i++){
			tripulante = list_get(lista_tripulantes,i);
			if(tripulante->estado == 'E'){
				tripulante->estado_anterior = 'E';
				list_add_sorted(bloqueado_suspendido, (void*) tripulante,(void*)menorId);
				running_suspendido(tripulante);
			}
		}

		for(int i=0;i<largo;i++){
			tripulante = list_get(lista_tripulantes,i);
			if(tripulante->estado == 'R'){
				tripulante->estado_anterior = 'R';
				list_add_sorted(bloqueado_suspendido_ready, (void*) tripulante,(void*)menorId);
				ready_suspendido(tripulante);
			}
		}

		list_add_all(bloqueado_suspendido,bloqueado_suspendido_ready);


		//se supone q te viene la tarea de imongo, le buscas la poscion
	//ESTO VA A IR



		/*esto va
		posicion_sabotaje=malloc(sizeof(posiciones));
		posicion_sabotaje->posicion_x=respuesta->tarea_sabotaje->posicion_x;
		posicion_sabotaje->posicion_y=respuesta->tarea_sabotaje->posicion_y;

		*/






		//posicion_sabotaje->posicion_x = respuesta->tarea_sabotaje->posicion_x;
		//posicion_sabotaje->posicion_y = respuesta->tarea_sabotaje->posicion_y;


	 //esto vas a pedir memoria cuando aceptas el mensaje no aca


		//hardcodeado

		//TODO RESPUESTA de imongo
		posicion_sabotaje=malloc(sizeof(posiciones));
		posicion_sabotaje->posicion_x=10;
		posicion_sabotaje->posicion_y=10;


		//HAY Q SACARLO DE LA LISTA AL MAS CERCANO
		int largoaa=list_size(bloqueado_suspendido);
		printf("largo lista %u",largoaa);
		fflush(stdout);

		tripu_mas_cercano = list_fold1(bloqueado_suspendido, (void*) mas_cercano);

		printf("id asignado: %u",tripu_mas_cercano->id_tripulante);
		fflush(stdout);

		int indice=obtener_indice(bloqueado_suspendido, tripu_mas_cercano);
		list_remove(bloqueado_suspendido, indice);


		for(int i=0;i<largo;i++){
			tripulante = list_get(lista_tripulantes,i);
			if(tripulante->estado == 'B'){
				tripulante->estado_anterior = 'B';
				list_add(bloqueado_suspendido,tripulante);
				actualizar_estado(tripulante, 'S');
			}
		}
//


		t_tarea* ayuda=malloc(sizeof(t_tarea));
		ayuda->posicion_x=10;
		ayuda->posicion_y=10;
		ayuda->operacion=REALIZAR_TAREA_SABOTAJE;
		ayuda->cantidad=0;
		ayuda->tiempo=0;

		tripu_mas_cercano->tarea_auxiliar=ayuda;

		actualizar_estado(tripu_mas_cercano, 'E');
		sem_post(mutex_rafaga);
		tripu_mas_cercano->elegido_sabotaje=1;



		sem_wait(mutex_rafaga);
		dar_pulsos_off=0;
		sem_post(mutex_rafaga);
		sem_post(planificion_rafaga);

		if(tripu_mas_cercano->estado_anterior=='R'){
			sem_post(tripu_mas_cercano->sem_planificacion);
		}else{
			sem_post(tripu_mas_cercano->sem_tripu);
		}


		sem_wait(termine_sabotaje);

		largo = list_size(bloqueado_suspendido);

		for(int i=0;i<largo;i++){
			tripulante = list_get(bloqueado_suspendido,i);
			tripulante->cantidad_realizada = 0;
			suspendido_ready(tripulante);
		}


		list_clean(bloqueado_suspendido);


		//Vuelve a activar los hilos de planificacion
/*
		sem_wait(mutex_new_ready);
		new_ready_off = 0;
		sem_post(mutex_new_ready);

		if(new_ready_off==0){
			sem_post(planificacion_on);
		}


		sem_wait(mutex_ready_running);
		ready_running_off = 0;
		sem_post(mutex_ready_running);

		if(ready_running_off==0){
			sem_post(planificacion_on_ready_running);
		}
*/
		sem_wait(mutex_rafaga);
		dar_pulsos_off = 1;
		sem_post(mutex_rafaga);

//		if(dar_pulsos_off==0){
//			sem_post(planificion_rafaga);
//		}

	//	tripu_mas_cercano->estado='E';

		posicion_sabotaje=NULL;
		tripu_mas_cercano=NULL;
		tripulante=NULL;
		respuesta=NULL;

		free(respuesta);
		free(posicion_sabotaje);
		free(tripulante);
		free(tripu_mas_cercano);
		//FINAL
	//	sem_wait(mutex_sabotaje);
	//	valor_sabotaje=0;
	//	sem_post(mutex_sabotaje);
	//}

}



tripulante_plani* mas_cercano(tripulante_plani* tripulante1, tripulante_plani* tripulante2){
	//tenemos variable global q dice la posicion del sabotaje

	posiciones* posicion_tripu1 = malloc(sizeof(posiciones));
	posicion_tripu1 = obtener_posiciones(tripulante1->id_tripulante, tripulante1->numero_patota);

	posiciones* posicion_tripu2 = malloc(sizeof(posiciones));
	posicion_tripu2 = obtener_posiciones(tripulante2->id_tripulante, tripulante2->numero_patota);

	int32_t distancia1 = obtener_distancia(posicion_tripu1, posicion_sabotaje);
	int32_t distancia2 = obtener_distancia(posicion_tripu2, posicion_sabotaje);

	if(distancia1 <= distancia2) {
		return tripulante1;
	}
	else {
		return tripulante2;
	}

	//obtener_distancia(posicion_tripu1, posicion_tripu2);

	//aca esTA MAL

}



bool menorId(tripulante_plani* tripulante1, tripulante_plani* tripulante2){
	return tripulante1->id_tripulante<tripulante2->id_tripulante;
}


void iniciar_escucha_por_consola(){

	while(1){
		sem_wait(comando_para_ejecutar);
		obtener_orden_input();
	}

}

void obtener_datos_de_config(t_config* config) {

	IP_MI_RAM = config_get_string_value(config, "IP_MI_RAM_HQ");
	PUERTO_MI_RAM = config_get_string_value(config, "PUERTO_MI_RAM_HQ");

	IP_MONGO_STORE = config_get_string_value(config, "IP_I_MONGO_STORE");
	PUERTO_MONGO_STORE = config_get_string_value(config, "PUERTO_I_MONGO_STORE");

	DURACION_SABOTAJE = config_get_int_value(config, "DURACION_SABOTAJE");
	RETARDO_CICLO_CPU = config_get_int_value(config, "RETARDO_CICLO_CPU");

}

void obtener_orden_input(){

	t_respuesta_iniciar_patota* respuesta_iniciar_patota;
	t_respuesta_tripulante* respuesta_al_expulsar_tripulante;

	tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));
	int largo;
	int recorrido;

	 char* cadena_ingresada = NULL;
	 size_t longitud = 0;

	 sem_wait(termino_operacion);

	 printf("Inserte orden:\n");

	 getline(&cadena_ingresada, &longitud, stdin);  //Bloqueante por eso no consume el 100%

	 string_trim(&cadena_ingresada);

	 parser_consola = string_split(cadena_ingresada, " ");

	 char* comando_ingresado = malloc(strlen(parser_consola[0])+1);

	 strcpy(comando_ingresado, parser_consola[0]);

	 sem_post(comando_para_ejecutar);
	 operacion = mapeo_valor_consola(comando_ingresado);
	 free(comando_ingresado);

	 //int32_t valor_semaforo;
	// tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));
	 //tripulante_plani* tripulante_a_ready = malloc(sizeof(tripulante_plani));

	 //int32_t valor_semaforo;

	 tripulante_plani* tripulante_a_expulsar;
	 t_tripulante* id_tripulante_a_expulsar;



	// sem_t* saca = malloc(sizeof(sem_t));

	 int valorPulsos;
	 switch(operacion){

		case INICIAR_PLANIFICACION:
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/tareasPatota5.txt 5|5 5|5 5|5
			// Ej: INICIAR_PATOTA 1 /home/utnso/tareas/tareasPatota5.txt 5|5
			// Ej: INICIAR_PATOTA 2 /home/utnso/tareas/tareasPatota1.txt 7|1 2|0
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/tareasPatota1.txt 7|1
			// Ej: INICIAR_PATOTA 1 /home/utnso/tareas/tareasPatota1.txt 7|1
			// PRUEBAS PARA DISCORDIADOR
			// Ej: INICIAR_PATOTA 2 /home/utnso/tareas/plantas.txt 1|1 3|4
			// Ej: INICIAR_PATOTA 1 /home/utnso/tareas/oxigeno.txt 5|5
			// PRUEBAS PARA MI RAM
			// Ej: INICIAR_PATOTA 10 /home/utnso/tareas/espartana.txt
			// Ej: INICIAR_PATOTA 6 /home/utnso/tareas/persa.txt
			// ARRANCA LA PLANIFICACION DE LOS TRIPULANTES (BUSCANDO EL ALGORITMO QUE ESTA EN CONFIG)
			//sem_getvalue(planificacion_on,&valor_semaforo);

			//if(valor_semaforo == 0){
			log_info(logger, "Iniciando Planificacion....... \n");
			//}

			sem_wait(mutex_new_ready);
			new_ready_off = 0;
			sem_post(mutex_new_ready);

			sem_wait(mutex_ready_running);
			ready_running_off = 0;
			sem_post(mutex_ready_running);

			sem_wait(mutex_rafaga);
			dar_pulsos_off = 0;
			sem_post(mutex_rafaga);

			sem_post(planificacion_on);
			sem_post(planificacion_on_ready_running);
			sem_post(planificion_rafaga);
			break;

		case PAUSAR_PLANIFICACION:

			log_info(logger, "Pausando Planificacion........ \n");
			//list_map(lista_semaforos_tripulantes, (void*) poner_en_cero_semaforos);

			//sem_wait(planificacion_on);
			//sem_wait(planificacion_on_ready_running);
			//sem_wait(planificion_rafaga);

			sem_wait(mutex_new_ready);
			new_ready_off = 1;
			sem_post(mutex_new_ready);

			sem_wait(mutex_ready_running);
			ready_running_off = 1;
			sem_post(mutex_ready_running);

			sem_wait(mutex_rafaga);
			dar_pulsos_off = 1;
			sem_post(mutex_rafaga);

			break;

		case INICIAR_PATOTA:
			// Ej: INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 5|5 1|1 2|0
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/tareasPatota5.txt 5|5 5|5 5|5
			// Ej: INICIAR_PATOTA 1 /home/utnso/tareas/tareasPatota5.txt 1|1
			// Ej: INICIAR_PATOTA 2 /home/utnso/tareas/tareasPatota1.txt 1|1 2|0
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/tareasPatota1.txt 7|1
			// Ej: INICIAR_PATOTA 1 /home/utnso/tareas/tareasPatota1.txt 7|1

			// PRUEBAS PARA DISCORDIADOR
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/ES3_Patota1.txt 9|9 0|0 5|5
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/ES3_Patota2.txt 4|0 2|6 8|2
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/ES3_Patota3.txt 2|3 5|8 5|3
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/ES3_Patota4.txt 0|9 4|4 9|0
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/ES3_Patota5.txt 0|2 9|6 3|5

			// PRUEBAS PARA MI RAM
			// Ej: INICIAR_PATOTA 4 /home/utnso/tareas/SEG_PatotaA.txt
			// Ej: INICIAR_PATOTA 2 /home/utnso/tareas/SEG_PatotaB.txt
			// Ej: INICIAR_PATOTA 1 /home/utnso/tareas/SEG_PatotaC.txt

			// LISTAR_TRIPULANTES
			if(parser_consola[1] == NULL || parser_consola[2] == NULL){
				log_error(logger, "Faltan argumentos. Debe iniciarse de la forma: INICIAR_PATOTA <CantidadTripulantes> >Ubicación txt Tareas>.");
				break;
			}

			uint32_t cantidad_argumentos = cantidad_argumentos_ingresados(parser_consola);
			uint32_t cantidad_posiciones = cantidad_argumentos - 3;
			int cantidad_tripulantes = atoi(parser_consola[1]);
			int posiciones_faltantes = cantidad_tripulantes - cantidad_posiciones;
			respuesta_iniciar_patota = malloc(sizeof(t_respuesta_iniciar_patota));

			if(posiciones_faltantes < 0) {
				log_error(logger, "Se ingresaron posiciones demás. Solo puede como máximo haber tantas posiciones como cantidad de tripulantes.\n");
				break;
			}

			strcat(parser_consola[1],"\0");
			strcat(parser_consola[2],"\0");

			// Indica que arranca a leer a partir de los primeros 3 argumentos: COMANDO CANTIDAD_TRIPULANTES ARCHIVO_TAREAS
			int ubicacion_parser = 3;
			// Meter todas las posiciones de los tripulantes en una sola cadena
			char* posiciones = string_new();
			while(parser_consola[ubicacion_parser] != NULL && ubicacion_parser < cantidad_argumentos) {
				string_append_with_format(&posiciones, "%s|", parser_consola[ubicacion_parser]);
				ubicacion_parser++;
			}

			for(int i = 0; i<posiciones_faltantes; i++) {
				string_append_with_format(&posiciones, "%s|", "0|0");
			}
			strcat(posiciones, "\0");


			// Conexión con Mi-RAM HQ
			conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);
			if(resultado_conexion(conexion_mi_ram, logger, "Mi-RAM HQ") == -1){
				break;
			}


			// Obtiene el contenido del Archivo de Tareas de la Patota
			FILE* archivo_tareas = fopen(parser_consola[2], "r");

				if(archivo_tareas > 0) {
					printf("El archivo existe en %s.\n", parser_consola[2]);
				}
				else {
					log_error(logger, "El archivo %s no existe. \n", parser_consola[2]);
					break;
				}

			fseek(archivo_tareas, 0, SEEK_END);
			long int tamanio_archivo = ftell(archivo_tareas);
			rewind(archivo_tareas);

			char* tareas_totales = malloc(sizeof(char)*tamanio_archivo);

			fread(tareas_totales, tamanio_archivo+1, 1, archivo_tareas);
			tareas_totales = string_substring_until(tareas_totales, tamanio_archivo);
			strcat(tareas_totales, "\0");


			// Creo la estructura de datos de la Patota
			t_iniciar_patota* mensaje_patota = malloc(sizeof(t_iniciar_patota));
			mensaje_patota->cantidad_tripulantes = atoi(parser_consola[1]);

			mensaje_patota->tamanio_tareas = strlen(tareas_totales);
			mensaje_patota->tareas_de_patota = malloc(mensaje_patota->tamanio_tareas+1);
			strcpy(mensaje_patota->tareas_de_patota, tareas_totales);

			mensaje_patota->tamanio_posiciones = strlen(posiciones);
			mensaje_patota->posiciones = malloc(mensaje_patota->tamanio_posiciones+1);
			strcpy(mensaje_patota->posiciones, posiciones);
			//mensaje_patota->pid_patota = pcb_patota->pid;

			enviar_mensaje(mensaje_patota, INICIAR_PATOTA, conexion_mi_ram);

			if(validacion_envio(conexion_mi_ram) == 1) {
				recibir_mensaje(respuesta_iniciar_patota, RESPUESTA_INICIAR_PATOTA, conexion_mi_ram);

				if(respuesta_iniciar_patota->respuesta == 1) {
					printf("La respuesta fue positiva. \n");
					printf("los id de los tripulantes %s\n",respuesta_iniciar_patota->ids_tripu);
					printf("Numero de patota: %u \n", respuesta_iniciar_patota->numero_de_patota);

					char** parser_ids = string_split(respuesta_iniciar_patota->ids_tripu, "|");

					for(int i=0; mensaje_patota->cantidad_tripulantes>i; i++) {

						tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));
						tripulante->id_tripulante = atoi(parser_ids[i]);
						tripulante->numero_patota = respuesta_iniciar_patota->numero_de_patota; //esto lo devuelve mi ram
						tripulante->estado = 'N';
						tripulante->estado_anterior = 'N';
						tripulante->expulsado=0;
						tripulante->elegido_sabotaje=0;
						tripulante->fui_elegido_antes=0;
						tripulante->cantidad_realizada=0;


						sem_t* sem_plani=malloc(sizeof(sem_t));
						sem_init(sem_plani,0,0);
						tripulante->sem_planificacion = sem_plani;


						sem_t* semaforo_exec = malloc(sizeof(sem_t));
						sem_init(semaforo_exec, 0, 0);
						tripulante->sem_tripu = semaforo_exec;


						sem_t* mutex_estado_tripu = malloc(sizeof(sem_t));
						sem_init(mutex_estado_tripu, 0, 1);
						tripulante->mutex_estado=mutex_estado_tripu;


						sem_t* mutex_tripu_expulsado=malloc(sizeof(sem_t));
						sem_init(mutex_tripu_expulsado, 0, 1);

						tripulante->mutex_expulsado=mutex_tripu_expulsado;

						pthread_create(&hilo_tripulante,NULL,(void*)tripulante_hilo,tripulante);
						pthread_detach(hilo_tripulante);

						list_add(lista_semaforos_tripulantes, tripulante->sem_tripu); //Creo que no se usan mas
						list_add(lista_tripulantes,tripulante);

						sem_wait(mutex_new);
						queue_push(cola_new,tripulante);
						sem_post(mutex_new);


						sem_post(contador_tripulantes_en_new);
					}

				}
				else {
					log_error(logger, "No hay espacio para almacenar la patota con sus tripulantes. \n");			// Salgo del Switch, ya que no pudo crearse la Patota en Mi-RAM HQ
					break;
				}
			}

			cerrar_conexion(logger, conexion_mi_ram);

			// Libero la memoria usada
			fclose(archivo_tareas);
			free(tareas_totales);
			free(respuesta_iniciar_patota);

			free(posiciones);
			free(parser_posiciones);

			free(mensaje_patota->tareas_de_patota);
			free(mensaje_patota->posiciones);
			free(mensaje_patota);
			break;


		case LISTAR_TRIPULANTES:

			if(parser_consola[1] != NULL) {
				log_warning(logger, "Sobran argumentos. Debe iniciarse de la forma LISTAR_TRIPULANTES.");
				break;

			}

			largo = list_size(lista_tripulantes);

			printf("-------------------------------------------------------------------------\n");
			printf("Estado de la Nave: %s \n", temporal_get_string_time("%d/%m/%y %H:%M:%S"));

			for(recorrido=0; recorrido<largo; recorrido++){
				tripulante=list_get(lista_tripulantes, recorrido);
				printf("Tripulante: %u          Patota: %u          Status: %c \n", tripulante->id_tripulante, tripulante->numero_patota, tripulante->estado);
			}
			printf( "--------------------------------------------------------------------------\n\n");
			break;

		case OBTENER_BITACORA:

			iniciar_escucha_sabotaje();
			/*
			if(parser_consola[1] == NULL) {
				log_error(logger, "Faltan argumentos. Debe inciarse de la forma OBTENER_BITACORA <Id_Tripulante>.");
				break;
			}
			strcat(parser_consola[1], "\0");

			t_tripulante* id_tripulante_x_bitacora = malloc(sizeof(t_tripulante));
			id_tripulante_x_bitacora->id_tripulante = atoi(parser_consola[1]);

			conexion_mongo_store = crear_conexion(IP_MONGO_STORE, PUERTO_MONGO_STORE);

			if(conexion_mongo_store < 0) {
				log_error(logger, "No se pudo conectar. \n");
				break;
			}

			enviar_mensaje(id_tripulante_x_bitacora, OBTENER_BITACORA, conexion_mongo_store);
			*/
			 /*
			 * ACA TIENE QUE RECIBIR UN MENSAJE DE MONGO STORE PARA MOSTRAR LA BITACORA
			 * 					O
			 * MONGO STORE IMPRIME LA BITACORA
			 */

			cerrar_conexion(logger,conexion_mongo_store);
			//free(id_tripulante_x_bitacora);
			//cerrar_conexion(logger,conexion_mongo_store);

			break;

		case EXPULSAR_TRIPULANTE:

			if(parser_consola[1] == NULL) {
				log_error(logger, "Faltan argumentos. Debe inciarse de la forma EXPULSAR_TRIPULANTE <Id_Tripulante>.");
				break;
			}
			strcat(parser_consola[1], "\0");

			id_tripulante_a_expulsar = malloc(sizeof(t_tripulante));
			id_tripulante_a_expulsar->id_tripulante = atoi(parser_consola[1]);
			respuesta_al_expulsar_tripulante = malloc(sizeof(t_respuesta_tripulante));
			char estado_anterior;

			bool mismo_id(tripulante_plani* tripu) {
				return tripu->id_tripulante == id_tripulante_a_expulsar->id_tripulante;
			}

			tripulante_a_expulsar = malloc(sizeof(tripulante_plani));
			tripulante_a_expulsar = list_find(lista_tripulantes, (void*)mismo_id);

			if(tripulante_a_expulsar != NULL) {

				id_tripulante_a_expulsar->id_patota = tripulante_a_expulsar->numero_patota;

				printf("ID del Tripulante a eliminar: %u \n",id_tripulante_a_expulsar->id_tripulante);
				printf("ID de la Patota del Tripulante a eliminar: %u \n",id_tripulante_a_expulsar->id_patota);

				estado_anterior = tripulante_a_expulsar->estado;

				switch(tripulante_a_expulsar->estado){

						case 'R':
							tripulante_a_expulsar->expulsado = 1;
							sem_post(tripulante_a_expulsar->sem_planificacion);
							ready_exit(tripulante_a_expulsar);
							break;

						case 'E':
							tripulante_a_expulsar->expulsado = 1;
							running_exit(tripulante_a_expulsar);
							break;

						case 'B':
							tripulante_a_expulsar->expulsado = 1;
							block_exit(tripulante_a_expulsar);
							break;

				}

					// No se puede expulsar un tripulante si este está Terminado o como Nuevo (antes de haber iniciado Planificacion)

				if(estado_anterior != 'T' && estado_anterior != 'N'){
					conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);

					if(resultado_conexion(conexion_mi_ram, logger, "Mi-RAM HQ") == -1){
						break;
					}

					enviar_mensaje(id_tripulante_a_expulsar, EXPULSAR_TRIPULANTE, conexion_mi_ram);

					if(validacion_envio(conexion_mi_ram) == 1) {
						recibir_mensaje(respuesta_al_expulsar_tripulante, RESPUESTA_TRIPULANTE_ELIMINADO, conexion_mi_ram);

						if(respuesta_al_expulsar_tripulante->respuesta != 1) {
							log_error(logger, "No se pudo expulsar al Tripulante %u.\n", respuesta_al_expulsar_tripulante->id_tripulante);
							abort();
						}

						log_info(logger, "Se expulsó al Tripulante %u.\n", respuesta_al_expulsar_tripulante->id_tripulante);


					}else{
						log_error(logger, "No se pudo enviar el mensaje a Mi-RAM.\n");
						abort();
					}
					cerrar_conexion(logger,conexion_mi_ram);
				}
					// En el caso que se quiera expulsar un Tripulante estando en Terminated
				else if(estado_anterior == 'T'){
					log_error(logger, "Se quiso eliminar un tripulante que ya estaba terminado.\n");
					}
					// En el caso que se quiera expulsar un Tripulante estando en Terminated

				else {
					log_warning(logger, "No se puede eliminar un Tripulante cuando no se ha iniciado la Planificación.\n");
				}

				}else {
					log_error(logger, "No existe el tripulante que se desea eliminar.\n");
				}

			   free(id_tripulante_a_expulsar);
			   free(respuesta_al_expulsar_tripulante);
						//free(tripulante_a_expulsar);
			   break;


		case TERMINAR_PROGRAMA:

			conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);
			if(resultado_conexion(conexion_mi_ram, logger, "Mi-RAM HQ") != -1){
				enviar_mensaje("", CERRAR_MODULO, conexion_mi_ram);
				cerrar_conexion(logger, conexion_mi_ram);
			}
			conexion_mongo_store = crear_conexion(IP_MONGO_STORE, PUERTO_MONGO_STORE);
	  		if(resultado_conexion(conexion_mongo_store, logger, "Mongo Store") != -1) {
	  			enviar_mensaje("", CERRAR_MODULO, conexion_mongo_store);
	 			cerrar_conexion(logger, conexion_mongo_store);
	 			}

			printf("Terminando programa... \n");
			sleep(1);
			printf("-------------------------------------------------------------------------------------------------------------------------------------------------- \n");
			// Libero memoria
			free(parser_consola);
			free(cadena_ingresada);
			finalizar_semaforos();
			finalizar_semaforos_plani();
			terminar_programa(config, logger);
			sem_post(finalizar_programa);

			break;


		default:
			printf("No se reconoce ese comando. Por favor, ingrese un comando válido.\n");
			break;
		}

	sem_post(termino_operacion);

	free(parser_consola);
	free(cadena_ingresada);

}




int obtener_indice(t_list* lista, void* valor) {

	int indice;
	for(int i = 0; i<list_size(lista); i++) {
		if(list_get(lista, i) == valor) {
			indice = i;
		}
	}
	return indice;
}




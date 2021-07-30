#include "Discordiador.h"

int main(void) {

	logger = crear_log_sin_pantalla("Discordiador.log", "Discordiador");
	logger_on = crear_log("DiscordiadorOn.log", "Discordiador");

	config = crear_config(CONFIG_PATH);
	obtener_datos_de_config(config);
	obtener_planificacion_de_config(config);

	elegir_algoritmo();
	inicializar_semaforos();
	inicializar_listas();
	iniciar_planificacion();

	crear_hilos();

	sem_wait(finalizar_programa);
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

	pthread_create(&hilo_sabotaje, NULL,(void*)iniciar_escucha_sabotaje, NULL);
	pthread_detach(hilo_sabotaje);

	pthread_create(&hilo_new_ready, NULL,(void*)new_ready, NULL);
	pthread_detach(hilo_new_ready);

	pthread_create(&hilo_ready_running, NULL,(void*)ready_running, NULL);
	pthread_detach(hilo_ready_running);

	pthread_create(&hilo_creador_rafagas, NULL,(void*)rafaga_cpu, lista_tripulantes);
	pthread_detach(hilo_creador_rafagas);

	pthread_create(&hilo_susp_block, NULL,(void*)esperandoIo_bloqueado, NULL);
	pthread_detach(hilo_susp_block);

}

void iniciar_escucha_sabotaje(void){
	conexion_sabotaje = iniciar_servidor(IP_DISCORDIADOR, PUERTO_DISCORDIADOR);

	while(1) {
		int32_t conexion_cliente = esperar_conexion(conexion_sabotaje);
		pthread_create(&hilo_tripulante_sabo, NULL, (void*)escuchar_conexion, (int32_t*)conexion_cliente);
		pthread_detach(hilo_tripulante_sabo);
	}
}

void procesar_mensajes(codigo_operacion operacion, int32_t conexion_cliente) {

	posicion_sabotaje* posicion_recibida;

	switch(operacion){

		case SABOTAJE:
			posicion_recibida = malloc(sizeof(posicion_sabotaje));
			recibir_mensaje(posicion_recibida, operacion, conexion_cliente);

			//la señal te llega filesystem tiene ip y puerto de disc

			/*int largo;

			tripulante_plani* tripu_mas_cercano;

			tripulante_plani* tripulante;

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

			sleep(1);

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

			list_add_all(bloqueado_suspendido, bloqueado_suspendido_ready);


			//se supone q te viene la tarea de imongo, le buscas la poscion
		//ESTO VA A IR



			//esto va
			posicion_sabotaje=malloc(sizeof(posiciones));
			posicion_sabotaje->posicion_x=respuesta->tarea_sabotaje->posicion_x;
			posicion_sabotaje->posicion_y=respuesta->tarea_sabotaje->posicion_y;


			//posicion_sabotaje->posicion_x = respuesta->tarea_sabotaje->posicion_x;
			//posicion_sabotaje->posicion_y = respuesta->tarea_sabotaje->posicion_y;


		 //esto vas a pedir memoria cuando aceptas el mensaje no aca


			//hardcodeado*/

			//TODO RESPUESTA de imongo
			/*posicion_sabotaje=malloc(sizeof(posiciones));
			posicion_sabotaje->posicion_x=10;
			posicion_sabotaje->posicion_y=10;


			//cuidado sin la lista esta vacia
			tripu_mas_cercano = list_fold1(bloqueado_suspendido, (void*) mas_cercano);

			log_info(logger, "El tripulante con id %u, corre en panico hacia el sabotaje", tripu_mas_cercano->id_tripulante);

			int indice=obtener_indice(bloqueado_suspendido, tripu_mas_cercano);
			list_remove(bloqueado_suspendido, indice);


			for(int i=0;i<largo;i++){
				tripulante = list_get(lista_tripulantes,i);
				if(tripulante->estado == 'B'){
					tripulante->estado_anterior = 'B';
					if(tripulante->puedo_ejecutar_io==1){
						tripulante->puedo_ejecutar_io=0;
						sem_post(bloqueado_disponible);
					}
					list_add(bloqueado_suspendido,tripulante);
					actualizar_estado(tripulante, 'S');
					log_info(logger,"El tripulante con id %d de la patota %d paso de Block a Block Suspended",tripulante->id_tripulante,tripulante->numero_patota);
				}
			}



			t_tarea* ayuda=malloc(sizeof(t_tarea));
			ayuda->posicion_x=10;
			ayuda->posicion_y=10;
			ayuda->operacion=REALIZAR_TAREA_SABOTAJE;
			ayuda->cantidad=0;
			ayuda->tiempo=0;

			tripu_mas_cercano->tarea_auxiliar=ayuda;

			actualizar_estado(tripu_mas_cercano, 'E');
			log_info(logger,"El tripulante con id %d de la patota %d paso de Block Suspended a Execute",tripu_mas_cercano->id_tripulante,tripu_mas_cercano->numero_patota);
			// TODO aca seria corre a sabotaje
			tripu_mas_cercano->elegido_sabotaje=1;



			sem_wait(mutex_rafaga);
			dar_pulsos_off=0;
			sem_post(mutex_rafaga);
			sem_post(planificion_rafaga);

			if(tripu_mas_cercano->estado_anterior=='R'){
				sem_post(tripu_mas_cercano->sem_planificacion);
			}else{
				//sem_post(tripu_mas_cercano->sem_tripu);
			}


			sem_wait(termine_sabotaje);
			// TODO aca mandar a mongo resolvi sabotaje
			queue_clean(cola_ready);

			tripu_mas_cercano->estado_anterior='R';
		//	sem_wait(tripu_mas_cercano->sem_tripu);

			largo = list_size(bloqueado_suspendido);

			for(int i=0;i<largo;i++){
				tripulante = list_get(bloqueado_suspendido,i);
				tripulante->cantidad_realizada = 0;
				suspendido_ready(tripulante);
			}


			list_clean(bloqueado_suspendido);*/

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
			/*sem_wait(mutex_rafaga);
			dar_pulsos_off = 1;
			sem_post(mutex_rafaga);

		//		if(dar_pulsos_off==0){
		//			sem_post(planificion_rafaga);
		//		}

		//	tripu_mas_cercano->estado='E';

			posicion_sabotaje=NULL;
			respuesta=NULL;

			free(respuesta);
			free(posicion_sabotaje);
*/
			//FINAL
		//	sem_wait(mutex_sabotaje);
		//	valor_sabotaje=0;
		//	sem_post(mutex_sabotaje);
		//}


/*			tripulante_plani* mas_cercano(tripulante_plani* tripulante1, tripulante_plani* tripulante2){
			//tenemos variable global q dice la posicion del sabotaje

			posiciones* posicion_tripu1 = malloc(sizeof(posiciones));
			obtener_posiciones(posicion_tripu1,tripulante1->id_tripulante, tripulante1->numero_patota);

			posiciones* posicion_tripu2 = malloc(sizeof(posiciones));
			obtener_posiciones(posicion_tripu2,tripulante2->id_tripulante, tripulante2->numero_patota);

			int32_t distancia1 = obtener_distancia(posicion_tripu1, posicion_sabotaje);
			int32_t distancia2 = obtener_distancia(posicion_tripu2, posicion_sabotaje);

			posicion_tripu1=NULL;
			posicion_tripu2=NULL;

			free(posicion_tripu1);
			free(posicion_tripu2);
			if(distancia1 <= distancia2) {
				return tripulante1;
			}
			else {
				return tripulante2;
			}

			//obtener_distancia(posicion_tripu1, posicion_tripu2);

			//aca esTA MAL

		}*/
		break;

		default:
			break;
	}
	return;
}


bool menorId(tripulante_plani* tripulante1, tripulante_plani* tripulante2){
	return tripulante1->id_tripulante < tripulante2->id_tripulante;
}


void iniciar_escucha_por_consola(){

	while(1){
		sem_wait(comando_para_ejecutar);
		obtener_orden_input();
	}

}

void obtener_datos_de_config(t_config* config) {

	IP_DISCORDIADOR = config_get_string_value(config, "IP_DISCORDIADOR");
	PUERTO_DISCORDIADOR = config_get_string_value(config, "PUERTO_DISCORDIADOR");

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

	 //int a;
	 switch(operacion){

		case INICIAR_PLANIFICACION:
			// ARRANCA LA PLANIFICACION DE LOS TRIPULANTES (BUSCANDO EL ALGORITMO QUE ESTA EN CONFIG)
			//sem_getvalue(planificacion_on,&valor_semaforo);

			//if(valor_semaforo == 0){
			log_info(logger_on, "Iniciando Planificacion.......");
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

			sem_wait(mutex_io);
			esperando_bloqueado = 0;
			sem_post(mutex_io);


			sem_post(planificacion_on);
			sem_post(planificacion_on_ready_running);
			sem_post(planificion_rafaga);
			sem_post(planificacion_on_io);
			sem_post(termino_operacion);
			break;

		case PAUSAR_PLANIFICACION:

			log_info(logger_on, "Pausando Planificacion........");
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

			sem_wait(mutex_io);
			esperando_bloqueado = 1;
			sem_post(mutex_io);

			sem_post(termino_operacion);
			break;

		case INICIAR_PATOTA:
			//TODO
			// Ej: INICIAR_PATOTA 5 /home/utnso/tareas/tareasPatota5.txt 1|1 5|5 1|1 2|0
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/tareasPatota5.txt 5|5 5|5 5|5
			// Ej: INICIAR_PATOTA 1 /home/utnso/tareas/tareasPatota5.txt 1|1
			// Ej: INICIAR_PATOTA 2 /home/utnso/tareas/tareasPatota1.txt 5|5 5|5
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/tareasPatota1.txt 7|1
			// Ej: INICIAR_PATOTA 1 /home/utnso/tareas/tareasPatota1.txt 7|1

			//	ESTABILIDAD GENERAL:
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/ES3_Patota1.txt 9|9 0|0 5|5
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/ES3_Patota2.txt 4|0 2|6 8|2
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/ES3_Patota3.txt 2|3 5|8 5|3
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/ES3_Patota4.txt 0|9 4|4 9|0
			// Ej: INICIAR_PATOTA 3 /home/utnso/tareas/ES3_Patota5.txt 0|2 9|6 3|5

			// PRUEBAS PARA DISCORDIADOR
			//	- USO cpu
			// INICIAR_PATOTA 2 /home/utnso/tareas/CPU_Patota1.txt 1|1
			// INICIAR_PATOTA 1 /home/utnso/tareas/CPU_Patota2.txt 1|0
			// INICIAR_PATOTA 1 /home/utnso/tareas/CPU_Patota3.txt 0|1
			// 	- I/O
			// INICIAR_PATOTA 2 /home/utnso/tareas/IO_Patota1.txt 1|1
			// INICIAR_PATOTA 1 /home/utnso/tareas/IO_Patota2.txt 1|0
			// INICIAR_PATOTA 1 /home/utnso/tareas/IO_Patota3.txt 0|1

			// PRUEBAS PARA MI RAM
			// 	- PRUEBAS PARA SEGMENTACION
			// Ej: INICIAR_PATOTA 4 /home/utnso/tareas/SEG_PatotaA.txt
			// Ej: INICIAR_PATOTA 2 /home/utnso/tareas/SEG_PatotaB.txt
			// Ej: INICIAR_PATOTA 1 /home/utnso/tareas/SEG_PatotaC.txt
			// Ej: INICIAR_PATOTA 10 /home/utnso/tareas/espartana.txt
			// Ej: INICIAR_PATOTA 6 /home/utnso/tareas/persa.txt

			// 	- PRUEBAS PARA PAGINACIÓN
			// Ej: INICIAR_PATOTA 1 /home/utnso/tareas/PAG_PatotaA.txt 1|1
			// Ej: INICIAR_PATOTA 1 /home/utnso/tareas/PAG_PatotaB.txt 3|3
			// Ej: INICIAR_PATOTA 1 /home/utnso/tareas/PAG_PatotaC.txt 5|5


			if(parser_consola[1] == NULL || parser_consola[2] == NULL){
				log_error(logger_on, "Faltan argumentos. Debe iniciarse de la forma: INICIAR_PATOTA <CantidadTripulantes> >Ubicación txt Tareas>.");
				break;
			}

			uint32_t cantidad_argumentos = cantidad_argumentos_ingresados(parser_consola);
			uint32_t cantidad_posiciones = cantidad_argumentos - 3;
			int cantidad_tripulantes = atoi(parser_consola[1]);
			int posiciones_faltantes = cantidad_tripulantes - cantidad_posiciones;
			respuesta_iniciar_patota = malloc(sizeof(t_respuesta_iniciar_patota));

			if(posiciones_faltantes < 0) {
				log_error(logger_on, "Se ingresaron posiciones demás. Solo puede como máximo haber tantas posiciones como cantidad de tripulantes.\n");
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
				log_error(logger_on, "¡Error! No se ha podido conectar a %s. \n", "Mi-RAM HQ");
				break;
			}


			// Obtiene el contenido del Archivo de Tareas de la Patota
			FILE* archivo_tareas = fopen(parser_consola[2], "r");

				if(archivo_tareas > 0) {
					log_info(logger, "El archivo existe en %s.\n", parser_consola[2]);
				}
				else {
					log_error(logger_on, "El archivo %s no existe. \n", parser_consola[2]);
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

			enviar_mensaje(mensaje_patota, INICIAR_PATOTA, conexion_mi_ram);

			if(validacion_envio(conexion_mi_ram) == 1) {
				recibir_mensaje(respuesta_iniciar_patota, RESPUESTA_INICIAR_PATOTA, conexion_mi_ram);

				if(respuesta_iniciar_patota->respuesta == 1) {

					char** parser_ids = string_split(respuesta_iniciar_patota->ids_tripu, "|");

					for(int i=0; i<mensaje_patota->cantidad_tripulantes; i++) {

						tripulante_plani* tripulante = malloc(sizeof(tripulante_plani));
						tripulante->id_tripulante = atoi(parser_ids[i]);
						tripulante->numero_patota = respuesta_iniciar_patota->numero_de_patota; //esto lo devuelve mi ram
						tripulante->estado = 'N';
						tripulante->estado_anterior = 'N';
						tripulante->expulsado = 0;
						tripulante->elegido_sabotaje = 0;
						tripulante->fui_elegido_antes = 0;
						tripulante->cantidad_realizada = 0;
						tripulante->puedo_ejecutar_io = 0;


						sem_t* sem_plani=malloc(sizeof(sem_t));
						sem_init(sem_plani,0,0);
						tripulante->sem_planificacion = sem_plani;


						sem_t* semaforo_exec = malloc(sizeof(sem_t));
						sem_init(semaforo_exec, 0, 0);
						tripulante->sem_tripu = semaforo_exec;


						sem_t* mutex_estado_tripu = malloc(sizeof(sem_t));
						sem_init(mutex_estado_tripu, 0, 1);
						tripulante->mutex_estado=mutex_estado_tripu;


						sem_t* mutex_tripu_expulsado = malloc(sizeof(sem_t));
						sem_init(mutex_tripu_expulsado, 0, 1);

						tripulante->tarea_a_realizar = malloc(sizeof(t_tarea));

						tripulante->mutex_expulsado = mutex_tripu_expulsado;

						pthread_create(&hilo_tripulante,NULL,(void*)tripulante_hilo,tripulante);
						pthread_detach(hilo_tripulante);

						list_add(lista_semaforos_tripulantes, tripulante->sem_tripu); //Creo que no se usan mas
						list_add(lista_tripulantes,tripulante);

						log_info(logger,"Se inicializó al Tripulante con ID: %d, correspondiente a la Patota %d en el estado New.", tripulante->id_tripulante, tripulante->numero_patota);

						sem_wait(mutex_new);
						queue_push(cola_new,tripulante);
						sem_post(mutex_new);

						sem_post(contador_tripulantes_en_new);
					}

				}
				else {
					log_error(logger_on, "No hay espacio para almacenar la patota con sus tripulantes. \n");			// Salgo del Switch, ya que no pudo crearse la Patota en Mi-RAM HQ
					break;
				}
			}

			log_info(logger, "Se inició la Patota %d con %d tripulantes.\n\n", respuesta_iniciar_patota->numero_de_patota, cantidad_tripulantes);
			log_info(logger_on, "Se inició la Patota %d con %d tripulantes.\n", respuesta_iniciar_patota->numero_de_patota, cantidad_tripulantes);

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
			sem_post(termino_operacion);
			break;


		case LISTAR_TRIPULANTES:

			if(parser_consola[1] != NULL) {
				log_warning(logger_on, "Sobran argumentos. Debe iniciarse de la forma LISTAR_TRIPULANTES.\n");
				break;

			}

			largo = list_size(lista_tripulantes);

			printf("-------------------------------------------------------------------------\n");
			printf("Estado de la Nave: %s \n", (char*)temporal_get_string_time("%d/%m/%y %H:%M:%S"));

			for(recorrido=0; recorrido<largo; recorrido++){
				tripulante = list_get(lista_tripulantes, recorrido);
				printf("Tripulante: %u          Patota: %u          Status: %c \n", tripulante->id_tripulante, tripulante->numero_patota, tripulante->estado);
			}
			printf( "--------------------------------------------------------------------------\n\n");
			sem_post(termino_operacion);
			break;

		case OBTENER_BITACORA:

			if(parser_consola[1] == NULL) {
				log_error(logger_on, "Faltan argumentos. Debe inciarse de la forma OBTENER_BITACORA <Id_Tripulante>.\n");
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

		 /*
		 * ACA TIENE QUE RECIBIR UN MENSAJE DE MONGO STORE PARA MOSTRAR LA BITACORA
		 * 					O
		 * MONGO STORE IMPRIME LA BITACORA
		 */

			//TODO
			//Guardar en el log la bitacora del tripulante que devuelve mongo

			//cerrar_conexion(logger,conexion_mongo_store);
			//free(id_tripulante_x_bitacora);
			//cerrar_conexion(logger,conexion_mongo_store);
			sem_post(termino_operacion);
			break;

		case EXPULSAR_TRIPULANTE:

			if(parser_consola[1] == NULL) {
				log_error(logger_on, "Faltan argumentos. Debe inciarse de la forma EXPULSAR_TRIPULANTE <Id_Tripulante>.\n");
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

			tripulante_a_expulsar = list_find(lista_tripulantes, (void*)mismo_id);

			if(tripulante_a_expulsar != NULL) {

				id_tripulante_a_expulsar->id_patota = tripulante_a_expulsar->numero_patota;

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

					log_info(logger_on, "Se expulsó al Tripulante %u.\n", id_tripulante_a_expulsar->id_tripulante);

					cerrar_conexion(logger,conexion_mi_ram);
				}

				else if(estado_anterior == 'T'){
					log_error(logger_on, "Se quiso eliminar un tripulante que ya estaba terminado.\n");
					}

				else {
					log_warning(logger_on, "No se puede eliminar un Tripulante cuando no se ha iniciado la Planificación.\n");
				}

				}else {
					log_error(logger_on, "No existe el tripulante que se desea eliminar.\n");
				}

		       sem_post(termino_operacion);

			   free(id_tripulante_a_expulsar);
			   free(respuesta_al_expulsar_tripulante);
			   break;


		case TERMINAR_PROGRAMA:

			conexion_mi_ram = crear_conexion(IP_MI_RAM, PUERTO_MI_RAM);
			if(resultado_conexion(conexion_mi_ram, logger, "Mi-RAM HQ") != -1){
				enviar_mensaje("", CERRAR_MODULO, conexion_mi_ram);
				cerrar_conexion(logger, conexion_mi_ram);
			}
			/*conexion_mongo_store = crear_conexion(IP_MONGO_STORE, PUERTO_MONGO_STORE);
	  		if(resultado_conexion(conexion_mongo_store, logger, "Mongo Store") != -1) {
	  			enviar_mensaje("", CERRAR_MODULO, conexion_mongo_store);
	 			cerrar_conexion(logger, conexion_mongo_store);
	 			}
*/
			printf("Terminando programa... \n");
			sleep(1);
			printf("-------------------------------------------------------------------------------------------------------------------------------------------------- \n");
			// Libero memoria
			free(parser_consola);
			free(cadena_ingresada);
			finalizar_semaforos();
			finalizar_semaforos_plani();
			terminar_programa_discordiador(config, logger, logger_on);
			sem_post(finalizar_programa);

			break;


		default:
			printf("No se reconoce ese comando. Por favor, ingrese un comando válido.\n");
		    sem_post(termino_operacion);
			break;
		}

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


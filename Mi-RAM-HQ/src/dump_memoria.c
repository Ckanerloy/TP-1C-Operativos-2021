#include "dump_memoria.h"

void iniciar_archivo_dump(void) {

	char* path_dump = (char*)temporal_get_string_time("Dump_%y%m%d%H%M%S.dmp");
	strcat(path_dump, "\0");

	dump_memoria = malloc(sizeof(archivo_dump));

	dump_memoria->tamanio_path = strlen(path_dump);
	dump_memoria->path_dump = malloc(dump_memoria->tamanio_path+1);
	strcpy(dump_memoria->path_dump, path_dump);

	free(path_dump);
	dump_memoria->sem_dump = malloc(sizeof(sem_t));
	sem_init(dump_memoria->sem_dump, 0, 1);
}



void iniciar_dump_memoria(void) {

	log_info(logger, "Se solicitó dump de memoria.");

	FILE* archivo = fopen(dump_memoria->path_dump, "w");

	fwrite("Dump: ", strlen("Dump: "), 1, archivo);
	char* tiempo = (char*)temporal_get_string_time("%d/%m/%y %H:%M:%S");
	char* tiempo_escrito = string_new();
	string_append_with_format(&tiempo_escrito, "%s\n", tiempo);

	fwrite(tiempo_escrito, strlen(tiempo_escrito), 1, archivo);

	free(tiempo);
	free(tiempo_escrito);

	fclose(archivo);

	if(esquema_elegido == 'S') {
		registrar_dump_segmentacion();
	}
	else if(esquema_elegido == 'P') {
		registrar_dump_paginacion();
	}
	else {
		log_error(logger, "No hay ningún esquema de memoria elegido.\n");
	}
}


void registrar_dump_segmentacion(void) {

	t_list* patotas_dump = list_create();
	patotas_dump = list_duplicate(tablas_segmentos);
	list_sort(patotas_dump, menor_a_mayor_por_pid);


	t_list* segmentos_totales = list_duplicate(segmentos);
	list_sort(segmentos_totales, menor_a_mayor_por_segmento);

	for(int j=0; j<list_size(segmentos_totales); j++) {

		t_segmento* segmento_a_mostrar = (t_segmento*) list_get(segmentos_totales, j);

		bool esta_en_patota(void* segmento) {
			return ((t_segmento*)segmento)->numero_de_segmento == segmento_a_mostrar->numero_de_segmento;
		}

		bool buscando_en_patota(void* patota) {
			t_list* segmentos_patota = ((t_tabla_segmentos_patota*)patota)->segmentos;
			return list_any_satisfy(segmentos_patota, esta_en_patota);
		}

		t_tabla_segmentos_patota* patota_encontrada = list_find(patotas_dump, buscando_en_patota);

		if(patota_encontrada != NULL) {
			char* buffer = string_new();
			string_append_with_format(&buffer, "Proceso: %u     ", patota_encontrada->patota->pid);
			string_append_with_format(&buffer, "Segmento: %u     ", segmento_a_mostrar->numero_de_segmento);
			string_append_with_format(&buffer, "Inicio: %06p     ", segmento_a_mostrar->inicio);
			string_append_with_format(&buffer, "Tam: %ub\n", segmento_a_mostrar->tamanio_segmento);

			escribir_en_archivo(buffer);
		}
		else {
			char* buffer = string_new();
			string_append_with_format(&buffer, "Proceso: -     ");
			string_append_with_format(&buffer, "Segmento: %u     ", segmento_a_mostrar->numero_de_segmento);
			string_append_with_format(&buffer, "Inicio: %06p     ", segmento_a_mostrar->inicio);
			string_append_with_format(&buffer, "Tam: %ub\n", segmento_a_mostrar->tamanio_segmento);

			escribir_en_archivo(buffer);
		}
	}
}




void registrar_dump_paginacion(void) {

	for(int i=0; i<cantidad_frames; i++) {

		char* buffer = string_new();
		string_append_with_format(&buffer, "Marco: %u     ", i);

		if(frames[i]->estado == OCUPADO) {
			string_append_with_format(&buffer, "Estado: Ocupado     ");
			string_append_with_format(&buffer, "Proceso: %u     ", frames[i]->proceso);
			string_append_with_format(&buffer, "Pagina: %u\n", frames[i]->pagina);
		}
		else if (frames[i]->estado == LIBRE){
			string_append_with_format(&buffer, "Estado: Libre     ");
			string_append_with_format(&buffer, "Proceso: -     ");
			string_append_with_format(&buffer, "Pagina: -\n");
		}
		escribir_en_archivo(buffer);
	}
}


void escribir_en_archivo(char* buffer){
	sem_wait(dump_memoria->sem_dump);
	FILE* archivo = fopen(dump_memoria->path_dump, "r+");

	fseek(archivo, 0, SEEK_END);

	fwrite(buffer, strlen(buffer), 1, archivo);

	fclose(archivo);

	sem_post(dump_memoria->sem_dump);
	free(buffer);
}

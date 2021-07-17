#include "paginacion.h"


int32_t cantidad_paginas_usadas(int32_t tamanio) {

    int32_t cantidad_paginas = 0;

    while((cantidad_paginas * TAMANIO_PAGINA) < tamanio){
    	cantidad_paginas++;
    }

    return cantidad_paginas;
}


void inicializar_frames(void) {
	cantidad_frames = TAMANIO_MEMORIA / TAMANIO_PAGINA;
	log_info(logger, "Se inician %u frames en Memoria Principal.", cantidad_frames);
	frames = malloc(sizeof(frame) * cantidad_frames);
	for(int i=0; i<cantidad_frames; i++) {
		frames[i]->estado = LIBRE;
	}
}

t_tabla_paginas_patota* crear_tabla_paginas(t_pcb* nueva_patota) {

	t_tabla_paginas_patota* tabla = malloc(sizeof(t_tabla_paginas_patota));
	tabla->paginas = list_create();
	tabla->direccion_tripulantes = list_create();
	return tabla;
}

/*
void administrar_guardar_pagina(void* estructura, uint32_t tamanio, t_tabla_paginas_patota* tabla_patota) {


}*/

void administrar_guardar_patota(t_tabla_paginas_patota* tabla_patota, int32_t tamanio_total, tareas_patota* tareas_de_la_patota, int32_t cantidad_tripulantes) {

	int32_t cantidad_paginas = cantidad_paginas_usadas(tamanio_total);

	for(int i=0; i<cantidad_paginas; i++) {
		t_pagina* pagina = malloc(sizeof(t_pagina));
		pagina->numero_de_pagina = contador_pagina;
		pagina->P = 0;
		pagina->numero_de_frame = obtener_frame_disponible();
		pagina->U = 0;

		list_add_in_index(tabla_patota->paginas, i, pagina);

		contador_pagina++;
	}

	/*
	 * Cuando se asigna un Frame => se guarda en memoria
	 *
	 */

	// Creo PCB
	t_pcb* nueva_patota = crear_pcb();

	uint32_t desplazamiento = 0;
	int32_t pagina_actual = 0; 		// if(desplazamiento == TAMANIO_PAGINA) -> pagina_actual++;
/*
	tabla_patota->direccion_patota = pagina_actual * TAMANIO_PAGINA + desplazamiento;

	memcpy(memoria_principal + desplazamiento, &(nueva_patota->pid), sizeof(nueva_patota->pid));
	desplazamiento += sizeof(nueva_patota->pid);

	memcpy(memoria_principal + desplazamiento, &(nueva_patota->tareas), sizeof(nueva_patota->tareas));
	desplazamiento += sizeof(nueva_patota->tareas);


	tabla_patota->direccion_tareas = pagina_actual * TAMANIO_PAGINA + desplazamiento;

	memcpy(memoria_principal + desplazamiento, tareas_de_la_patota->tareas, tareas_de_la_patota->tamanio_tareas);
	desplazamiento += tareas_de_la_patota->tamanio_tareas;
*/





}


int32_t obtener_frame_disponible(void) {

	frame* frame_buscado;
	int32_t num_frame;

	bool _frame_libre(void* frame_buscado) {
		return ((frame*)frame_buscado)->estado == LIBRE;
	}

	if(list_any_satisfy(frames, _frame_libre)) {
		frame_buscado = list_find(frames, _frame_libre);
		num_frame = obtener_indice(frames, frame_buscado);
		return num_frame;
	}
	else {
		//APLICAR ALGORITMOS DE REEMPLAZO (LRU o CLOCK)
		//PARA OBTENER UN FRAME PARA GUARDAR (LO DEMAS SE GUARDA EN SWAP)
		return NULL;
	}
}



/*
 * 1) Hay alguna forma de validar si se puede guardar una patota con sus tripulantes?
 * 		- Se debe a que si no hay suficiente memoria, ¿podría guardarlo en la memoria SWAP?
 *
 * 2) Suponiendo que valida el tamaño:
 * 		- En un Frame guardaria un PCB y una parte de las tareas, y el sig Frame guardo el resto de las tareas
 *
 * 	Ejemplo: Tamaño Pagina: 32 bytes
 * 		Patota con 3 tripulantes y 2 tareas; cada Tarea = 20 bytes, cada Tripulante = 21 bytes y cada Pcb = 8 bytes
 * 		Tamaño Total de Patota = 111 bytes
 *
 * 		Utilizaria (aprox) 3,5 paginas
 *
 * 		Por lo tanto, utilizaria 3 frames completos y la mitad de otro para guardar una patota completa.
 *
 */






// FUNCIONES PARA ORDENAR O USAR EN LISTAS
bool menor_a_mayor_por_frame(void* pagina, void* pagina_siguiente) {
	return ((t_pagina*)pagina)->numero_de_frame < ((t_pagina*)pagina_siguiente)->numero_de_frame;
}

#include "paginacion.h"


t_tabla_paginas_patota* crear_tabla_paginas(t_pcb* nueva_patota) {

	t_tabla_paginas_patota* tabla = malloc(sizeof(t_tabla_paginas_patota));
	tabla->patota = malloc(sizeof(t_pcb));
	tabla->patota = nueva_patota;
	tabla->paginas = list_create();
	return tabla;
}


t_pagina* administrar_guardar_pagina(void* estructura, tipo_estructura tipo_pagina, uint32_t tamanio) {

	/*
	 * 1) Obtener la primer pagina que tenga algo de memoria libre
	 *
	 *
	 */
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

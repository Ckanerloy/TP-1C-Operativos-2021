#ifndef PAGINACION_H_
#define PAGINACION_H_

#include "Mi-RAM.h"
#include "utils/estructuras.h"
#include "swap.h"


typedef struct pagina {
	int32_t numero_de_pagina;
	int32_t numero_de_frame;
	estado estado_pagina;
	tipo_estructura tipo_pagina;	// Si es una pagina de tipo PATOTA, TAREAS, TRIPULANTE
	int32_t M;						// Bit de Modificado
	int32_t U;						// Bit de Uso
	int32_t P;						// Bit de Presencia
} t_pagina;


typedef struct tabla_paginas_patota {
	t_pcb* patota;
	t_list* paginas;
} t_tabla_paginas_patota;


t_list* tablas_paginas;


uint32_t cantidad_paginas;
uint32_t cantidad_frames;



bool menor_a_mayor_por_frame(void* pagina, void* pagina_siguiente);

#endif /* PAGINACION_H_ */

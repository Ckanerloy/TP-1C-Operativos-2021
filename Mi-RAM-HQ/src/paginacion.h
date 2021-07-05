#ifndef PAGINACION_H_
#define PAGINACION_H_

#include "Mi-RAM.h"
#include "utils/estructuras.h"
#include "swap.h"


typedef struct pagina {
	uint32_t numero_de_pagina;
	int32_t numero_de_frame;
	estado estado_pagina;
	int32_t M;					// Bit de Modificado
	int32_t U;					// Bit de Uso
	int32_t P;					// Bit de Presencia
} t_pagina;


typedef struct tabla_paginas_patota {
	t_pcb* patota;
	t_list* paginas;
} t_tabla_paginas_patota;



t_list* tablas_paginas;



#endif /* PAGINACION_H_ */

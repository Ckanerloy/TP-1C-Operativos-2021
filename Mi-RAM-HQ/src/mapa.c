/*
#include "mapa.h"

#include <stdlib.h>
#include <curses.h>
#include <commons/collections/list.h>


void iniciar_mapa(void) {

	nivel_gui_inicializar();

	nivel_gui_get_area_nivel(&columnas, &filas);

	amongOs = nivel_crear("A-MongOs");

	caracter_personaje = 65;	// Comienza en la letra A
}


void iniciar_tripulante(NIVEL* nivel, int caracter_personaje, int posicion_x, int posicion_y) {
	personaje_crear(nivel, caracter_personaje, posicion_x, posicion_y);
	nivel_gui_dibujar(nivel);
}


void actualizar_posicion_tripulante(NIVEL* nivel, int caracter_personaje, int posicion_x, int posicion_y) {
	item_mover(nivel, caracter_personaje, posicion_x, posicion_y);
	nivel_gui_dibujar(nivel);
}

void eliminar_tripulante(NIVEL* nivel, int caracter_personaje) {
	item_borrar(nivel, caracter_personaje);
	nivel_gui_dibujar(nivel);
}

void finalizar_mapa(NIVEL* nivel) {
	nivel_destruir(nivel);
	nivel_gui_terminar();
}
*/

#include <stdio.h>
#include <stdlib.h>

#include "Nodo.h"

Nodo *nuevoNodo(char *mensaje){
	Nodo *nuevo = (Nodo *) malloc(sizeof(Nodo));
	nuevo->id = 0;
	nuevo->mensaje = mensaje;
	nuevo->siguiente = NULL;
	return nuevo;
}
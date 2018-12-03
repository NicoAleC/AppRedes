#include <stdio.h>
#include <stdlib.h>

#include "Nodo.h"

Nodo *nuevoNodo(int id, char *mensaje){
	Nodo *nuevo = (Nodo *) malloc(sizeof(Nodo));
	nuevo->id = id;
	nuevo->mensaje = mensaje;
	nuevo->siguiente = NULL;
	return nuevo;
}
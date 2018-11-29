#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "Nodo.c"
#include "Cola.h"

#define TRUE 1
#define FALSE 0
/*
int main(){
	Cola *prueba = nuevaCola();

	Nodo *nodo1 = (Nodo*) malloc(sizeof(Nodo));
	nodo1->id = 1;
	nodo1->mensaje = "hola putos";
	nodo1->siguiente = NULL;
	esVacio(prueba);
	
	agregar(nodo1, prueba);
	esVacio(prueba);

	Nodo *nodo2 = (Nodo*) malloc(sizeof(Nodo));
	nodo2->id = 2;
	nodo2->mensaje = "kyu";
	nodo2->siguiente = NULL;
	
	agregar(nodo2, prueba);

	tomar(prueba);
	tomar(prueba);
	esVacio(prueba);

}*/

Cola *nuevaCola(){

	Cola *cola = (Cola*) malloc(sizeof(Cola));
	cola->primero = NULL;
	cola->ultimo = NULL;
	printf("Nueva cola creada\n");
	return cola;
}

int agregar(Nodo *nuevo,Cola *cola){

	if(cola == NULL || nuevo == NULL){
		errno = EBADMSG;
		perror("nodo o cola nulos");
		exit(0);
	}

	if(cola->primero == NULL){
		cola->primero = nuevo;
		cola->ultimo = nuevo;
		nuevo->siguiente = NULL;
	} else {
		cola->ultimo->siguiente = nuevo;
		cola->ultimo = nuevo;
	}

	printf("Elemento agregado a la cola\n");

	if(cola->ultimo->id == nuevo->id){
		return TRUE;
	} else {
		return FALSE;
	}
}

Nodo *tomar(Cola *cola){
	Nodo *salida = (Nodo*) malloc(sizeof(Nodo));

	if(cola->primero == NULL){
		errno = EBADMSG;
		perror("nodo o cola nulos");
		exit(0);
	} else if(cola->primero == cola->ultimo){

		salida = cola->ultimo;

		cola->primero = NULL;
		cola->ultimo = NULL;
		printf("Elemento tomado de una cola con un solo Elemento\n");
		return salida;
	} else {
		salida = cola->primero;

		cola->primero = salida->siguiente;

		printf("Elemento tomado de una cola\n");
		return salida;

	}
}

int esVacio (Cola *cola){
	if(cola->primero == NULL || cola->ultimo == NULL){
		printf("Cola vacía\n");
		return TRUE;
	} else {
		printf("Cola no vacía\n");
		return FALSE;
	}
}
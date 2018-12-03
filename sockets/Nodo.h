typedef struct Nodo{
	int id;
	char *mensaje;
	struct Nodo *siguiente;
} Nodo;

Nodo *nuevoNodo(char *mensaje);
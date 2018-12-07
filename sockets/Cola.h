typedef struct Cola{
	Nodo *primero;
	Nodo *ultimo;
	int largo;
} Cola;

int agregar(Nodo * ,Cola *);

Cola *nuevaCola();

Nodo *tomar(Cola *);

int esVacio(Cola *);
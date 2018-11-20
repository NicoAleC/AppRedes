#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define TRUE 1;
#define FALSE 0;
#define ERROR -1;

unsigned int ReadDir(char dirccion[]);
int esNumero(char numero[]);

int main(void) {

	char ip_entrada[16];
	fprintf(stderr, "ingresar una ip: \n");
	scanf("%s", ip_entrada);

	unsigned int ip = esNumero(ip_entrada);

	fprintf(stderr, "longitud de IP : %d\n", strlen(ip_entrada));

	fprintf(stderr, "esta es la ip ingresada (%s) y esta es la transformada (%d) \n", ip_entrada, ip);

   return 0;
}

unsigned int readDir(char direccion[]){

	unsigned int aux[4];
	char dir[16] = "";

	if(strlen(direccion) <= 16){
		
		for(int i = 0; i < 16; i++){
			dir[i] = direccion[i];
		}

		for(int i = 0; i < 4; i++){
			aux[i] = esNumero(direccion);
			dir = strchr(dir, '.');
		}
	} else {
		error("La direccion escrita es más larga de lo que debería");
		return FALSE;
	}

	return aux;
}

int esNumero(char numero[]){
	int n = atoi(numero);
	fprintf(stderr, "numero transformado: %d\n", n);
	int verificado;
	for(int i = 0; i < strlen(numero);i++){
		if(!isdigit(numero[i])){
			verificado = 0;
		}
	}

	if(verificado){
		return n;
	} else {
		error("Los datos ingresados deben ser una combinación de números y puntos");
	}

}
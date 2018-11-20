#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

unsigned int dirToInt(char dirccion[]);
int validarPunto(char direccion[]);
int validarNumero(char numero[]);

int main(void) {

	char ip_entrada[20];
	fprintf(stderr, "ingresar una ip: \n");
	scanf("%s", ip_entrada);

	dirToInt(ip_entrada);

	unsigned int ip = validarPunto(ip_entrada);



	fprintf(stderr, "esta es la ip ingresada (%s) y esta es la transformada (%d) \n", ip_entrada, ip);

   return 0;
}

unsigned int dirToInt(char direccion[]){

	unsigned int aux = 0x00000000, contador;



}

int validarPunto(char direccion[]){
	direccion = strchr(direccion, '.');
	int contador = 0;
	while (direccion != NULL){
		direccion = strchr(direccion + 1, '.');
		contador++;
	}
	printf("puntos contados %d\n", contador);
	if(contador == 3){
		return 1;
	} else {
		return 0;
	}
}

int validarNumero(char numero[]){
	int contador = 0;
	while(contador != strlen(numero)){
		if(!isdigit(numero[contador])){
			return 0;
		}
		contador ++;
	}

	return 1;

}
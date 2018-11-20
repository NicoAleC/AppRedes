#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define TRUE 1;
#define FALSE 0;
#define ERROR -1;

unsigned int readDir(char dirccion[]);
int esNumero(char numero[]);

int main(void) {

	char ip_entrada[16];
	fprintf(stderr, "ingresar una ip: \n");
	scanf("%s", ip_entrada);

	unsigned int ip = readDir(ip_entrada);

	fprintf(stderr, "esta es la ip ingresada (%s) y esta es la transformada (%d) \n", ip_entrada, ip);

   return 0;
}

unsigned int readDir(char direccion[]){

	long unsigned int aux1 = 0, aux2 = 0, aux3 = 0, aux4 = 0;
	char dir[16] = "";

	fprintf(stderr, "longitud de IP : %ld y la validacion: %ld\n", strlen(direccion), strspn(direccion, "0123456789."));

	if(strlen(direccion) <= 16 && strspn(direccion, "0123456789.") == strlen(direccion)){
		sscanf(direccion, "%lu.%lu.%lu.%lu", &aux1, &aux2, &aux3, &aux4);
	} else {
		perror("La direccion escrita es más larga de lo que debería o incluye caracteres no numéricos");
		return FALSE;
	}

	if((aux1|aux2|aux3|aux4) > 255){
		perror("los valores están fuera de rango");
	}

	fprintf(stderr, "%lu.%lu.%lu.%lu\n", aux1, aux2, aux3, aux4);

	return ((((((0|aux1) << 8)|aux2) << 8)|aux3) << 8)|aux4;
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
		perror("Los datos ingresados deben ser una combinación de números y puntos");
	}

}
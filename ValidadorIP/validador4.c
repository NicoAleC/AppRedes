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

	char ip_entrada[15];
	fprintf(stderr, "ingresar una ip: \n");
	scanf("%s", ip_entrada);

	unsigned int ip = esNumero(ip_entrada);



	fprintf(stderr, "esta es la ip ingresada (%s) y esta es la transformada (%d) \n", ip_entrada, ip);

   return 0;
}

unsigned int Readdir(char direccion[]){

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
		return ERROR;
	}

}
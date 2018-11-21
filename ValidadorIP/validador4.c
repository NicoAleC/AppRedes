##include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define TRUE 1;
#define FALSE 0;
#define ERROR -1;

unsigned int readDir(char direccion[]);
unsigned int verifMask(int mascara);
int contarPuntos(char direccion[]);
void ipClass(unsigned int ip);
void gatewayValido(unsigned int ip, unsigned int mascara,unsigned int gateway);

int main(void) {

	char ip_entrada[16];
	int mascara;
	char gateway_entrada[16];
	
	printf("ingresar una ip: \n");
	scanf("%s", ip_entrada);
	printf("ingresar la máscara de subred en notación \"/24\"\n");
	scanf("%d", &mascara);
	
	printf("ingresar el gateway: \n");
	scanf("%s", gateway_entrada);

	unsigned int ip = readDir(ip_entrada);
	unsigned int mask = verifMask(mascara);
	unsigned int gtwy= readDir(gateway_entrada);
	ipClass (ip);
	gatewayValido(ip,mask,gtwy);

	fprintf(stderr, "esta es la ip ingresada (%s) y esta es la transformada (%u) \n", ip_entrada, ip);
	fprintf(stderr, "verifMask: (%u)\n", mask);

   return 0;
}

unsigned int readDir(char direccion[]){

	unsigned int aux1 = 0, aux2 = 0, aux3 = 0, aux4 = 0;

	fprintf(stderr, "longitud de IP : %ld y la validacion: %ld\n", strlen(direccion), strspn(direccion, "0123456789."));

	if(strlen(direccion) <= 16 && strspn(direccion, "0123456789.") == strlen(direccion) && contarPuntos(direccion)){
		sscanf(direccion, "%u.%u.%u.%u", &aux1, &aux2, &aux3, &aux4);
	} else {
		perror("La direccion escrita es más larga de lo que debería, incluye caracteres no numéricos o está incompleta");
		return FALSE;
	}

	if((aux1|aux2|aux3|aux4) > 255){
		perror("los valores están fuera de rango");
	}

	fprintf(stderr, "la direccion verificada: %u.%u.%u.%u\n", aux1, aux2, aux3, aux4);
	/*
	fprintf(stderr, "%u\n", (0|aux1));
	fprintf(stderr, "%u\n", (0|aux1)<<8);
	fprintf(stderr, "%u\n", ((0|aux1)<<8)|aux2);
	fprintf(stderr, "%u\n", (((0|aux1)<<8)|aux2)<<8);
	fprintf(stderr, "%u\n", ((((0|aux1)<<8)|aux2)<<8)|aux3);
	fprintf(stderr, "%u\n", (((((0|aux1)<<8)|aux2)<<8)|aux3)<<8);
	fprintf(stderr, "%u\n", ((((((0|aux1)<<8)|aux2)<<8)|aux3)<<8)|aux4);*/

	return ((((((0|aux1) << 8)|aux2) << 8)|aux3) << 8)|aux4;
}

unsigned int verifMask(int mascara){
	unsigned int aux;
	
	if(mascara > 0 && mascara < 30){
		for(int i = 0;i < mascara; i++){
			aux = ((1|aux)<<1)|1;
		}

		for(int i = 0; i < (32-mascara); i++){
			aux = aux<<1;
		}
	} else {
		perror("La máscara de subred debe estar entre 1 y 32");
	}

	return aux;
}

int contarPuntos(char in[]){

   in = strchr(in,'.');
   int i = 0;
   while (in != NULL){
      in = strchr(in+1,'.');
      i++;   
   }
   if (i == 3){
      return 1;
   }else {
      return 0;
   }
}

void ipClass(unsigned int ip)
{
    if ((ip & 0x80000000)==0){
        puts("Clase A");
    } else if((ip & 0xC0000000)==0x80000000)
    {
        puts("Clase B");
    }else if((ip & 0xE0000000)==0xC0000000)
    {
        puts("Clase C");
    }else if((ip & 0xE1000000)==0xE0000000)
    {
        puts("Clase D");
    }
    else{
        puts("Clase E");
    }
}
void gatewayValido(unsigned int ip,unsigned int mascara,unsigned int gateway)
{
    //fprintf(stderr, "ip: %u y gw: %u", ip&mascara, gateway);
    if ((ip&mascara) == (gateway&mascara)){
        puts("Gateway Valido");
    }else{
        puts("Gateway invalido");
    }
       
}
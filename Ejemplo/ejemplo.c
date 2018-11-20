//errno
//strtol

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

unsigned int ipToInt(char *ip);  //ip es dirip
int validate0255(int in);  //in es entrada

unsigned int netMaskToInt(char *netMask);
int validate830(int in);
int validateDot(char *in);
int esNumero(char* num);   //num es numero

void error(char *msg);     // msg es mensaje
char verificarClase(unsigned int ip);
void verificarCIDR(char* mask, char clase);

int main(int argc, char *argv[]) {

   unsigned int ip = ipToInt(argv[1]), //ip es dirip
             nm = netMaskToInt(argv[2]),  //nm es mascarared
             gw = ipToInt(argv[3]),       //gw es gateway
             ni = (nm & ip),              //ni es netid
             bc = (0xffffffff^nm)|ni,     //bc es broadcast
             hi =  ip - ni,               //hi es hostid
             mh = (nm^0xffffffff)-1;      //mh es hostmaximo

   printf("net id: %u.%u.%u.%u\n", ni >> 24, (ni&0x00ff0000)>>16, (ni&0x0000ff00)>>8, (ni&0x000000ff));
   printf("max hosts: %u\n", mh);
   printf("host ip: %u.%u.%u.%u\n", ip >> 24, (ip&0x00ff0000)>>16, (ip&0x0000ff00)>>8, (ip&0x000000ff));
   printf("host id: %u\n", hi);
   printf("Class: %c\n", verificarClase(ip));
   verificarCIDR(argv[2],verificarClase(ip));
   printf("net mask: %u.%u.%u.%u\n", nm >> 24, (nm&0x00ff0000)>>16, (nm&0x0000ff00)>>8, (nm&0x000000ff));
   printf("broad cast: %u.%u.%u.%u\n", bc >> 24, (bc&0x00ff0000)>>16, (bc&0x0000ff00)>>8, (bc&0x000000ff));   
   printf("gateway: %u.%u.%u.%u ", gw >> 24, (gw&0x00ff0000)>>16, (gw&0x0000ff00)>>8, (gw&0x000000ff));
   printf(((ip&nm) == (gw&nm)) && (gw != ni) && (gw != bc)? "correcto!\n" : "incorrecto :(\n");

   return 0;

}

unsigned int ipToInt(char *ip){

   unsigned int temp = 0x00000000, 
             count = 0;

   if (validateDot(ip)){

      ip = strtok(ip,".");
      while (ip != NULL){
         if (esNumero(ip)) {
            temp += validate0255(atoi(ip));   
            count++;
            if (count < 4) {
               temp = temp << 8;
               
            }else {
               return temp;
            }
            ip = strtok(NULL,".");
         
         }else{
            error("La ip solo puede contener numeros y puntos.");
         }
         
      }
      if(count <4){
         error("IP invalida, no puede haber puntos seguidos");
      }

   }else {
      error("La ip solo debe contener 3 puntos.");
   }

}


unsigned int netMaskToInt(char *netMask){

   unsigned int temp = 0xffffffff;

   netMask = strchr(netMask,'/')+1;
   temp = temp << (32 - validate830(atoi(netMask)));

   return temp;

}

int validate0255(int in){

   if (in>-1 && in<256) {
      return in;
   }else {
      error("Los digitos de la ip deben estar entre 0 y 255.");
   }

}

int validate830(int in){

   if (in>7 && in<31) {
      return in;
   }else {
      error("La mascara debe estar entre 8 y 30.");
   }

}

int validateDot(char *in){

   in = strchr(in,'.');
   int count = 0;
   while (in != NULL){
      in = strchr(in+1,'.');
      count++;   
   }
   if (count == 3){
      return 1;
   }else {
      return 0;
   }

}

int esNumero(char* num){
   
   int i =0;
   while(i!=strlen(num)){
      if(!isdigit(num[i])){
         return 0;
      }

      i++;
   }
   return 1;
}

void error(char *msg){

   perror(msg);
   exit(2);

}

char verificarClase(unsigned int ip){

   if((ip>>31) == 0b0){
//      printf("%s\n", "Clase A");
      return 'A';
   }else if((ip>>30) == 0b10){
//      printf("%s\n", "Clase B");
      return 'B';
   }else if((ip>>29) == 0b110){
//      printf("%s\n", "Clase C");
      return 'C';
   }else if((ip>>28) == 0b1110){
//      printf("%s\n", "Clase D");
      return 'D';
   }else{
//      printf("%s\n", "Clase E");
      return 'E';
   }   
   
}

// 0 = subneting
// 1 = CIDR
// 2 = clase
// 3 = error
void verificarCIDR(char* mask, char clase){
   mask = strchr(mask,'/')+1;
   unsigned int mask1=atoi(mask);
   switch(clase){
      case 'A':
      if(mask1<8){
         error("Clase A no puede cer CIDR");
      }else if(mask1==8){
         printf("%s\n", "Mascara de clase");
      }else{
         printf("%s\n", "Mascara de Subneting");
      }
      break;
      case 'B':
      if(mask1<16){
         printf("%s\n", "Mascara de CIDR");
      }else if(mask1==16){
         printf("%s\n", "Mascara de clase");
      }else{
         printf("%s\n", "Mascara de Subneting");
      }
      break;
      case 'C':
      if(mask1<24){
         printf("%s\n", "Mascara de CIDR");
      }else if(mask1==24){
         printf("%s\n", "Mascara de clase");
      }else{
         printf("%s\n", "Mascara de Subneting");      }
      break;
      default:
      printf("%s\n", "Esta clase no posee Mascara");
      break;
   }
}
#include <stdio.h>
#include <pcap.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/ether.h>
#define LENGTH_CAPTURE 2000

void printing(){
    
    printf("Paquete capturado\n");
    //i=i+1;
}

int main(int argc, char **argv)
{
    int i=0;
    char errbuff[PCAP_ERRBUF_SIZE];
    char *dev = pcap_lookupdev(errbuff);
    pcap_t *handleloop;

    if(dev == NULL)
    {
        printf("No existen dispositivos para poder capturar datos");
        exit(EXIT_FAILURE);
    }

    else
    {
        printf("Dispositivo encontrado:%s",dev);
    }

    handleloop = pcap_open_live("enp0s3",LENGTH_CAPTURE,1,1000,errbuff);

    if(handleloop == NULL)
    {
        perror(errbuff);
        exit(EXIT_FAILURE);
    }
    
    printf("Empezando a capturar paquetes");
    pcap_loop(handleloop,10,printing,NULL);
    
}
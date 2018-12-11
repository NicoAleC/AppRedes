#include <stdio.h>
#include <pcap.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/ether.h>

#include "capturador.h"

#define LENGTH_CAPTURE 20000


int main(int argc, char **argv)
{
    char errbuff[PCAP_ERRBUF_SIZE];
    char *dev = pcap_lookupdev(errbuff);
    
    char *mascara_de_subred;
    char *ip;
    bpf_u_int32 mask_raw;
    bpf_u_int32 ip_raw;
    int lookup_code;
    struct in_addr direccion;

    pcap_t *handleloop;
    const u_char *paquete;
    struct pcap_pkthdr cabecera_paquete;

    if(dev == NULL){
        fprintf(stderr, "%s\n", errbuff);
        exit(EXIT_FAILURE);
    } else {
        printf("Dispositivo encontrado:%s",dev);
    }

    lookup_code = pcap_lookupnet(dev, &ip_raw, &mask_raw, errbuff);

    if( lookup_code == -1){
        fprintf(stderr, "%s\n", errbuff);
        exit(EXIT_FAILURE);
    }

    direccion.s_addr = ip_raw;

    strcpy(ip, inet_ntoa(direccion));

    if(ip == NULL){
        printf("direccion ip no reconocida\n");
        exit(EXIT_FAILURE);
    }

    direccion.s_addr = mask_raw;

    strcpy(mascara_de_subred, inet_ntoa(direccion));

    if(mascara_de_subred == NULL){
        printf("máscara de subred no reconocida\n");
        exit(EXIT_FAILURE);
    }

    handleloop = pcap_open_live(dev, LENGTH_CAPTURE, 1, 10000, errbuff);

    paquete = pcap_next(handleloop, &cabecera_paquete);

    if(paquete == NULL){
        printf("error al reconocer un paquete\n");
        exit(EXIT_FAILURE);
    }

    if(handleloop == NULL){
        perror(errbuff);
        exit(EXIT_FAILURE);
    }
    
    fprintf(stderr, "Empezando a capturar paquetes %s", dev);
    pcap_loop(handleloop,10,printing,NULL);
    
}
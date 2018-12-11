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
    printf("device found\n");
    char *dev = pcap_lookupdev(errbuff);
    
    char mascara_de_subred[16];
    char ip[16];
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
        printf("Dispositivo encontrado:%s\n",dev);
    }

    printf("reconociendo las direcciones ip y máscara subred\n");
    lookup_code = pcap_lookupnet(dev, &ip_raw, &mask_raw, errbuff);
    printf("reconocidas las direcciones ip y máscara subred\n");
    
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
    printf("%s\n", ip);

    direccion.s_addr = mask_raw;

    strcpy(mascara_de_subred, inet_ntoa(direccion));

    if(mascara_de_subred == NULL){
        printf("máscara de subred no reconocida\n");
        exit(EXIT_FAILURE);
    }
    printf("%s\n", mascara_de_subred);

    printf("definiendo handleloop\n");
    handleloop = pcap_open_live(dev, LENGTH_CAPTURE, 1, 10000, errbuff);

    /*paquete = pcap_next(handleloop, &cabecera_paquete);

    if(paquete == NULL){
        printf("error al reconocer un paquete\n");
        exit(EXIT_FAILURE);
    }*/

    if(handleloop == NULL){
        perror(errbuff);
        exit(EXIT_FAILURE);
    }
    
    fprintf(stderr, "Empezando a capturar paquetes %s", dev);
    pcap_loop(handleloop, 0, manejador_paquete, NULL);
    
}

void informacion_paquete(const u_char *paquete, const struct pcap_pkthdr *cabecera_paquete){
    fprintf(stderr, "largo del paquete capturado: %d\n", cabecera_paquete->caplen);
    fprintf(stderr, "largo total del paquete: %d\n", cabecera_paquete->len);
}

void manejador_paquete(u_char * args, const struct pcap_pkthdr * cabecera_paquete, const u_char * cuerpo_paquete){
    informacion_paquete(cuerpo_paquete, cabecera_paquete);
    struct ether_header * cabecera_ethernet;

    cabecera_ethernet = (struct ether_header *) cuerpo_paquete;

    if(ntohs(cabecera_ethernet->ether_type) == ETHERTYPE_IP){
        printf("IP\n");
    } else if(ntohs(cabecera_ethernet->ether_type) == ETHERTYPE_ARP) {
        printf("ARP\n");
    }
}
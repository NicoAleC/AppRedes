#include <stdio.h>
#include <pcap.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>

#include "capturador.h"

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
    
    if( lookup_code == -1){
        fprintf(stderr, "%s\n", errbuff);
        exit(EXIT_FAILURE);
    }

    reconocer_direccion(direccion, ip_raw, ip);
    printf("IP: %s\n", ip);

    reconocer_direccion(direccion, mask_raw, mascara_de_subred);
    printf("Máscara de subred: %s\n", mascara_de_subred);

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

void reconocer_direccion(struct in_addr direccion, bpf_u_int32 dir, char *stdir){
    direccion.s_addr = dir;

    strcpy(stdir, inet_ntoa(direccion));

    if(stdir == NULL){
        printf("direccion ip no reconocida\n");
        exit(EXIT_FAILURE);
    }
}

void manejador_paquete(u_char * args, const struct pcap_pkthdr * cabecera_paquete, const u_char * cuerpo_paquete){
    informacion_paquete(cuerpo_paquete, cabecera_paquete);
    struct ether_header * cabecera_ethernet;
    //imprimir_cabecera_ethernet(cabecera_ethernet);

    cabecera_ethernet = (struct ether_header *) cuerpo_paquete;

    u_short tipo = ntohs(cabecera_ethernet->ether_type);

    switch (tipo) {
        case ETHERTYPE_IP:
            fprintf(stderr, "IPv4: %x\n", tipo);
            imprimir_cabecera_ip(cuerpo_paquete);
            break;
        case ETHERTYPE_ARP:
            fprintf(stderr, "ARP: %x\n", tipo);
            break;
        case ETHERTYPE_IPV6:
            fprintf(stderr, "IPv6: %x\n", tipo);
            break;
        default:
            printf("error al analizar la capa de enlace\n");
            exit(EXIT_FAILURE);
            break;
    } 
}

void imprimir_cabecera_ethernet(struct ether_header * cabecera_ethernet){
    /*fprintf(stderr, "Mac destino: %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", cabecera_ethernet->ether_dhost[0], 
        cabecera_ethernet->ether_dhost[1], cabecera_ethernet->ether_dhost[2], cabecera_ethernet->ether_dhost[3], 
        cabecera_ethernet->ether_dhost[4], cabecera_ethernet->ether_dhost[5]);*/
    fprintf(stderr, "mac destino: %hhn \n", cabecera_ethernet->ether_dhost);

    /*fprintf(stderr, "Mac origen: %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", cabecera_ethernet->ether_shost[0], 
        cabecera_ethernet->ether_shost[1], cabecera_ethernet->ether_shost[2], cabecera_ethernet->ether_shost[3], 
        cabecera_ethernet->ether_shost[4], cabecera_ethernet->ether_shost[5]);*/
}

void imprimir_cabecera_ip(const u_char* cuerpo_paquete){
    struct in_addr destino;
    struct in_addr origen;

    u_short largo_cabecera_ip = 0;
    struct iphdr *cabecera_ip = (struct iphdr *) (cuerpo_paquete + sizeof(struct ether_header));
    largo_cabecera_ip = cabecera_ip->ihl * 4;
    unsigned int protocolo = (unsigned int) cabecera_ip->protocol;
    
    bzero(&origen, sizeof(origen));
    origen.s_addr = cabecera_ip->saddr;

    bzero(&destino, sizeof(destino));
    destino.s_addr = cabecera_ip->daddr;

    printf("----------------------------IP----------------------\n");
    fprintf(stderr, "version: %d\n", cabecera_ip->version);
    fprintf(stderr, "tamaño de la cabecera ip: %d\n", largo_cabecera_ip);
    fprintf(stderr, "protocolo encapsulado: %d\n", protocolo);
    fprintf(stderr, "dirección de origen: %s\n", inet_ntoa(origen));
    fprintf(stderr, "dirección de destino: %s\n", inet_ntoa(destino));
    printf("----------------------------IP----------------------\n");

    switch (protocolo){
        case TCP_P:
            imprimir_cabecera_tcp(cuerpo_paquete);
            break;
        case UPD_P:
            imprimir_cabecera_udp(cuerpo_paquete);
            break;
        case ICMP_P:
            imprimir_cabecera_icmp(cuerpo_paquete);
            break;
        case IGMP_P:
            imprimir_cabecera_igmp(cuerpo_paquete);
            break;
        default:
            printf("error al analizar la capa de red\n");
            exit(EXIT_FAILURE);
            break;
    }

}

void imprimir_cabecera_tcp(const u_char * cuerpo_paquete){

    u_short largo_cabecera_ip = 0;
    struct iphdr *cabecera_ip = (struct iphdr *) (cuerpo_paquete + sizeof(struct ether_header));
    largo_cabecera_ip = cabecera_ip->ihl * 4;

    struct tcphdr  * cabecera_tcp = (struct tcphdr *) (cuerpo_paquete + largo_cabecera_ip + sizeof(struct ether_header));

    int largo_cabecera_tcp = sizeof(struct ether_header) + largo_cabecera_ip + cabecera_tcp->doff * 4;

    printf("----------------------------TCP----------------------\n");
    fprintf(stderr, "Puerto de origen: %u\n", ntohs(cabecera_tcp->source));
    fprintf(stderr, "Puerto de destino: %u\n", ntohs(cabecera_tcp->dest));
    fprintf(stderr, "Número de secuencia: %d\n", ntohl(cabecera_tcp->seq));
    fprintf(stderr, "Número de secuencia de ACK: %d\n", ntohl(cabecera_tcp->ack_seq));
    fprintf(stderr, "ACK flag %d\n", (unsigned int) cabecera_tcp->ack);
    fprintf(stderr, "Tamaño de la cabecera: %d\n", largo_cabecera_tcp);
    printf("----------------------------TCP----------------------\n");
}

void imprimir_cabecera_udp(const u_char * cuerpo_paquete){
    u_short largo_cabecera_ip = 0;
    struct iphdr *cabecera_ip = (struct iphdr *) (cuerpo_paquete + sizeof(struct ether_header));
    largo_cabecera_ip = cabecera_ip->ihl * 4;

    struct udphdr *cabecera_udp = (struct udphdr *) (cuerpo_paquete + largo_cabecera_ip + sizeof(struct ether_header));

    int largo_cabecera_udp = sizeof(struct ether_header) + largo_cabecera_ip + sizeof(cabecera_udp);

    printf("----------------------------UDP----------------------\n");
    fprintf(stderr, "Puerto de origen: %u\n", ntohs(cabecera_udp->source));
    fprintf(stderr, "Puerto de destino: %u\n", ntohs(cabecera_udp->dest));
    fprintf(stderr, "Tamaño del paquete UDP: %d\n", ntohs(cabecera_udp->len));
    printf("----------------------------UDP----------------------\n");
}

void imprimir_cabecera_icmp(const u_char * cuerpo_paquete){
    u_short largo_cabecera_ip = 0;
    struct iphdr *cabecera_ip = (struct iphdr *) (cuerpo_paquete + sizeof(struct ether_header));
    largo_cabecera_ip = cabecera_ip->ihl * 4;

    struct icmphdr *cabecera_icmp = (struct icmphdr *) (cuerpo_paquete + largo_cabecera_ip + sizeof(struct ether_header));

    int largo_cabecera_icmp = sizeof(struct ether_header) + largo_cabecera_ip + sizeof (cabecera_icmp);

    printf("----------------------------ICMP----------------------\n");
    unsigned int tipo = (unsigned int) cabecera_icmp->type;
    fprintf(stderr, "Tipo: %u\n", tipo);
    if(tipo == 11){
        printf("TTL expirado\n");
    } else if (tipo == ICMP_ECHOREPLY){
        printf("ICMP echo reply\n");
    }
    fprintf(stderr, "Código: %u\n", (unsigned int) cabecera_icmp->code);
    //fprintf(stderr, "ID: %d\n", ntohs(cabecera_icmp->id));
    //fprintf(stderr, "Secuencia: %s\n", ntohs(cabecera_icmp->sequence));
    printf("----------------------------ICMP----------------------\n");
}

void imprimir_cabecera_igmp(const u_char * cuerpo_paquete){
    printf("----------------------------IGMP----------------------\n");
    printf("Paquete IGMP capturado\n");
    printf("----------------------------IGMP----------------------\n");
}
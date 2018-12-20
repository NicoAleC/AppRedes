#include <stdio.h>
#include <pcap.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <ctype.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>
#include </usr/include/netinet/ip6.h>

#include "capturador.h"

//Variables globales agregadas con motivo de análicis del tráfico de red
u_int c_eth, c_ipv4, c_ipv6, c_arp , c_tcp, c_udp, c_icmp, c_igmp, c_icmp6; 

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

    limpiar_cantidades();
    
    fprintf(stderr, "Empezando a capturar paquetes %s", dev);
    pcap_loop(handleloop, 0, manejador_paquete, NULL);
    
}

void limpiar_cantidades(){
    c_eth = 0;
    c_ipv6 = 0;
    c_ipv4 = 0;
    c_arp = 0;
    c_tcp = 0;
    c_udp = 0;
    c_icmp = 0;
    c_igmp = 0;
    c_icmp6 = 0;
}

void informacion_paquete(const u_char *paquete, const struct pcap_pkthdr *cabecera_paquete){
    fprintf(stderr, "\n\nLargo del paquete capturado: %d\n", cabecera_paquete->caplen);
    fprintf(stderr, "Largo total del paquete: %d\n", cabecera_paquete->len);
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
    imprimir_cabecera_ethernet(cuerpo_paquete);
    printf("\nCantidades de paquetes o tramas capturadas.......................\n");
    fprintf(stderr, "Ethernet: %u\n", c_eth);
    fprintf(stderr, "IPv4: %d, IPv6: %u, ARP: %u\n", c_ipv4, c_ipv6, c_arp);
    fprintf(stderr, "TCP: %u, UDP: %u, ICMP: %u, IGMP: %u, ICMP6: %u\n", c_tcp, c_udp, c_icmp, c_igmp, c_icmp6);
    fprintf(stderr, "Otros paquetes: %u\n", c_eth - (c_ipv6 + c_ipv4 + c_arp));
    printf("\nFin de las cantidades............................................\n");
}

void imprimir_cabecera_ethernet(const u_char *cuerpo_paquete){

    c_eth++;

    struct ether_header * cabecera_ethernet = (struct ether_header *) cuerpo_paquete;
    u_short tipo = ntohs(cabecera_ethernet->ether_type);
    //if(tipo == ETHERTYPE_ARP){
    fprintf(stderr, "Mac destino: %s \n", ether_ntoa((struct ether_addr *)cabecera_ethernet->ether_dhost));
    fprintf(stderr, "Mac origen: %s \n", ether_ntoa((struct ether_addr *)cabecera_ethernet->ether_shost));

    switch (tipo) {
        case ETHERTYPE_IP:
            fprintf(stderr, "IPv4: %x\n", tipo);
            imprimir_cabecera_ip(cuerpo_paquete);
            break;
        case ETHERTYPE_ARP:
            fprintf(stderr, "ARP: %x\n", tipo);
            imprimir_cabecera_arp(cuerpo_paquete);
            break;
        case ETHERTYPE_IPV6:
            fprintf(stderr, "IPv6: %x\n", tipo);
            imprimir_cabecera_ipv6(cuerpo_paquete);
            break;
        default:
            fprintf(stderr, "Otro tipo de trama ethernet: %x\n", tipo);
            break;
    } 
    //}
}

void imprimir_cabecera_ip(const u_char* cuerpo_paquete){
    
    c_ipv4++;

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

    printf("----------------------------IPv4----------------------\n");
    fprintf(stderr, "Version: %d\n", cabecera_ip->version);
    fprintf(stderr, "Tamaño de la cabecera ip: %d\n", largo_cabecera_ip);
    fprintf(stderr, "Protocolo encapsulado: %d\n", protocolo);
    fprintf(stderr, "Dirección de origen: %s\n", inet_ntoa(origen));
    fprintf(stderr, "Dirección de destino: %s\n", inet_ntoa(destino));
    printf("----------------------------IPv4----------------------\n");

    switch (protocolo){
        case IPPROTO_TCP:
            imprimir_cabecera_tcp(cuerpo_paquete);
            break;
        case IPPROTO_UDP:
            imprimir_cabecera_udp(cuerpo_paquete);
            break;
        case IPPROTO_ICMP:
            imprimir_cabecera_icmp(cuerpo_paquete);
            break;
        case IPPROTO_IGMP:
            imprimir_cabecera_igmp(cuerpo_paquete);
            break;
        default:
            printf("error al analizar la capa de red\n");
            break;
    }

}

void imprimir_cabecera_ipv6(const u_char * cuerpo_paquete){
    
    c_ipv6++;

    struct ip6_hdr *cabecera_ipv6 = (struct ip6_hdr *) (cuerpo_paquete + sizeof(struct ether_header));
    char origen[60];
    char destino[60];

    bzero(origen, sizeof(origen));
    inet_ntop(AF_INET6, &(cabecera_ipv6->ip6_src), origen, INET6_ADDRSTRLEN);

    bzero(destino, sizeof(destino));
    inet_ntop(AF_INET6, &(cabecera_ipv6->ip6_dst), destino, INET6_ADDRSTRLEN);

    int siguiente_cabecera = cabecera_ipv6->ip6_nxt;

    printf("----------------------------IPv6----------------------\n");
    fprintf(stderr, "Versión: %d\n", cabecera_ipv6->ip6_vfc);
    fprintf(stderr, "Dirección de origen: %s\n", origen);
    fprintf(stderr, "Dirección de destino: %s\n", destino);
    fprintf(stderr, "Siguiente cabecera: %d\n", siguiente_cabecera);
    printf("----------------------------IPv6----------------------\n");
    switch (siguiente_cabecera){
        case IPPROTO_TCP:
            imprimir_cabecera_tcp(cuerpo_paquete);
            break;
        case IPPROTO_UDP:
            imprimir_cabecera_udp(cuerpo_paquete);
        case IPPROTO_ICMPV6:
            imprimir_cabecera_icmp6(cuerpo_paquete);
            break;
        default:
            printf("error al analizar la capa de red \n");
            break;
    }
}

void imprimir_cabecera_arp(const u_char * cuerpo_paquete){

    c_arp++;

    struct arphdr * cabecera_arp = (struct arphdr *) cuerpo_paquete;
    u_short tipo = cabecera_arp->ar_op;

    printf("----------------------------ARP----------------------\n");
    fprintf(stderr, "Hardware Type : %u\n", cabecera_arp->ar_hrd);
    fprintf(stderr, "Hardware Length: %u\n", cabecera_arp->ar_hln);
    fprintf(stderr, "Protocol Type : %u\n", cabecera_arp->ar_pro);
    fprintf(stderr, "Protocol Length: %u\n", cabecera_arp->ar_pln);
    switch (tipo){
        case ARPOP_REQUEST:
            fprintf(stderr, "ARP Request: %u\n", tipo);
            break;
        case ARPOP_REPLY:
            fprintf(stderr, "ARP Reply: %u\n", tipo);
            break;
        default:
            fprintf(stderr, "ARPOP (%u)\n", tipo);
            break;
    }    
    printf("----------------------------ARP----------------------\n");
}

void imprimir_cabecera_tcp(const u_char * cuerpo_paquete){

    c_tcp++;

    struct ether_header * cabecera_ethernet = (struct ether_header *) cuerpo_paquete;
    u_short largo_cabecera_ip = 0;
    struct iphdr *cabecera_ip;
    struct ip6_hdr *cabecera_ipv6;

    switch (cabecera_ethernet->ether_type){
        case ETHERTYPE_IP:  
            cabecera_ip = (struct iphdr *) (cuerpo_paquete + sizeof(struct ether_header));
            largo_cabecera_ip = cabecera_ip->ihl * 4;
            break;
        case ETHERTYPE_IPV6:
            cabecera_ipv6 = (struct ip6_hdr *) (cuerpo_paquete + sizeof(struct ether_header));
            largo_cabecera_ip =  cabecera_ipv6->ip6_nxt;
            break;
    }

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
    
    c_udp++;

    struct ether_header * cabecera_ethernet = (struct ether_header *) cuerpo_paquete;
    u_short largo_cabecera_ip = 0;
    struct iphdr *cabecera_ip;
    struct ip6_hdr *cabecera_ipv6;

    switch (cabecera_ethernet->ether_type){
        case ETHERTYPE_IP:  
            cabecera_ip = (struct iphdr *) (cuerpo_paquete + sizeof(struct ether_header));
            largo_cabecera_ip = cabecera_ip->ihl * 4;
            break;
        case ETHERTYPE_IPV6:
            cabecera_ipv6 = (struct ip6_hdr *) (cuerpo_paquete + sizeof(struct ether_header));
            largo_cabecera_ip =  cabecera_ipv6->ip6_nxt;
            break;
    }

    struct udphdr *cabecera_udp = (struct udphdr *) (cuerpo_paquete + largo_cabecera_ip + sizeof(struct ether_header));

    int largo_cabecera_udp = sizeof(struct ether_header) + largo_cabecera_ip + sizeof(cabecera_udp);

    printf("----------------------------UDP----------------------\n");
    fprintf(stderr, "Puerto de origen: %u\n", ntohs(cabecera_udp->source));
    fprintf(stderr, "Puerto de destino: %u\n", ntohs(cabecera_udp->dest));
    fprintf(stderr, "Tamaño del paquete UDP: %d\n", ntohs(cabecera_udp->len));
    printf("----------------------------UDP----------------------\n");
}

void imprimir_cabecera_icmp(const u_char * cuerpo_paquete){
    
    c_icmp++;

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

void imprimir_cabecera_icmp6(const u_char * cuerpo_paquete){
    
    c_icmp6++;

    struct ip6_hdr *cabecera_ipv6 = (struct ip6_hdr *) (cuerpo_paquete + sizeof(struct ether_header));

    struct icmp6_hdr *cabecera_icmp6 = (struct icmp6_hdr *) (cuerpo_paquete + cabecera_ipv6->ip6_nxt + sizeof(struct ether_header));
    unsigned int tipo = (unsigned int) cabecera_icmp6->icmp6_type;

    printf("----------------------------ICMP6----------------------\n");
    fprintf(stderr, "Código: %u\n", cabecera_icmp6->icmp6_code);
    switch (tipo){
        case ICMP6_DST_UNREACH:
            fprintf(stderr, "Destino inaccesible: %u\n", tipo);
            break;
        case ICMP6_PACKET_TOO_BIG:
            fprintf(stderr, "Paquete demasiado grande: %u\n", tipo);
            break;
        case ICMP6_TIME_EXCEEDED:
            fprintf(stderr, "Tiempo excedido: %u\n", tipo);
            break;
        case ICMP6_PARAM_PROB:
            fprintf(stderr, "Problema del parametro: %u\n", tipo);
            break;
        default:
            fprintf(stderr, "Otros tipos de ICMP6 (%u)\n", tipo);
            break;
    }
    printf("----------------------------ICMP6----------------------\n");

}

void imprimir_cabecera_igmp(const u_char * cuerpo_paquete){
    
    c_igmp++;

    printf("----------------------------IGMP----------------------\n");
    printf("Paquete IGMP capturado\n");
    printf("----------------------------IGMP----------------------\n");
}
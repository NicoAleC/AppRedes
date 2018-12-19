#define LENGTH_CAPTURE 20000
#define TCP_P 6
#define ICMP_P 1
#define UPD_P 17
#define IGMP_P 2


void informacion_paquete(const u_char *, const struct pcap_pkthdr*);

void reconocer_direccion(struct in_addr, bpf_u_int32, char *);

void manejador_paquete(u_char *, const struct pcap_pkthdr *, const u_char *);

void imprimir_cabecera_ethernet(struct ether_header *);

void imprimir_cabecera_ip(const u_char *);

void imprimir_cabecera_tcp(const u_char *);

void imprimir_cabecera_udp(const u_char *);

void imprimir_cabecera_icmp(const u_char *);

void imprimir_cabecera_igmp(const u_char *);
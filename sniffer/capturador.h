void informacion_paquete(const u_char *, const struct pcap_pkthdr*);

void reconocer_direccion(struct in_addr, bpf_u_int32, char *);

void manejador_paquete(u_char *, const struct pcap_pkthdr *, const u_char *);
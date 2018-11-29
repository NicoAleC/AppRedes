#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "Cola.c"

#define PORT 4950
#define TAMANO 1024

void enviaratodos(int j, int i, int sockfd, int nbytes_recvd, char *recv_buf, fd_set *master);
void enviarrecibir(int i, fd_set *master, int sockfd, int fdmax);
void conexionaceptada(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr);
void solicitarconexion(int *sockfd, struct sockaddr_in *server_addr);

int main()
{
	fd_set master;
	fd_set read_fds;
	int fdmax, i;
	int sockfd= 0;
	struct sockaddr_in server_addr, client_addr;
	
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	solicitarconexion(&sockfd, &server_addr);
	FD_SET(sockfd, &master);
	
	fdmax = sockfd;
	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("error durante el select");
			exit(4);
		}
		
		for (i = 0; i <= fdmax; i++){
			if (FD_ISSET(i, &read_fds)){
				if (i == sockfd)
					conexionaceptada(&master, &fdmax, sockfd, &client_addr);
				else
					enviarrecibir(i, &master, sockfd, fdmax);
			}
		}
	}
	return 0;
}

void enviaratodos(int j, int i, int sockfd, int nbytes_recvd, char *recv_buf, fd_set *master)
{
	if (FD_ISSET(j, master)){
		if (j != sockfd && j != i) {
			if (send(j, recv_buf, nbytes_recvd, 0) == -1) {
				perror("error al enviar");
			}
		}
	}
}
		
void enviarrecibir(int i, fd_set *master, int sockfd, int fdmax)
{
	int nbytes_recvd, j;
	char recv_buf[TAMANO], buf[TAMANO];
	
	if ((nbytes_recvd = recv(i, recv_buf, TAMANO, 0)) <= 0) {
		if (nbytes_recvd == 0) {
			printf("el cliente %d salio\n", i);
		}else {
			perror("error al recibir");
		}
		close(i);
		//FD_CLR(i, master);
	}else { 
		for(j = 0; j <= fdmax; j++){
			enviaratodos(j, i, sockfd, nbytes_recvd, recv_buf, master );
		}
	}	
}
		
void conexionaceptada(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr)
{
	socklen_t addrlen;
	int newsockfd;
	
	addrlen = sizeof(struct sockaddr_in);
	if((newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen)) == -1) {
		perror("error al aceptar la conexion");
		exit(1);
	}else {
		FD_SET(newsockfd, master);
		if(newsockfd > *fdmax){
			*fdmax = newsockfd;
		}
		printf("nueva conexion del cliente %s \n",inet_ntoa(client_addr->sin_addr));
	}
}
	
void solicitarconexion(int *sockfd, struct sockaddr_in *server_addr)
{
	int yes = 1;
		
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("error al crear el Socket");
		exit(1);
	}
		
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(PORT);
	server_addr->sin_addr.s_addr = INADDR_ANY;
	memset(server_addr->sin_zero,0, sizeof server_addr->sin_zero);
		
	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("error al crear el setsockopt sadasdsa");
		exit(1);
	}
		
	if (bind(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		perror("No se pudo realizar el bind");
		exit(1);
	}

	if (listen(*sockfd, 10) == -1) {
		perror("Error al esperar conexiones");
		exit(1);
	}

	printf("\nEsperando que los clientes se conecten\n");
	fflush(stdout);
}
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT 4950
#define TAMANO 1024
#define NULO 0
//-------------------
char* getPtrNom();
int getId();
//-------------------


struct mensajes {
  int id;
  char *msg;
  struct mensajes *siguiente;
};

typedef struct mensajes Nodo;

struct cliente {
  int id;
  Nodo *final;
  Nodo *inicio;
};

typedef struct cliente client;
bool isEmpty(Nodo *final);
void enqueue(char*mensaje, client * puntero);
void dequeue(client * puntero);
void enviaratodos(int j, int i, int sockfd, int nbytes_recvd, char *recv_buf, fd_set *master);
void enviarrecibir(int i, fd_set *master, int sockfd, int fdmax, client*puntero[]);
void conexionaceptada(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr, client *puntero);
void solicitarconexion(int *sockfd, struct sockaddr_in *my_addr);

int main()
{
  //final = inicio = 0;
  client PtoClient[30];
	fd_set master;
	fd_set read_fds;
	int fdmax, i;
	int sockfd= 0;
	struct sockaddr_in my_addr, client_addr;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	solicitarconexion(&sockfd, &my_addr);
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
				if (i == sockfd){
					conexionaceptada(&master, &fdmax, sockfd, &client_addr, &PtoClient[i]);
          printf("client: %d .... %d\n",PtoClient[i].id, i );
        }
				else
					enviarrecibir(i, &master, sockfd, fdmax, PtoClient);
			}
		}
	}
	return 0;
}

void enviaratodos(int j, int i, int sockfd, int nbytes_recvd, char *recv_buf, fd_set*master)
{
	if (FD_ISSET(j, master)){
		if (j != sockfd && j != i) {

			if (send(j, recv_buf, nbytes_recvd, 0) == -1) {
				perror("error al enviar");
			}
		}
	}
}

void enviarrecibir(int i, fd_set *master, int sockfd, int fdmax, client*puntero[30])
{
	int nbytes_recvd, j;
	char recv_buf[TAMANO], buf[TAMANO];

	if ((nbytes_recvd = recv(i, recv_buf, TAMANO, 0)) <= 0) {
		if (nbytes_recvd == 0) {
			printf("el cliente %d salio\n", i-3);
		}else {
			perror("error al recibir");
		}
		close(i);
		FD_CLR(i, master);
	}else {
    printf("Recibi bien...%s\n", recv_buf);
		for(j = 0; j <= fdmax; j++){
      if (puntero[i]->id != j){
        printf("\nEmpezando a encolar...para el cliente numero %d\n",puntero[i]->id  );
        enqueue(recv_buf,puntero[i]);
      }
      else
        while (!isEmpty(puntero[i]->final)){
          dequeue(puntero[i]);
        }
			//enviaratodos(j, i, sockfd, nbytes_recvd, recv_buf, master );
		}
	}
}

void conexionaceptada(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr, client *puntero)
{
  client *nuevoCliente;
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
    printf("Socket: %d\n", newsockfd);
    nuevoCliente = (client*) malloc(sizeof(client));
    puntero = nuevoCliente;
    puntero->id = newsockfd;
    puntero->inicio = 0;
    puntero->final = 0;
    //puntero = nuevoCliente;
		printf("nueva conexion del cliente %s numero %d\n",inet_ntoa(client_addr->sin_addr),puntero->id-3);
	}
}

void solicitarconexion(int *sockfd, struct sockaddr_in *my_addr)
{
	int yes = 1;

	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("error al crear el Socket");
		exit(1);
	}

	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(4950);
	my_addr->sin_addr.s_addr = INADDR_ANY;
	memset(my_addr->sin_zero,0, sizeof my_addr->sin_zero);

	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("error al crear el setsockopt sadasdsa");
		exit(1);
	}

	if (bind(*sockfd, (struct sockaddr *)my_addr, sizeof(struct sockaddr)) == -1) {
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
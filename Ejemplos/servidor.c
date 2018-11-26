#include<stdio.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
#define MAX 80
#define PORT 43454
#define SA struct sockaddr
void func(int sockfd)
{
    char buff[MAX];
    int n;
    for(;;)
    {
    bzero(buff,MAX);
    read(sockfd,buff,sizeof(buff));
    printf("Del cliente: %s\t Para el cliente : ",buff);
    bzero(buff,MAX);
    n=0;
        while((buff[n++]=getchar())!='\n');
        write(sockfd,buff,sizeof(buff));
        if(strncmp("salir",buff,4)==0)
        {
            printf("Saliendo del servidor...\n");
            break;
        }
    }
}
int main()
{
int sockfd,connfd,len;
struct sockaddr_in servaddr,cli;
sockfd=socket(AF_INET,SOCK_STREAM,0);
if(sockfd==-1)
{
printf("Fallo el socket...\n");
exit(0);
}
else
printf("Socket creado...\n");
bzero(&servaddr,sizeof(servaddr));
servaddr.sin_family=AF_INET;
servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
servaddr.sin_port=htons(PORT);
if((bind(sockfd,(SA*)&servaddr, sizeof(servaddr)))!=0)
{
printf("Fallo en el enlace del socket...\n");
exit(0);
}
else
printf("Socket enlazado correctamente...\n");
if((listen(sockfd,5))!=0)
{
printf("Fallo en la recepcion...\n");
exit(0);
}
else
printf("Servidor escuchando..\n");
len=sizeof(cli);
connfd=accept(sockfd,(SA *)&cli,&len);
if(connfd<0)
{
printf("Servidor no acepto al cliente...\n");
exit(0);
}
else
printf("Cliente aceptado por el servidor...\n");
func(connfd);
close(sockfd);
}
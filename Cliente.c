#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO  9999
#define TAM_BUFFER 1024




int conectar(char *ip){

    int conector;

    struct sockaddr_in dir_servidor;

    conector= socket(AF_INET, SOCK_STREAM, 0);

    if(conector<0){
        perror("Error en socket");
        exit(1);

    }

    memset(&dir_servidor, '\0', sizeof(dir_servidor));
    dir_servidor.sin_family=AF_INET;
    dir_servidor.sin_addr.s_addr= inet_addr(ip);
    dir_servidor.sin_port= htons(PUERTO);

    if(connect(conector, (struct sockaddr *)& dir_servidor, sizeof(dir_servidor))<0){
        perror("Error en connect");
        exit(1);
    }

    return conector;
}





void recibir_archivo(int conector){
    int n;
    char buffer[TAM_BUFFER];

    while((n=read(conector, buffer, TAM_BUFFER-1))>0){
        buffer[n]=0;

        if(fputs(buffer, stdout)==EOF){
            printf("\n Error en fputs \n");
        }
    }

    if(n<0){
        perror("Error al leer del socket");
    }
}



int main( int argc, char *argv[]){
    int conector;

    if(argc!=2){

        printf("\n Usage: %s <ip of server> \n", argv[0]);
        return 1;
    }

    conector = conectar(argv[1]);
    recibir_archivo(conector);
    close(conector);
    return 0;
}

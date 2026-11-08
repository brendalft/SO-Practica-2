#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO  9999
#define COLA    5
#define TAM_BUFFER 1024
#define ARCHIVO "Google.html"




//lee Google.html por bloques y los envia al cliente
void transferir(int cliente){
    int df;
    int leidos;
    char buffer[TAM_BUFFER];

    df=open(ARCHIVO, O_RDONLY);

    if(df<0){
        perror("Hijo: error al abrir Google.html");
        return;
    }

    while((leidos=read(df, buffer, TAM_BUFFER))>0){
        if(write(cliente, buffer, leidos)<0){
            perror("Hijo: error al escribir en el socket");
            break;
        }

    }

    if(leidos<0){
        perror("Hijo: error al leer Google.html");
        
    }

    close(df);
}



int crear_conector(){
    int conector;
    struct sockaddr_in dir_servidor;

    conector=socket(AF_INET, SOCK_STREAM, 0);

    if(conector<0){
        perror("Error en socket");
        exit(1);
    }

    memset(&dir_servidor, '\0', sizeof(dir_servidor));

    dir_servidor.sin_family = AF_INET;
    dir_servidor.sin_addr.s_addr= htonl(INADDR_ANY);
    dir_servidor.sin_port= htons(PUERTO);

    if(bind(conector, (struct sockaddr*)& dir_servidor, sizeof(dir_servidor))<0){
        perror("Error en bind");
        exit(1);
    }

    if(listen(conector, COLA)<0){
        perror("Error en listen");
        exit(1);
    }

    return conector;

}


void atender_cliente(int conector, int cliente){
    switch(fork()){

        case -1:
            perror("Error en fork");
            close(cliente);
            break;

        case 0:
            close(conector);
            transferir(cliente);
            close(cliente);
            exit(0);

        default:
            close(cliente);
    }
}



int main(){
    int conector;
    int cliente;

    socklen_t lon;
    struct sockaddr_in dir_cliente;

    conector= crear_conector();

    signal(SIGCHLD, SIG_IGN);

    printf("Servidor escuchando en el puerto %d\n", PUERTO);

    while(1){

        lon=sizeof(dir_cliente);
        cliente=accept(conector, (struct sockaddr*)&dir_cliente, &lon);

        if(cliente>=0){

            printf("Conexion desde  %s\n", inet_ntoa(dir_cliente.sin_addr));
            atender_cliente(conector, cliente);

        }

    }
}

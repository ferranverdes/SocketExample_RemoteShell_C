#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <time.h>

void check_return(int, const char*);
void send_cmds(int);

int main() {
    struct timeval tv;
    struct sockaddr_in addr_server;
    struct hostent *ent;
    unsigned short port = 2000; // Puerto del servidor
    
    /* 1. Creación del socket */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    check_return(sockfd, "ERROR: No se ha podido abrir el socket.\n");
    
    /* Búsqueda del servidor */
    ent = gethostbyname("Ubuntu-Socket-Server");
    if( !ent ) {
	fprintf(stderr, "ERROR: No se ha encontrado el servidor.\n");
	exit(-1);
    }

    /* 5. Iniciar/Solicitar una conexión */
    addr_server.sin_family = AF_INET;
    addr_server.sin_addr.s_addr = (((struct in_addr *) ent->h_addr)->s_addr);
    addr_server.sin_port = htons(port);
    check_return( connect(sockfd, (struct sockaddr*) &addr_server, sizeof(struct sockaddr_in)),
		  "ERROR: No se ha podido conectar con el servidor.\n");
    
    /* Configurar el timeout al socket */
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*) &tv, sizeof(struct timeval));
    
    /* Envío de comandos a la shell remota */
    send_cmds(sockfd);

    /* 8. Cerrar el socket */
    check_return(close(sockfd), "ERROR: No se ha podido cerrar el socket.\n");

    printf("Final de sesión.\n");
    return 0;
}

void check_return(int num, const char* text) {
    if( num < 0 ) {
    	fprintf(stderr, text, NULL);
    	exit(-1);
    }
}

void send_cmds(int sockfd) {
    char cmd[10];
    char buffer[10];
    int ret;
    
    printf("INFO: Escribe 'exit' y luego pulse ENTER para salir.\n");
    
    while(1) {
    	printf("$ ");
    	fflush(stdout);

    	while( ( ret = read(0, cmd, sizeof(cmd)) ) > 0 ) {
    	    write(sockfd, cmd, sizeof(char) * ret);
    	    if( cmd[ret-1] == '\n') { break; }
    	}
    	if( strstr(cmd, "exit\n") == cmd) { break; }

    	while( ( ret = read(sockfd, buffer, sizeof(buffer)) ) > 0 ) {
    	    write(1, buffer, sizeof(char) * ret);
    	}
    }
}
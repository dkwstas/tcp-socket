#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LENGTH 80

//Gets a connection socket and orchestrates communication between server and client
void com (int conn) {
    char buffer[MAX_LENGTH];

    while (1) {
        bzero(buffer, MAX_LENGTH);
        printf("TX: ");
        scanf("%s", buffer);
        write(conn, buffer, MAX_LENGTH);

        bzero(buffer, MAX_LENGTH);
        if (read(conn, buffer, MAX_LENGTH) <= 0) {
            printf("Connection lost.\n");
            break;
        }
        printf("RX: %s\n", buffer);
    }
}

void free_heap (struct sockaddr_in *sock_addr, socklen_t *sock_length) {
    free(sock_addr);
    free(sock_length);
}

int main (int argc, char *argv[]) {
    int conn_status = -1, sock;
    in_addr_t ip_addr;
    struct sockaddr_in *sock_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    socklen_t *sock_length = (socklen_t *)malloc(sizeof(socklen_t));

    if (argc != 3) {
        printf("Usage ./client <IP> <PORT>\n");
        free_heap(sock_addr, sock_length);
        return(-1);
    }

    //Creating TCP socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //Defining IP
    ip_addr = inet_addr(argv[1]);
    //Setting IP, PORT, SOCKET TYPE
    sock_addr->sin_family = AF_INET;
    sock_addr->sin_addr.s_addr = ip_addr;
    sock_addr->sin_port = htons(atoi(argv[2]));

    //Accepting incoming connection
    *sock_length = sizeof(*sock_addr);
    conn_status = connect(sock, (struct sockaddr *)sock_addr, *sock_length);

    if (conn_status == -1) {
        perror(strerror(errno));
        exit(errno);
    } else {
        printf("Connected\n");
        com(sock);
    }

    shutdown(sock, SHUT_RDWR);
    close(sock);

    free_heap(sock_addr, sock_length);

    return(0);
}

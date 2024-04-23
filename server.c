#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define IP "0.0.0.0"
#define MAX_LENGTH 80

//Gets a connection socket and orchestrates communication between server and client
void com (int conn) {
    char buffer[MAX_LENGTH];

    while (1) {
        bzero(buffer, MAX_LENGTH);
        if (read(conn, buffer, MAX_LENGTH) <= 0) {
            printf("Connection lost.\n");
            break;
        }
        printf("RX: %s\nTX: ", buffer);

        bzero(buffer, MAX_LENGTH);
        scanf("%s", buffer);
        write(conn, buffer, MAX_LENGTH);
    }
}

void free_heap (struct sockaddr_in *sock_addr, struct sockaddr_in *client_addr,
        socklen_t *sock_length, socklen_t *client_length) {
    free(sock_addr);
    free(sock_length);
    free(client_addr);
    free(client_length);
}

int main (int argc, char *argv[]) {
    int bind_status = -1, listen_status = -1, sock, conn, opt_value = 1, *p_opt_value = &opt_value;
    in_addr_t ip_addr = inet_addr(IP);
    struct sockaddr_in *sock_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in)),
                       *client_addr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
    socklen_t *sock_length = (socklen_t *)malloc(sizeof(socklen_t)),
              *client_length = (socklen_t *)malloc(sizeof(socklen_t));

    if (argc != 2) {
        printf("Usage ./server <PORT>\n");
        free_heap(sock_addr, client_addr, sock_length, client_length);
        return(-1);
    }

    //Creating TCP socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Setting IP, PORT, SOCKET TYPE
    sock_addr->sin_family = AF_INET;
    sock_addr->sin_addr.s_addr = ip_addr;
    sock_addr->sin_port = htons(atoi(argv[1]));

    *sock_length = sizeof(*sock_addr);

    //Setting options to prevent address in use error after quitting
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, p_opt_value, sizeof(int));

    //Binding address
    bind_status = bind(sock, (struct sockaddr *)sock_addr, *sock_length);
    if (bind_status == -1) {
        perror(strerror(errno));
        return(errno);
    }

    //Starting listener with no queue
    listen_status = listen(sock, -1);
    if (listen_status == -1) {
        perror(strerror(errno));
        return(errno);
    }

    //Accepting incoming connection
    *client_length = sizeof(*client_addr);
    conn = accept(sock, (struct sockaddr *)client_addr, client_length);

    if (conn == -1) {
        printf("Client not accepted\n");
    } else {
        printf("Client accepted\n");
        com(conn);
    }

    shutdown(sock, SHUT_RDWR);
    close(sock);
    close(conn);

    free_heap(sock_addr, client_addr, sock_length, client_length);

    return(0);
}

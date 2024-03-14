#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SIZE 1024
#define IP "127.0.0.1"
#define PORT 8080

void *send_msg(void *arg) {
    char message[SIZE];
    int client_socket = *((int *)arg);
    while (1) {
        fgets(message, SIZE, stdin);
        if (strncmp(message, "bye", 3) == 0) {
            printf("You ended the session.\n");
            exit(0);
        }
        if (send(client_socket, message, strlen(message), 0) == -1) {
            printf("Failed to send message\n");
            exit(1);
        }
        printf("Sent: %s", message);
        printf("Server: ACK\nServer: Revert\n");
    }
}

void *recv_msg(void *arg) {
    char message[SIZE];
    int client_socket = *((int *)arg);
    while (1) {
        int len = recv(client_socket, message, SIZE, 0);
        if (len <= 0) {
            printf("Connection closed\n");
            exit(1);
        }
        message[len] = '\0';
        printf("Server: %s", message);
    }
}


int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    pthread_t sendt, recvt;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        printf("Failed to create socket\n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(IP);

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        printf("Failed to connect to server\n");
        exit(1);
    }

    pthread_create(&sendt, NULL, send_msg, &client_socket);
    pthread_create(&recvt, NULL, recv_msg, &client_socket);

    pthread_join(sendt, NULL);
    pthread_join(recvt, NULL);

    close(client_socket);

    return 0;
}

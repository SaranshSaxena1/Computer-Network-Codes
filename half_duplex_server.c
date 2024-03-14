#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define ACK "ACK"

int newsockfd;
time_t last_ack_time = 0;

void* send_ack(void* arg) {
    while (1) {
        sleep(rand() % 10 + 1); // Sleep for a random time between 1 and 5 seconds
        time_t current_time = time(NULL);
        if (difftime(current_time, last_ack_time) >= 2) {
            if (send(newsockfd, ACK, strlen(ACK), 0) != strlen(ACK)) {
                perror("Send failed");
                exit(EXIT_FAILURE);
            }
            printf("Sent ACK to client\n");
            last_ack_time = current_time;
        }
    }
    return NULL;
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    srand(time(NULL)); // Seed the random number generator

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d\n", SERVER_PORT);

    if ((newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, send_ack, NULL);

    while (1) {
        char buffer[1024] = {0};
        if (recv(newsockfd, buffer, 1024, 0) > 0) {
            printf("Received message from client: %s\n", buffer);
        }
    }

    close(newsockfd);
    close(sockfd);

    return 0;
}   

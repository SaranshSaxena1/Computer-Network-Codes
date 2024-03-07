#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define MESSAGE "hello"
#define ACK "ACK"
#define MAX_TOKENS 5

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    int tokens = MAX_TOKENS;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (tokens > 0) {
            if (send(sockfd, MESSAGE, strlen(MESSAGE), 0) != strlen(MESSAGE)) {
                perror("Send failed");
                exit(EXIT_FAILURE);
            }
            printf("Sent hello message to server\n");
            tokens--;
            printf("Tokens: %d\n", tokens);
            sleep(5); // Delay of 2 seconds
        } else {
            printf("No tokens left, waiting for ACK...\n");
        }

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 100000; // 100 ms timeout

        int activity = select(sockfd + 1, &read_fds, NULL, NULL, &tv);
        if (activity > 0 && FD_ISSET(sockfd, &read_fds)) {
            char ack_buffer[1024] = {0};
            if (recv(sockfd, ack_buffer, 1024, 0) > 0) {
                printf("Received ACK from server: %s\n", ack_buffer);
                tokens++;
                printf("Tokens: %d\n", tokens);
            }
        }
    }

    close(sockfd);

    return 0;
}
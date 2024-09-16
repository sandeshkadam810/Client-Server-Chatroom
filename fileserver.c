#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

#define SERVER_PORT 8080
#define MAX_BUFFER_SIZE 1024

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    // Bind server socket
    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Listen for connections
    listen(server_socket, 5);

    printf("Server listening on port %d...\n", SERVER_PORT);

    // Accept a connection
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
    printf("Client connected\n");

    // Receive and save the file
    char file_buffer[MAX_BUFFER_SIZE];
    FILE *file = fopen("received_file", "wb");

    if (file == NULL) {
        perror("File opening failed");
        exit(1);
    }

    ssize_t bytes_received;
    while ((bytes_received = recv(client_socket, file_buffer, sizeof(file_buffer), 0)) > 0) {
        fwrite(file_buffer, 1, bytes_received, file);
        
    }

    if (bytes_received == -1) {
        perror("Receive failed");
        exit(1);
    }

    printf("File received and saved as 'received_file'.\n");

    fclose(file);
    close(client_socket);
    close(server_socket);

    return 0;
}


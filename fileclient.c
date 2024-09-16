#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define MAX_BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create a socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    if(client_socket<0)
    printf("socket is not created\n");
    else 
    printf("socket is created succesfully\n");

    // Set up server address
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

int constatus=connect(client_socket,(struct sockaddr *) &server_addr, sizeof(server_addr));
 
 if(constatus==-1)
 printf("there was an error in the connection\n");
 else 
 printf("connection is estableshed\n"); 
 
 char filen[100];

        printf("Enter the name of the file to send (or type 'exit' to end): ");
        fgets(filen, sizeof(filen), stdin);
        filen[strlen(filen) - 1] = '\0';  // Remove the newline character


    char *filename = filen;  // Change to the path of the file you want to send
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        perror("File opening failed");
        exit(1);
    }

    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
        
    }

    if (bytes_read == -1) {
        perror("Send failed");
        exit(1);
    }

    printf("File sent successfully.\n");

    fclose(file);
    close(client_socket);

    return 0;
}


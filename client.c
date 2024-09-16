#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>

#define MAX_BUFFER_SIZE 1024
#define LENGTH 2048

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];

void str_overwrite_stdout() {
    printf("%s", "> ");
    fflush(stdout);
}

void str_trim_lf(char *arr, int length) {
    int i;
    for (i = 0; i < length; i++) { // trim \n
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}

void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

void send_msg_handler() {
    char message[LENGTH] = {};
    char buffer[LENGTH + 32] = {};

    while (1) {
        str_overwrite_stdout();
        fgets(message, LENGTH, stdin);
        str_trim_lf(message, LENGTH);

        if (strcmp(message, "exit") == 0) {
            break;
        } else if (strcmp(message, "file") == 0) {
            
            send(sockfd,"FILE", 5, 0);
            
            
            
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

    bytes_read = fread(buffer, 1, sizeof(buffer), file); 
        send(sockfd, buffer, bytes_read, 0);

    if (bytes_read == -1) {
        perror("Send failed");
        exit(1);
    }


    printf("File sent successfully.\n");

    fclose(file);
    
    
            
        } else {
            sprintf(buffer, "MSG:%s: %s", name, message);
            send(sockfd, buffer, strlen(buffer), 0);
        }

        bzero(message, LENGTH);
        bzero(buffer, LENGTH + 32);
    }
    catch_ctrl_c_and_exit(2);
}

void recv_msg_handler() {
    char message[LENGTH];

    while (1) {
        int receive = recv(sockfd, message, LENGTH, 0);
        if (receive > 0) {
            if (strncmp(message, "FILE", 5) == 0) {
                // Received a file request
                printf("%s", message);
                printf("Receiving file... \n");

                char file_buffer[MAX_BUFFER_SIZE];
                FILE *file = fopen("received_file", "wb");

                if (file == NULL) {
                    perror("File opening failed");
                    exit(1);
                }

                ssize_t bytes_received;
                bytes_received = recv(sockfd, file_buffer, sizeof(file_buffer), 0);
                fwrite(file_buffer, 1, bytes_received, file);


                if (bytes_received == -1) {
                    perror("Receive failed");
                    exit(1);
                }

                printf("File received and saved as 'received_file'.\n");

                fclose(file);
            } else if (strncmp(message, "MSG:", 4) == 0) {
                printf("%s\n", message + 4);
                str_overwrite_stdout();
            }
        } else if (receive == 0) {
            break;
        } else {
            // Handle error or -1
        }
        memset(message, 0, sizeof(message));
    }
}

// Function to verify user credentials
int verify_user(char *username, char *password) {
    char line[512]; // Adjust the size as needed
    FILE *file = fopen("user_credentials.txt", "r");

    if (file == NULL) {
        perror("User credentials file not found");
        exit(1);
    }

    while (fgets(line, sizeof(line), file)) {
        char stored_username[32];
        char stored_password[64]; // Use an appropriate size for storing hashed passwords

        if (sscanf(line, "%s %s", stored_username, stored_password) == 2) {
            if (strcmp(username, stored_username) == 0 && strcmp(password, stored_password)==0) {
                fclose(file);
                return 1; // User is authenticated
            }
        }
    }

    fclose(file);
    return 0; // User authentication failed
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *ip = "127.0.0.1";
    int port = atoi(argv[1]);

    signal(SIGINT, catch_ctrl_c_and_exit);

printf("Please enter your username: ");
        fgets(name, 32, stdin);
        str_trim_lf(name, strlen(name));

        if (strlen(name) < 2 || strlen(name) > 32) {
            printf("Username must be between 2 and 32 characters.\n");
        }

        char password[64]; // Adjust the size as needed
        printf("Please enter your password: ");
        fgets(password, sizeof(password), stdin);
        str_trim_lf(password, strlen(password));

        if (verify_user(name, password)) {
            
            printf("User Authentication Successful\n");
        } else {
            printf("Invalid username or password. Try again.\n");
            exit(1);
        }

    struct sockaddr_in server_addr;

    /* Socket settings */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    // Connect to Server
    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1) {
        printf("ERROR: connect\n");
        return EXIT_FAILURE;
    }

    // Send name
    send(sockfd, name, 32, 0);

    printf("=== WELCOME TO THE CHATROOM ===\n");

    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, (void *)send_msg_handler, NULL) != 0) {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, NULL) != 0) {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    while (1) {
        if (flag) {
            printf("\nBye\n");
            break;
        }
    }

    close(sockfd);

    return EXIT_SUCCESS;
}

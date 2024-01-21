#include <stdio.h>          // stardard I/O operations
#include <stdlib.h>         // standard lib
#include <string.h>         // string functions
#include <sys/socket.h>     // socket functions
#include <netinet/in.h>     // contains constants and structures needed for internet domain addresses
#include <pthread.h>        // thread functions for handling concurrent input and message receibing

#define SERVER_PORT 9002

int client_socket;

void *receive_messages(void *arg) {
    char received_message[256];
    while (1) {
        recv(client_socket, received_message, sizeof(received_message), 0);
        printf("Server: %s\n", received_message);
    }
}

void *send_inputs(void *arg) {
    char user_input[256];
    while (1) {
        fgets(user_input, sizeof(user_input), stdin);
        send(client_socket, user_input, strlen(user_input), 0);
    }
}

int main() {
    struct sockaddr_in server_address;
    
    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    // Create threads for receiving and sending messages
    pthread_t receive_thread, send_thread;
    if (pthread_create(&receive_thread, NULL, receive_messages, NULL) != 0 ||
        pthread_create(&send_thread, NULL, send_inputs, NULL) != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }

    // Wait for threads to finish
    pthread_join(receive_thread, NULL);
    pthread_join(send_thread, NULL);

    // Close the socket
    close(client_socket);

    return 0;
}

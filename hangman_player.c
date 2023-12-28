#include <stdio.h>          // stardard I/O operations
#include <stdlib.h>         // stardard lib
#include <sys/types.h>      // defines a collection of typedef symbols and structures
#include <sys/socket.h>     // socket functions
#include <netinet/in.h>     // contains constants and structures needed for internet domain addresses

int main()
{
    char server_message[256];

    // create a socket
    int network_socket;
    // AF_INET -> address family of IPv4
    // SOCK_STREAM, 0 -> configuration for TCP
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // connecting the socket
    int connection_status = connect(network_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if(connection_status == -1)
    {
        printf("There was an error making a connection to the remote socket\n\n");
    }

    // // receive data from the server
    // char server_response[256];
    // recv(network_socket, &server_response, sizeof(server_response), 0);

    // // print out the server's response
    // printf("The server sent the data: %s", server_response);

    while(1)
    {
        char user_input[256];
        if(fgets(user_input, sizeof(user_input), stdin) != NULL)
        {
            // printf("%s\n", user_input);
            send(network_socket, user_input, sizeof(user_input), 0);
        }

        recv(network_socket, &server_message, sizeof(server_message), 0);
        printf("%s\n", server_message);
    }

    // close the socket
    // close(network_socket);

    return 0;
}
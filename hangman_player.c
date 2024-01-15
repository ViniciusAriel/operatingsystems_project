#include <stdio.h>          // stardard I/O operations
#include <stdlib.h>         // stardard lib
#include <sys/types.h>      // defines a collection of typedef symbols and structures
#include <sys/socket.h>     // socket functions
#include <netinet/in.h>     // contains constants and structures needed for internet domain addresses
#include <string.h>         // string functions

int main()
{
    char server_message[256];
    char losing_message[256] = "Oh no, you lost!\n";
    char winning_message[256] = "Congratulations, you won!\n";

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
    else
    {
        printf("Connected to the server! Guess the secret word!\nPlease write a character and press enter to send it!\n\n");
    }

    while(1)
    {
        char user_input[256];
        if(fgets(user_input, sizeof(user_input), stdin) != NULL)
        {
            send(network_socket, user_input, sizeof(user_input), 0);
        }

        // this makes the player keep receiving messages from the server until it sends a NULL message
        // when that happens, this code goes back to wait for player input
        while(1)
        {
            recv(network_socket, &server_message, sizeof(server_message), 0);
            if(strlen(server_message) < 1)
            {
                break;
            }
            else if((strcmp(server_message, losing_message) == 0) || (strcmp(server_message, winning_message) == 0))
            {
                printf("%s\n", server_message);
                goto EXIT;
            }

            printf("%s\n", server_message);
        }
    }

EXIT:

    printf("End of Game. Closing the player\n");

    // close the socket
    close(network_socket);

    return 0;
}
#include <stdio.h>          // stardard I/O operations
#include <stdlib.h>         // stardard lib
#include <sys/types.h>      // defines a collection of typedef symbols and structures
#include <sys/socket.h>     // socket functions
#include <netinet/in.h>     // contains constants and structures needed for internet domain addresses
#include <string.h>         // string functions
#include <time.h>           // time functions for random int

#define LINE_LEN 60

int main()
{
    int lives = 6;

    char secret_word[256];
    char word_status[256] = "******";
    char winning_message[256] = "Congratulations, you won!\n";
    char losing_message[256] = "Oh no, you lost!\n";
    char invalid_input_message[256] = "Please, send only one charcter!\n";
    char letter_found_message[256] = "Letter found!\n";
    char lost_life_message[256] = "Oh no, you lost a life!\n";
    char remaining_lives_message[256] = "";
    char null_message[256] = "";

    char user_input[256];

    // picks random word from list
    srand(time(NULL));
    int random_number = rand() % 50;

    FILE *word_list = fopen("word_list.txt", "r");

    for(int i = 0; i <= 50; i++)
    {
        fgets(secret_word, LINE_LEN, word_list);

        if(i == random_number)
            break;
    }

    // create server socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to our specified IP and port
    bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));

    listen(server_socket, 5);

    int client_socket;
    client_socket = accept(server_socket, NULL, NULL);

    while(1)
    {
        recv(client_socket, &user_input, sizeof(user_input), 0);

        if(strlen(user_input) > 2)
        {
            send(client_socket, invalid_input_message, sizeof(invalid_input_message), 0);
            send(client_socket, null_message, sizeof(null_message), 0);
            continue;
        }

        if(strchr(secret_word, user_input[0]) != NULL)
        {

            for(int i = 0; i <= strlen(secret_word); i++)
            {
                if(secret_word[i] == user_input[0])
                {
                    word_status[i] = user_input[0];
                }
            }

            send(client_socket, word_status, sizeof(word_status), 0);

            if(strncmp(word_status, secret_word, 6) == 0)
            {
                send(client_socket, winning_message, sizeof(winning_message), 0);
                break;
            }

            send(client_socket, null_message, sizeof(null_message), 0);

        }
        else
        {
            send(client_socket, lost_life_message, sizeof(lost_life_message), 0);

            lives--;
            if(lives == 0)
            {
                send(client_socket, losing_message, sizeof(losing_message), 0);
                break;
            }

            send(client_socket, word_status, sizeof(word_status), 0);
            // formats string to be sent with the correct number of lives
            sprintf(remaining_lives_message, "You have %i lives remaining.\n", lives);
            send(client_socket, remaining_lives_message, sizeof(remaining_lives_message), 0);
            send(client_socket, null_message, sizeof(null_message), 0);
        }

    }

    printf("End of Game. Closing the server\n");

    // close the socket
    close(server_socket);

    // close word list
    fclose(word_list);

    return 0;
}
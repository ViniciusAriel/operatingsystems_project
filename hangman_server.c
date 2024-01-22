#include <stdio.h>      // stardard I/O operations
#include <stdlib.h>     // stardard lib
#include <sys/types.h>  // defines a collection of typedef symbols and structures
#include <sys/socket.h> // socket functions
#include <netinet/in.h> // contains constants and structures needed for internet domain addresses
#include <string.h>     // string functions
#include <time.h>       // time functions for random int
#include <pthread.h>    // thread functions for handling multiple clients

#define LINE_LEN 60

int lives = 6;

char secret_word[256];
char word_status[256] = "******";
char winning_message[256] = "Congratulations, you won!\n";
char losing_message[256] = "Oh no, game over!\n";
char invalid_input_message[256] = "Please, send only one charcter!\n";
char letter_found_message[256] = "Letter found!\n";
char lost_life_message[256] = "Oh no, you lost a life!\n";
char remaining_lives_message[256] = "";
char received_input_message[256] = "";

int server_socket;

int client_sockets[20];
int number_of_clients = 0;

void close_client_socket();
void send_message_to_all_clients(char pmessage[256]);

void *game_logic(void *pclient_socket)
{
    int client_socket = *((int *)pclient_socket);
    free(pclient_socket);

    char user_input[256];

    while (1)
    {
        memset(user_input, 0, strlen(user_input));
        recv(client_socket, &user_input, sizeof(user_input), 0);

        if (strlen(user_input) > 2)
        {
            send(client_socket, invalid_input_message, sizeof(invalid_input_message), 0);
            continue;
        }

        sprintf(received_input_message, "Received the letter: %s\n", user_input);
        send_message_to_all_clients(received_input_message);

        if (strchr(secret_word, user_input[0]) != NULL)
        {

            for (int i = 0; i <= strlen(secret_word); i++)
            {
                if (secret_word[i] == user_input[0])
                {
                    word_status[i] = user_input[0];
                }
            }

            send_message_to_all_clients(word_status);

            if (strncmp(word_status, secret_word, 6) == 0)
            {
                send_message_to_all_clients(winning_message);
                close_client_socket();
                exit(0);
            }
        }
        else
        {
            send_message_to_all_clients(lost_life_message);

            lives--;
            if (lives == 0)
            {
                send_message_to_all_clients(losing_message);
                close_client_socket();
                exit(0);
            }

            send_message_to_all_clients(word_status);
            sprintf(remaining_lives_message, "You have %i lives remaining.\n", lives);
            send_message_to_all_clients(remaining_lives_message);
        }
    }

    return NULL;
}

void close_client_socket()
{
    printf("End of Game. Closing the server\n");

    // close the socket
    close(server_socket);
}

void send_message_to_all_clients(char message[256])
{
    for (int i = 0; i < number_of_clients; i++)
    {
        send(client_sockets[i], message, sizeof(invalid_input_message), 0);
    }
}

int main()
{
    // picks random word from list
    srand(time(NULL));
    int random_number = rand() % 50;

    FILE *word_list = fopen("word_list.txt", "r");

    for (int i = 0; i <= 50; i++)
    {
        fgets(secret_word, LINE_LEN, word_list);

        if (i == random_number)
            break;
    }

    // create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    // bind the socket to our specified IP and port
    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    listen(server_socket, 5);

    int client_socket;
    while (1)
    {
        client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == -1)
        {
            printf("Error handling connection!\n");
            break;
        }

        printf("Connected!\n");

        client_sockets[number_of_clients] = client_socket;
        number_of_clients++;

        pthread_t client_thread;
        int *pclient = malloc(sizeof(int));
        *pclient = client_socket;

        pthread_create(&client_thread, NULL, game_logic, pclient);
    }

    printf("End of Game. Closing the server\n");

    // close the socket
    close(server_socket);

    // close word list
    fclose(word_list);

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define LINE_LEN 60

typedef struct
{
    int client_socket;
    int *lives;
    char *secret_word;
    char *word_status;
} ThreadArgs;

void *handle_client(void *arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    int client_socket = args->client_socket;
    int *lives = args->lives;
    char *secret_word = args->secret_word;
    char *word_status = args->word_status;

    char user_input[256];
    char winning_message[256] = "Congratulations, you won!\n";
    char losing_message[256] = "Oh no, you lost!\n";
    char invalid_input_message[256] = "Please, send only one character!\n";
    char letter_found_message[256] = "Letter found!\n";
    char lost_life_message[256] = "Oh no, you lost a life!\n";
    char remaining_lives_message[256] = "";
    char null_message[256] = "";

    while (1)
    {
        recv(client_socket, &user_input, sizeof(user_input), 0);

        if (strlen(user_input) > 2)
        {
            send(client_socket, invalid_input_message, sizeof(invalid_input_message), 0);
            send(client_socket, null_message, sizeof(null_message), 0);
            continue;
        }

        if (strchr(secret_word, user_input[0]) != NULL)
        {
            for (int i = 0; i < strlen(secret_word); i++)
            {
                if (secret_word[i] == user_input[0])
                {
                    word_status[i] = user_input[0];
                }
            }

            send(client_socket, word_status, sizeof(word_status), 0);

            if (strncmp(word_status, secret_word, strlen(secret_word) - 1) == 0)
            {
                send(client_socket, winning_message, sizeof(winning_message), 0);
                break;
            }

            send(client_socket, null_message, sizeof(null_message), 0);
        }
        else
        {
            send(client_socket, lost_life_message, sizeof(lost_life_message), 0);

            (*lives)--;
            if (*lives == 0)
            {
                send(client_socket, losing_message, sizeof(losing_message), 0);
                break;
            }

            send(client_socket, word_status, sizeof(word_status), 0);
            sprintf(remaining_lives_message, "You have %i lives remaining.\n", *lives);
            send(client_socket, remaining_lives_message, sizeof(remaining_lives_message), 0);
            send(client_socket, null_message, sizeof(null_message), 0);
        }
    }

    printf("Client disconnected. Closing the thread.\n");
    close(client_socket);
    free(args);

    return NULL;
}

int main()
{
    int lives = 6;

    char secret_word[256];
    char word_status[256] = "******";

    srand(time(NULL));
    int random_number = rand() % 50;

    FILE *word_list = fopen("word_list.txt", "r");

    for (int i = 0; i < 50; i++)
    {
        fgets(secret_word, LINE_LEN, word_list);

        if (i == random_number)
            break;
    }

    // Remove newline character from secret_word
    secret_word[strcspn(secret_word, "\n")] = '\0';

    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9002);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    listen(server_socket, 5);

    pthread_t thread_id;

    while (1)
    {
        int client_socket;
        client_socket = accept(server_socket, NULL, NULL);

        if (client_socket < 0)
        {
            perror("Error accepting client connection");
            continue;
        }

        ThreadArgs *args = (ThreadArgs *)malloc(sizeof(ThreadArgs));
        args->client_socket = client_socket;
        args->lives = &lives;
        args->secret_word = secret_word;
        args->word_status = word_status;

        if (pthread_create(&thread_id, NULL, handle_client, (void *)args) != 0)
        {
            perror("Error creating thread");
            close(client_socket);
            free(args);
        }
    }

    printf("End of Game. Closing the server\n");
    close(server_socket);
    fclose(word_list);

    return 0;
}

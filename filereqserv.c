#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define FILEREQPORT 1024
#define MAX_FILENAME_LENGTH 128
#define MAX_FILE_LENGTH 4096

typedef struct sockaddr* SocketAddress;
typedef struct sockaddr_in Address;

char *minifyString(char *data, int *length);

int main(void)
{
    // initialise our server socket
    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (server < 0)
    {
        perror("There was a problem creating our socket!");
        exit(1);
    }

    Address serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(FILEREQPORT);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind and listen to it
    if (bind(server, (SocketAddress)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("There was a problem binding our socket!");
        close(server);
        exit(1);
    }

    if (listen(server, 4) < 0)
    {
        perror("There was a problem listening to our server!");
        close(server);
        exit(1);
    }

    char filename[MAX_FILENAME_LENGTH];
    char fileContents[MAX_FILE_LENGTH];

    // format our buffers
    memset(filename, '\0', MAX_FILENAME_LENGTH);
    memset(fileContents, '\0', MAX_FILE_LENGTH);

    // accept ONE connection
    int client = accept(server, NULL, NULL);

    // receive filename request
    if (recv(client, filename, MAX_FILENAME_LENGTH, 0) < 0)
    {
        perror("There was a problem receiving our filename input!");
        close(client);
        close(server);
        exit(2);
    }

    // mini-fy our filename buffer
    char *actualFilename = minifyString(filename, NULL);

    // read file to memory
    FILE *file = fopen(actualFilename, "rb");
    printf("Client requested file \"%s\"\n", actualFilename);
    free(actualFilename);

    if (file != NULL)
    {
        fseek(file, 0, SEEK_END);
        int length = ftell(file);
        fseek(file, 0, SEEK_SET);

        printf("File length: %i\n", length);

        // read its content to the buffer
        fread(fileContents, length, 1, file);
        fclose(file);
    }
    else
    {
        // set it as our default no-file-found response
        memcpy(fileContents, "Invalid file!", 14);
    }

    // send data to the client
    if (send(client, fileContents, MAX_FILE_LENGTH, 0) < 0)
    {
        perror("Unable to send data to client!");
        close(client);
        close(server);
        exit(3);
    }

    // close our sockets
    close(client);
    close(server);
}

char *minifyString(char *data, int *length)
{
    int l;
    for (l = 1; data[l] != '\0'; ++l);

    // initialise and copy our string data
    char *newStr = malloc(sizeof(char) * l);

    if (newStr != NULL)
    {
        memset(newStr, '\0', l);
        memcpy(newStr, data, l);
    }

    if (length != NULL)
        *length = l;

    return newStr;
}


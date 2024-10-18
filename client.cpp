#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>


#define MAX_DATA_SIZE   8192
#define NETWORK_PORT    23450

const char *server_address = "127.0.0.1"; 

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: client <filename>\n");
        return -1;
    }

    // Try to open the file that's to be sent
    char *filename = argv[1];
    FILE *sendfile = fopen(filename, "rt");
    if (sendfile == NULL)
    {
        printf("Error, unable to open file %s\n", filename);
        return -1;
    }

    printf("Starting test client\n");
    
    // create a network socket
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        printf("Error creating a network socket\n");
        fclose(sendfile);
        return -1;
    } 

    struct sockaddr_in serverAddr;
    // specify address and type for socket
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(NETWORK_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(server_address);

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        printf("Error connecting to server on %s:%d\n", server_address, NETWORK_PORT);
        fclose(sendfile);
        return -1;
    }

    printf("Connected to server on %s:%d\n", server_address, NETWORK_PORT);

    int filesize = -1;
    fseek(sendfile, 0L, SEEK_END);
    filesize = ftell(sendfile);
    fseek(sendfile, 0L, SEEK_SET);

    if (filesize > 0)
    {
        char *buffer = (char *)malloc(filesize);
        if (buffer)
        {
            int bytesread = fread(buffer, 1, filesize, sendfile);
            printf("sending %d bytes from file %s\n", bytesread, filename);

            // send the data to the server
            send(clientSocket, buffer, bytesread, 0);
            free(buffer);
        }
        else
            printf("Unable to allocate %d bytes for data, nothing sent\n", filesize);
    }
    else
        printf("File is empty, nothing sent\n");


    fclose(sendfile);
    close(clientSocket);
    return 0;
}
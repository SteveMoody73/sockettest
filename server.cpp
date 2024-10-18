#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>


#define MAX_DATA_SIZE   8192
#define NETWORK_PORT    23450


int main(int argc, char **argv)
{
    printf("Starting xml listener server\n");
    
    // create a network socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        printf("Error creating a network socket\n");
        return -1;
    } 

    struct sockaddr_in serverAddr;
    // specify address and type for socket
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(NETWORK_PORT);
    serverAddr.sin_addr.s_addr = (INADDR_ANY);
    
    int bindStatus = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (bindStatus < 0)
    {
        printf("Error binding socket on port %d\n", NETWORK_PORT);
        return -1;
    }

    // listen for client connections
    int listenStatus = listen(serverSocket, 5);
    if (listenStatus < 0)
    {
        printf("Error creating the listener\n");
        return -1;
    }

    printf("Socket has been created on port %d, waiting for connections\n", NETWORK_PORT);
    char buffer[MAX_DATA_SIZE];
    memset(buffer, 0, MAX_DATA_SIZE);


    struct sockaddr_in clientAddr;
    int clientHandler;
    socklen_t len = sizeof(clientAddr);
    int connection = accept(serverSocket, (struct sockaddr *)&clientAddr, &len);
    if (connection < 0)
    {
        printf("Server did not accept request from client\n");
        close(serverSocket);
        return -1;
    }
    else
        printf("Server has accepted request from the client\n");

    int recSize = recv(connection, buffer, MAX_DATA_SIZE, 0);
    if (recSize == 0)
        printf("Error, no data received\n");
    else
    {
        printf("Received %d bytes from client\n", recSize);
        // process the XML data, for now just print it
        printf("%s\n", buffer);
    }
    
    close(serverSocket);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <pugixml.hpp>

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

        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_string(buffer);
        pugi::xml_node xml = doc.child("XML");

		if (!xml)
		{
			printf("Data is not in XML format\n");
		}
        else
        {
            if (result.status != pugi::xml_parse_status::status_ok)
            {
                printf("Error loading XML data\n");
            }
            else
            {
                for (pugi::xml_node_iterator it = xml.begin(); it != xml.end(); ++it)
                {
                    std::string nodename = it->name();
                    if (nodename.compare("Component") == 0)
                    {
                        printf("XML Node: %s ", it->name());
                        for (pugi::xml_attribute_iterator ait = it->attributes_begin(); ait != it->attributes_end(); ++ait)
                        {
                            printf("%s,%s ", ait->name(), ait->value());
                        }
                        printf("\n");
                    }
                }
            }
        }
    }
    
    close(serverSocket);
    return 0;
}
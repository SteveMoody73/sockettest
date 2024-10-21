#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include <pugixml.hpp>
#include <algorithm>


#define MAX_DATA_SIZE   8192
#define NETWORK_PORT    23450

// trim from start (in place)
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch)
    {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch)
    {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
}

bool stringToFloat(const std::string &str, float *f)
{
    char *endPtr = nullptr;
    std::string trimmed = str;
    trim(trimmed);
    float result = (float)std::strtod(trimmed.c_str(), &endPtr);
    if (*endPtr != '\0')
    {
        printf("Error converting a string to a float \"%s\"", trimmed.c_str());
        return false;
    }

    *f = result;
    return true;
}

bool stringToLong(const std::string &str, long *l)
{
    char *endPtr = nullptr;
    std::string trimmed = str;
    trim(trimmed);
    long result = std::strtol(trimmed.c_str(), &endPtr, 10);
    if (*endPtr != '\0')
    {
        printf("Error converting a string to an integer \"%s\"", trimmed.c_str());
        return false;
    }
    *l = result;
    return true;
}


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
                    std::string compname = it->attribute("name") == nullptr ? "Unknown" : it->attribute("name").value(); 
                    if (nodename.compare("Component") == 0)
                    {
                        std::string nodetype = it->child("Type") == nullptr ? "Unknown" : it->child("Type").text().get();
                        if (nodetype.compare("SolenoidValve") == 0)
                        {
                            std::string valvepos = it->child("ValvePos") == nullptr ? "0" : it->child("ValvePos").text().get();
                            long pos = 0;
                            stringToLong(valvepos, &pos);

                            printf("Valve %s set to %d\n", compname.c_str(), (int)pos);
                        }
                        else if (nodetype.compare("HeatedZone") == 0)
                        {
                            std::string targettemp = it->child("TargetTemp") == nullptr ? "0" : it->child("TargetTemp").text().get();
                            std::string heateron = it->child("TargetTemp") == nullptr ? "0" : it->child("TargetTemp").text().get();
                            float temp = 0.0f; long onoff = 0;
                            stringToFloat(targettemp, &temp);
                            stringToLong(heateron, &onoff);

                            printf("Heater %s set to  %3.2f (%s)\n", compname.c_str(), temp, onoff == 0 ? "off" : "on");
                        }
                    }
                }
            }
        }
    }
    
    close(serverSocket);
    return 0;
}
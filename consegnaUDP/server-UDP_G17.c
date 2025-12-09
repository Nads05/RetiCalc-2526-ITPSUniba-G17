#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

int main(void)
{
    int mySocket;
    struct sockaddr_in sad;
    struct sockaddr_in cad;
    int recvMessSize;
    struct hostent *host = gethostbyname("localhost");

    sad.sin_family = AF_INET;
    sad.sin_port = htons(6666);
    memcpy(&sad.sin_addr, host->h_addr_list[0], host->h_length);

    if ((mySocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("Errore creazione socket");
        return 1;
    }

    if ((bind(mySocket, (struct sockaddr *)&sad, sizeof(sad))) < 0)
    {
        perror("Errore nel binding");
        return 1;
    }

    while (1)
    {
        char buff[1024];
        unsigned int cadLength = sizeof(cad);
        recvMessSize = recvfrom(mySocket, buff, sizeof(buff) - 1, 0, (struct sockaddr *)&cad, &cadLength);
        printf("Gestendo il client %s\n", inet_ntoa(cad.sin_addr));
        buff[recvMessSize] = '\0';
        int num = 0;
        int i = 0;
        int virgole = 0;
        while (buff[i] != '\0')
        {
            if (buff[i] == ',')
                virgole++;
            i++;
        }
        char *token = strtok(buff, ",");
        if (strlen(token) != 1 || virgole != 2)
        {
            strcpy(buff, "Stringa errata, termine processo");
            sendto(mySocket, buff, sizeof(buff), 0, (struct sockaddr *)&cad, cadLength);
        }
        else
        {
            switch (*token)
            {
            case 'a':
            case 'A':
                while (token != NULL)
                {
                    token = strtok(NULL, ",");
                    if (token == NULL)
                        continue;
                    num += atoi(token);
                }
                snprintf(buff, sizeof(buff), "%d", num);
                sendto(mySocket, buff, sizeof(buff), 0, (struct sockaddr *)&cad, cadLength);
                break;
            case 's':
            case 'S':
                token = strtok(NULL, ",");
                num = atoi(token);
                token = strtok(NULL, ",");
                num -= atoi(token);
                snprintf(buff, sizeof(buff), "%d", num);
                sendto(mySocket, buff, sizeof(buff), 0, (struct sockaddr *)&cad, cadLength);
                break;
            case 'm':
            case 'M':
                num = 1;
                while (token != NULL)
                {
                    token = strtok(NULL, ",");
                    if (token == NULL)
                        continue;
                    num *= atoi(token);
                }
                snprintf(buff, sizeof(buff), "%d", num);
                sendto(mySocket, buff, sizeof(buff), 0, (struct sockaddr *)&cad, cadLength);
                break;
            case 'd':
            case 'D':
                token = strtok(NULL, ",");
                num = atoi(token);
                token = strtok(NULL, ",");
                num /= atoi(token);
                snprintf(buff, sizeof(buff), "%d", num);
                sendto(mySocket, buff, sizeof(buff), 0, (struct sockaddr *)&cad, cadLength);
                break;
            default:
                printf("Stringa Errata\n");
                break;
            }
        }
        printf("Fine gestione del client\n");
    }
}
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h> //gethost
#include <stdbool.h>

int main(void)
{
    ////crea socket: fam indirizzi ipv4, protocollo tcp per questa socket, protocollo 0: default (es. -1= errore)
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *host = gethostbyname("localhost"); // trova local host tramite gethostbyname()
    struct sockaddr_in sad;                            //*

    // controlla valore ritorno: se <0 socket non viene creata
    if (sock < 0)
    {
        perror("Socket creating error");
        return 1;
    }

    // assegno i valori ai campi della struct sockaddri_in  *. E' un continitore di valori che interessano poi alla socket per utilizzare le altre funzioni
    memcpy(&sad.sin_addr, host->h_addr_list[0], host->h_length); // copio l'indirizzo IP ottenuto con gethostbyname nella struttura sad al campo sin_addr
    sad.sin_family = AF_INET;                                    // Address familiy
    sad.sin_port = htons(6666);                                  // Imposta la porta sulla quale il socket si collegherà o ascolterà. Porta host to network short (htons) serve a convertire il decimale in binario big endian

    // assegno un local address(ip e porta) alla socket creata in precedenza
    if (bind(sock, (struct sockaddr *)&sad, sizeof(sad)) < 0)
    {
        perror("bind() fallito");
        close(sock);
        return 1;
    }

    // metto in ascolto la socket di bevenuto
    int qlen = 5;
    if (listen(sock, qlen) < 0)
    {
        perror("listen() fallito");
        close(sock);
        return 1;
    }

    // iterazioni per accettare connessioni, fare azioni e chiudere connessioni
    struct sockaddr_in cad;
    int clientSocket;
    socklen_t clientLen; // dimensione struttura, servirà per definire dimensione di cad in accept
    printf("In attesa di connessione...\n");

    while (1)
    {
        // accetto la connessione
        clientLen = sizeof(cad);
        clientSocket = accept(sock, (struct sockaddr *)&cad, &clientLen); // descrittore della socket del client
        if (clientSocket < 0)
        {
            perror("accept() fallito");
            close(sock);
            return 1;
        }
        printf("Gestendo il client %s\n", inet_ntoa(cad.sin_addr));

        // invio la stringa contenuta in buf di lunghezza stringLen al client
        char buf[1024] = "Connessione avvenuta\n";
        int stringLen = strlen(buf);
        if ((send(clientSocket, buf, stringLen, 0)) < 0)
        {
            perror("Invio fallito");
            close(clientSocket);
            continue;
        }

        // resetto il buffer per utilizzarlo dopo
        buf[0] = '\0';
        int byteRicevuti = 0;

        // ricevo la stringa 1
        if ((byteRicevuti = recv(clientSocket, buf, sizeof(buf) - 1, 0) - 1) < 0)
        {
            perror("Ricezione fallita");
            continue;
        }
        buf[byteRicevuti] = '\0';

        if (!(byteRicevuti == 1 && (buf[0] == 'a' || buf[0] == 'A' || buf[0] == 's' || buf[0] == 'S' || buf[0] == 'm' || buf[0] == 'M' || buf[0] == 'd' || buf[0] == 'D')))
        {
            strcpy(buf, "TERMINE PROCESSO CLIENT");
            if ((send(clientSocket, buf, strlen(buf), 0)) < 0)
            {
                perror("Invio fallito");
                close(clientSocket);
                continue;
            }
        }
        else
        {
            int byteRcv1 = 0;
            int byteRcv2 = 0;
            char firstOp[20];
            char secondOp[20];
            int x = 0, y = 0, z = 0;
            // Ci siamo assicurati che entra qui se l'input è corretto
            // Ora switch case: per ogni operazione riceve due numeri fa l'operazione e invia
            switch (buf[0])
            {
            case 'a':
            case 'A':
                strcpy(buf, "ADDIZIONE");
                if ((send(clientSocket, buf, strlen(buf), 0)) < 0)
                {
                    perror("Invio fallito");
                    close(clientSocket);
                    continue;
                }
                if ((byteRcv1 = recv(clientSocket, firstOp, sizeof(firstOp) - 1, 0)) < 0)
                {
                    perror("Ricezione fallita");
                    continue;
                }
                firstOp[byteRcv1] = '\0';

                if ((byteRcv2 = recv(clientSocket, secondOp, sizeof(secondOp) - 1, 0)) < 0)
                {
                    perror("Ricezione fallita");
                    continue;
                }
                secondOp[byteRcv2] = '\0';

                x = atoi(firstOp);
                y = atoi(secondOp);
                z = x + y;
                snprintf(buf, sizeof(buf), "%d", z);
                buf[strlen(buf)] = '\0';
                if ((send(clientSocket, buf, strlen(buf), 0)) < 0)
                {
                    perror("Invio fallito");
                    close(clientSocket);
                    continue;
                }
                break;
            case 's':
            case 'S':
                strcpy(buf, "SOTTRAZIONE");
                if ((send(clientSocket, buf, strlen(buf), 0)) < 0)
                {
                    perror("Invio fallito");
                    close(clientSocket);
                    continue;
                }
                if ((byteRcv1 = recv(clientSocket, firstOp, sizeof(firstOp) - 1, 0)) < 0)
                {
                    perror("Ricezione fallita");
                    continue;
                }
                firstOp[byteRcv1] = '\0';

                if ((byteRcv2 = recv(clientSocket, secondOp, sizeof(secondOp) - 1, 0)) < 0)
                {
                    perror("Ricezione fallita");
                    continue;
                }
                secondOp[byteRcv2] = '\0';

                x = atoi(firstOp);
                y = atoi(secondOp);
                z = x - y;
                snprintf(buf, sizeof(buf), "%d", z);
                buf[strlen(buf)] = '\0';
                if ((send(clientSocket, buf, strlen(buf), 0)) < 0)
                {
                    perror("Invio fallito");
                    close(clientSocket);
                    continue;
                }
                break;
            case 'm':
            case 'M':
                strcpy(buf, "MOLTIPLICAZIONE");
                if ((send(clientSocket, buf, strlen(buf), 0)) < 0)
                {
                    perror("Invio fallito");
                    close(clientSocket);
                    continue;
                }
                if ((byteRcv1 = recv(clientSocket, firstOp, sizeof(firstOp) - 1, 0)) < 0)
                {
                    perror("Ricezione fallita");
                    continue;
                }
                firstOp[byteRcv1] = '\0';

                if ((byteRcv2 = recv(clientSocket, secondOp, sizeof(secondOp) - 1, 0)) < 0)
                {
                    perror("Ricezione fallita");
                    continue;
                }
                secondOp[byteRcv2] = '\0';

                x = atoi(firstOp);
                y = atoi(secondOp);
                z = x * y;

                snprintf(buf, sizeof(buf), "%d", z);

                buf[strlen(buf)] = '\0';
                if ((send(clientSocket, buf, strlen(buf), 0)) < 0)
                {
                    perror("Invio fallito");
                    close(clientSocket);
                    continue;
                }
                break;
            case 'd':
            case 'D':
                strcpy(buf, "DIVISIONE");
                if ((send(clientSocket, buf, strlen(buf), 0)) < 0)
                {
                    perror("Invio fallito");
                    close(clientSocket);
                    continue;
                }
                if ((byteRcv1 = recv(clientSocket, firstOp, sizeof(firstOp) - 1, 0)) < 0)
                {
                    perror("Ricezione fallita");
                    continue;
                }
                firstOp[byteRcv1] = '\0';

                if ((byteRcv2 = recv(clientSocket, secondOp, sizeof(secondOp) - 1, 0)) < 0)
                {
                    perror("Ricezione fallita");
                    continue;
                }
                secondOp[byteRcv2] = '\0';

                x = atoi(firstOp);
                y = atoi(secondOp);
                z = x / y;

                snprintf(buf, sizeof(buf), "%d", z);
                buf[strlen(buf)] = '\0';
                if ((send(clientSocket, buf, strlen(buf), 0)) < 0)
                {
                    perror("Invio fallito");
                    close(clientSocket);
                    continue;
                }
                break;
            default:
                break;
            }
        }

        close(clientSocket);
        printf("Connessione con %s terminata\n", inet_ntoa(cad.sin_addr));
    }
}
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>

int main(void)
{
    int mySocket = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *host = gethostbyname("localhost");
    struct sockaddr_in sad;
    if (mySocket < 0)
    {
        perror("Errore nella creazione della socket");
        close(mySocket);
        return 1;
    }

    sad.sin_family = AF_INET;
    memcpy(&sad.sin_addr, host->h_addr_list[0], host->h_length);
    sad.sin_port = htons(6666);

    if (connect(mySocket, (struct sockaddr *)&sad, sizeof(sad)) < 0)
    {
        perror("Errore di connessione con il server");
        close(mySocket);
        return 0;
    }

    // ricevo la stringa di connessione avvenuta con rcv
    char buffer[1024];
    int byteRicevuti = 0;
    if ((byteRicevuti = recv(mySocket, buffer, sizeof(buffer) - 1, 0)) < 0)
    {
        perror("Ricezione fallita");
    }
    printf("%s", buffer);

    // leggo stringa dell'operatore
    printf("Inserisci il carattere per effettuare l'operazione (a, s, m, d): ");
    char str[100];
    fgets(str, sizeof(str), stdin);
    int strLen = strlen(str);

    // La invio
    if (send(mySocket, str, strLen, 0) != strLen)
    {
        perror("Inviati un numero di byte diversi");
        close(mySocket);
        return 0;
    }

    // ricevo l'operazione che dovrò fare
    byteRicevuti = 0;
    if ((byteRicevuti = recv(mySocket, str, sizeof(str) - 1, 0)) < 0)
    {
        perror("Errore in ricezione stringa");
    }
    if (strcmp(str, "TERMINE PROCESSO CLIENT") == 0)
    {
        close(mySocket);
        str[byteRicevuti] = '\0';
        printf("%s\n", str);
        return 1;
    }

    str[byteRicevuti] = '\0';
    printf("%s\n", str);

    // invio il primo numero
    str[0] = '\0';
    printf("Inserisci il primo operando: ");
    fgets(str, sizeof(str), stdin);
    strLen = strlen(str);
    if ((send(mySocket, str, strLen, 0)) < 0)
    {
        perror("Inviati un numero di byte diversi");
        close(mySocket);
        return 0;
    }

    // invio il secondo numero
    str[0] = '\0';
    printf("Inserisci il secondo operando: ");
    fgets(str, sizeof(str), stdin);
    strLen = strlen(str);
    if ((send(mySocket, str, strLen, 0)) < 0)
    {
        perror("Inviati un numero di byte diversi");
        close(mySocket);
        return 0;
    }

    // stampo il risultato ricevuto
    byteRicevuti = 0;
    if ((byteRicevuti = recv(mySocket, str, sizeof(str) - 1, 0)) < 0)
    {
        perror("Errore in ricezione stringa");
    }

    str[byteRicevuti] = '\0';
    printf("Risultato: %s\n", str);

    close(mySocket);
    return 0;
}
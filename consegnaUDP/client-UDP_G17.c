#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <netdb.h>

int main(void)
{
    int sock;
    struct sockaddr_in socketDestinatario;
    struct sockaddr_in socketMittente;
    char string[1024];
    char buffer[1024];

    printf("Inserisci la stringa da inviare al server (Es. a,5,6)\n");
    fgets(string, sizeof(string), stdin);

    if (strlen(string) > 1024)
    {
        perror("Stringa troppo lunga");
        return 1;
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        perror("Errore creazione scoket");
    }

    socketDestinatario.sin_addr.s_addr = inet_addr("127.0.0.1");
    socketDestinatario.sin_family = AF_INET;
    socketDestinatario.sin_port = htons(6666);

    if ((sendto(sock, string, strlen(string) + 1, 0, (struct sockaddr *)&socketDestinatario, sizeof(socketDestinatario))) != strlen(string) + 1)
    {
        perror("Problemi con l'invio della stringa, forse la connessione al server è fallita");
        return 1;
    }

    unsigned int sizeRcv = sizeof(socketMittente);
    int mittStrLen = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&socketMittente, &sizeRcv);

    if (socketDestinatario.sin_addr.s_addr != socketMittente.sin_addr.s_addr)
    {
        perror("ERRORE: arrivato pacchetto da fonte sconosciuta");
        return 1;
    }

    buffer[mittStrLen] = '\0';
    printf("Risultato: %s\n", buffer);

    close(sock);
    return 0;
}
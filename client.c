#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9999

int main() {
    struct sockaddr_in serv_addr;
    int sock = 0;
    char buffer[1024] = {0};
    FILE *file;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    file = fopen("Received_Google.html", "wb");
    if (file == NULL) {
        perror("File cannot be opened");
        return -1;
    }

    while (read(sock, buffer, sizeof(buffer) - 1) > 0) {
        fwrite(buffer, sizeof(char), strlen(buffer), file);
        memset(buffer, 0, sizeof(buffer));
    }

    fclose(file);
    close(sock);

    // Abrir el archivo en el navegador
    system("xdg-open Received_Google.html");

    return 0;
}

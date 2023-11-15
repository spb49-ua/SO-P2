#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netdb.h> // Incluir para NI_MAXHOST

#define PORT 9999

void printIPAddress() {
    struct ifaddrs *interfaces, *iface;
    char host[NI_MAXHOST]; // Tamaño del buffer para la dirección IP

    if (getifaddrs(&interfaces) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    printf("Server IP Addresses:\n");
    for (iface = interfaces; iface != NULL; iface = iface->ifa_next) {
        if (iface->ifa_addr == NULL) continue;
        if (iface->ifa_addr->sa_family == AF_INET) { // check it is IP4
            // is a valid IP4 Address
            void* tmpAddrPtr = &((struct sockaddr_in *)iface->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, host, NI_MAXHOST);
            printf("%s: %s\n", iface->ifa_name, host); 
        }
    }

    freeifaddrs(interfaces);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    FILE *file;

    // Crear descriptor de archivo de socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Asignar el socket al puerto 9999
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vincular el socket al puerto 9999
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printIPAddress(); // Imprimir la dirección IP del servidor

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Enviar archivo "Google.html"
    file = fopen("Google.html", "rb");
    if (file == NULL) {
        perror("File not found");
        exit(EXIT_FAILURE);
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        send(new_socket, buffer, strlen(buffer), 0);
        memset(buffer, 0, sizeof(buffer));
    }

    printf("File sent successfully\n");

    fclose(file);
    close(new_socket);
    close(server_fd);
    return 0;
}

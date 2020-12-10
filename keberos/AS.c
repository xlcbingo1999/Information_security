#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <time.h>
#undef _UNICODE

#define AS_port 20001
#define maxChar 1024

int main(int argc, char **argv) {
    int socket_AS;
    int socket_client;
    struct sockaddr_in socket_AS_addr;
    struct sockaddr_in socket_client_addr;
    int socket_addr_len = sizeof(struct sockaddr_in);
    char buffer[maxChar];

    // if (argc != 2) {
    //     printf("usage: ./client <ipaddress>\n");
    //     exit(0);
    // }
    
    if ((socket_AS = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        exit(0);
    } else {
        printf("create socket: success.\n");
    }

    memset(&socket_AS_addr, 0, sizeof(socket_AS_addr));
    socket_AS_addr.sin_family = AF_INET;
    socket_AS_addr.sin_port = htons(AS_port);
    socket_AS_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_AS, (struct sockaddr *)&socket_AS_addr, sizeof(socket_AS_addr)) < 0) {
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    } else {
        printf("bind socket: success.\n");
    }

    if (listen(socket_AS, 100) == -1) {
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    } else {
        printf("listen client: success.\n");
    }

    while(1) {
        if (socket_client = accept(socket_AS, (struct sockaddr *)&socket_client_addr, (socklen_t *)&socket_addr_len) < 0) {
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        } else {
            printf("accept socket: success.\n");
        }
        int received_msg = recv(socket_client, buffer, maxChar, 0);
        buffer[received_msg] = '\0';
        printf("receive msg: %s\n", buffer);
        if (strcmp(buffer, "beijing") == 0) {
            printf("receive msg: success.\n");
        } else {
            printf("receive msg: not right.\n");
        }
        close(socket_client);
    }
    close(socket_AS);
    return 0;
}
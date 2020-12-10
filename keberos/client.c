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
#define SS_port 20000
#define AS_port 20001
#define TGS_port 20002
#define maxChar 1024

int main(int argc, char** argv) {
    int socket_client;
    struct sockaddr_in socket_AS_addr;
    memset(&socket_AS_addr, 0, sizeof(socket_AS_addr));
    socket_AS_addr.sin_family = AF_INET;
    socket_AS_addr.sin_port = htons(AS_port);
    socket_AS_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    char clientID[8] = "beijing";
    clientID[7] = '\0';


    
    if ((socket_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        exit(0);
    } else {
        printf("create socket: success.\n");
    }

    if (connect(socket_client, (struct sockaddr*)&socket_AS_addr, sizeof(socket_AS_addr)) < 0) {
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    } else {
        printf("create connect: success.\n");
    }

    printf("client connects to AS: success.\n");
    printf("send client ID: %s\n", clientID);
    send(socket_client, clientID, strlen(clientID), 0);
    printf("send client ID: success.\n");

    // char msg_A[maxChar];
    // int msg_A_len = recv(socket_client, msg_A, maxChar, 0);
    // msg_A[msg_A_len] = '\0';
    // if (strcmp(msg_A, "beijing") == 0) {
    //     printf("receive A: success.\n");
    // } else {
    //     printf("receive A: not right.\n");
    // }
    

    close(socket_client);
    printf("phase 1: success.\n");
    return 0;
}
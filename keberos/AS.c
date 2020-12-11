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
#include "des.c"
#undef _UNICODE

#define AS_port 20001
#define maxChar 1024

const char clientID[] = "beijing";

const unsigned char *K_client = "keywith1";
const unsigned char *K_TGS = "keywith3";


int main(int argc, char **argv) {
    int socket_AS;
    int socket_client;
    struct sockaddr_in socket_AS_addr;
    struct sockaddr_in socket_client_addr;
    int socket_addr_len = sizeof(struct sockaddr_in);
    char buffer[maxChar];
    pid_t pid;
    
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
    // int received_msg = -1;
    
    while(1) {
        socket_client = accept(socket_AS, (struct sockaddr *)&socket_client_addr, (socklen_t *)&socket_addr_len);
        if (socket_client < 0) {
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            exit(0);
        } else {
            printf("accept socket: success.\n");
        }
        int received_msg = -1;
        received_msg = recv(socket_client, buffer, maxChar, 0);
        buffer[received_msg] = '\0';
        printf("receive msg: %s. [length: %d]\n", buffer, received_msg);
        if (strcmp(buffer, clientID) == 0) {
            printf("用户名: %s [状态:success]\n", buffer);
        } else {
            printf("用户名: %s [状态:failed]\n", buffer);
            send(socket_client, "client ID is wrong.", strlen("client ID is wrong."), 0); // 增加一个返回，用于报错
            continue;
        }
        // 发送消息a给client
        unsigned char *K_client_TGS = "keywith2"; // 只会生成一次，应该在回话过程中生成
        unsigned char messageA[maxChar];
        uint64_t key_client = kstr2k64(K_client);
        int messageA_len = encryption(K_client_TGS, messageA, key_client);
        messageA[messageA_len] = '\0';
        send(socket_client, messageA, messageA_len, 0);

        // 发送消息b给client，消息b用K_TGS加密
        unsigned char messageB_ori[maxChar];
        sprintf(messageB_ori, "<%s,%s,%ld,%s>", clientID, inet_ntoa(socket_client_addr.sin_addr), time(NULL) + 600, K_client_TGS);
        printf("send messageB origin: %s length %d\n", messageB_ori, strlen(messageB_ori));
        uint64_t key_TGS = kstr2k64(K_TGS);
        unsigned char messageB[maxChar];
        int messageB_len = encryption(messageB_ori, messageB, key_TGS);
        messageB[messageB_len] = '\0';
        send(socket_client, messageB, messageB_len, 0);
        printf("send messageB: %s length %d\n", messageB, messageB_len);

    }
    close(socket_AS);
    return 0;
}
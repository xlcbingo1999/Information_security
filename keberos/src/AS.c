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
#include "md5.c"
#undef _UNICODE

#define AS_port 20201
#define maxChar 1024

const unsigned char clientID_store[] = "beijing";
const char clientPassword_store[] = "123456";
const unsigned char *K_TGS = "keywith3";

int main(int argc, char **argv) {
    int socket_AS;
    int socket_client;
    struct sockaddr_in socket_AS_addr;
    struct sockaddr_in socket_client_addr;
    int socket_addr_len = sizeof(struct sockaddr_in);
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
    
    while(1) {
        socket_client = accept(socket_AS, (struct sockaddr *)&socket_client_addr, (socklen_t *)&socket_addr_len);
        if (socket_client < 0) {
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            exit(0);
        } else {
            printf("accept socket: success.\n");
        }

        unsigned char clientID[maxChar];
        int clientID_len = recv(socket_client, clientID, maxChar, 0);
        clientID[clientID_len] = '\0';
        printf("接收clientID: success.\n");
        if (strcmp(clientID, clientID_store) == 0) {
            printf("clientID: %s [状态: 存在于本地数据库中]\n", clientID);
        } else {
            printf("clientID: %s [状态: 不存在于本地数据库中]\n", clientID);
            send(socket_client, "client ID is wrong.", strlen("client ID is wrong."), 0); // 增加一个返回，用于报错
            continue;
        }

        // 根据密码生成密钥
        unsigned char K_client_temp[16];
        unsigned char K_client[8];
        MD5(clientPassword_store, K_client_temp);
        afterMD5Hash(K_client_temp, K_client);
        printf("利用clientID，在本地数据库中查询密码并用hash函数转换为主密钥K_client\n");

        // 发送消息a给client
        unsigned char *K_client_TGS = "keywith2"; // 只会生成一次，应该在回话过程中生成
        unsigned char messageA[maxChar];
        uint64_t key_client = kstr2k64(K_client);
        int messageA_len = encryption(K_client_TGS, messageA, key_client);
        messageA[messageA_len] = '\0';
        send(socket_client, messageA, messageA_len, 0);
        printf("发送messageA[原文: %s用K_client加密]: success.\n", K_client_TGS);

        // 发送消息b给client，消息b用K_TGS加密
        unsigned char messageB_ori[maxChar];
        sprintf(messageB_ori, "<%s,%s,%ld,%s>", clientID, inet_ntoa(socket_client_addr.sin_addr), time(NULL) + 6000, K_client_TGS);
        printf("将clientID、client网络地址、票据有效时间和K_client_TGS打包成messageB的原文.\n");
        uint64_t key_TGS = kstr2k64(K_TGS);
        unsigned char messageB[maxChar];
        int messageB_len = encryption(messageB_ori, messageB, key_TGS);
        messageB[messageB_len] = '\0';
        send(socket_client, messageB, messageB_len, 0);
        printf("发送messageB[原文: %s用K_TGS加密]: success.\n", messageB_ori);

        unsigned char quit_str[maxChar];
        int quit_len = recv(socket_client, quit_str, maxChar, 0);
        if (strcmp(quit_str, "quit") == 0) {
            printf("完成任务，AS自动退出.\n");
            break;
        }
    }
    close(socket_AS);
    return 0;
}
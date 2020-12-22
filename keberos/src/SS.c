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

#define SS_port 20203
#define maxChar 1024
const unsigned char *K_SS = "keywith4";
const unsigned char *serviceID_store = "shanghai";

int main(int argc, char **argv) {
    int socket_SS;
    int socket_client;
    struct sockaddr_in socket_SS_addr;
    struct sockaddr_in socket_client_addr;
    int socket_addr_len = sizeof(struct sockaddr_in);
    char buffer[maxChar];
    
    if ((socket_SS = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        exit(0);
    } else {
        printf("create socket: success.\n");
    }

    memset(&socket_SS_addr, 0, sizeof(socket_SS_addr));
    socket_SS_addr.sin_family = AF_INET;
    socket_SS_addr.sin_port = htons(SS_port);
    socket_SS_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_SS, (struct sockaddr *)&socket_SS_addr, sizeof(socket_SS_addr)) < 0) {
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    } else {
        printf("bind socket: success.\n");
    }

    if (listen(socket_SS, 100) == -1) {
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    } else {
        printf("listen client: success.\n");
    }

    while(1) {
        socket_client = accept(socket_SS, (struct sockaddr *)&socket_client_addr, (socklen_t *)&socket_addr_len);
        if (socket_client < 0) {
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        } else {
            printf("accept socket: success.\n");
        }

        unsigned char messageE[maxChar];
        int messageE_len = recv(socket_client, messageE, maxChar, 0);
        messageE[messageE_len] = '\0';
        printf("接收messageE: success.\n");
        
        unsigned char serviceID[maxChar];
        unsigned char ST[maxChar];
        for (int i = 0; i < messageE_len; ++i) {
            if (messageE[i] == ',') {
                serviceID[i] = '\0';
                break;
            } else {
                serviceID[i] = messageE[i];
            }
        }
        for (int i = strlen(serviceID) + 1; i < messageE_len; ++i) {
            ST[i - (strlen(serviceID) + 1)] = messageE[i];
        }
        ST[messageE_len - (strlen(serviceID) + 1)] = '\0';
        printf("分解消息E获得serviceID: %s\n", serviceID);
        printf("分解消息E获得ST: %s\n", ST);
        uint64_t key_SS = kstr2k64(K_SS);
        unsigned char ST_ori[maxChar];
        int ST_ori_len = decryption(ST, ST_ori, key_SS);
        ST_ori[ST_ori_len] = '\0';

        unsigned char clientID[maxChar];
        unsigned char client_address[maxChar];
        time_t validity;
        unsigned char K_client_SS[maxChar];
        sscanf(ST_ori, "<%[^,],%[^,],%ld,%[^>]>", clientID, client_address, &validity, K_client_SS);
        printf("用K_SS解密ST获得clientID: %s\n", clientID);
        printf("用K_SS解密ST获得client网络地址client_address: %s\n", client_address);
        printf("用K_SS解密ST获得票据有效期限validity: %ld\n", validity);
        printf("用K_SS解密ST获得K_client_SS: %s\n", K_client_SS);

        unsigned char messageG[maxChar];
        int messageG_len = recv(socket_client, messageG, maxChar, 0);
        messageG[messageG_len] = '\0';
        printf("接收messageG: success.\n");
        uint64_t key_client_SS = kstr2k64(K_client_SS);
        unsigned char messageG_ori[maxChar];
        int messageG_ori_len = decryption(messageG, messageG_ori, key_client_SS);
        messageG_ori[messageG_ori_len] = '\0';

        unsigned char clientID_ver[maxChar];
        time_t timestamp;
        sscanf(messageG_ori, "<%[^,],%ld>", clientID_ver, &timestamp);
        printf("用K_client_SS解密消息G获得clientID: %s\n", clientID_ver);
        printf("用K_client_SS解密消息G获得时间戳: %ld\n", timestamp);

        if (strcmp(serviceID_store, serviceID) != 0) {
            printf("serviceID wrong!\n");
            send(socket_client, "serviceID wrong!", strlen("serviceID wrong!"), 0);
            continue;
        } else {
            printf("serviceID right!\n");
        }

        if (strcmp(clientID, clientID_ver) != 0) {
            printf("clientID wrong!\n");
            send(socket_client, "clientID wrong!", strlen("clientID wrong!"), 0);
            continue;
        } else {
            printf("clientID right!\n");
        }

        unsigned char messageH_ori[maxChar];
        sprintf(messageH_ori, "<%s,%ld>", clientID, timestamp+1);
        printf("将clientID和时间戳+1封装成消息H的原文.\n");
        unsigned char messageH[maxChar];
        int messageH_len = encryption(messageH_ori, messageH, key_client_SS);
        messageH[messageH_len] = '\0';
        send(socket_client, messageH, messageH_len, 0);
        printf("发送messageH[原文: %s用K_client_SS加密]: success.\n", messageH_ori);

        unsigned char quit_str[maxChar];
        int quit_len = recv(socket_client, quit_str, maxChar, 0);
        if (strcmp(quit_str, "quit") == 0) {
            printf("完成任务，SS自动退出.\n");
            break;
        }
    }
    close(socket_SS);
    return 0;
}

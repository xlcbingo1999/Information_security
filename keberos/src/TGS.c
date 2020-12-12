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

#define TGS_port 20202
#define maxChar 1024

const unsigned char *K_TGS = "keywith3";
const unsigned char *K_SS = "keywith4";
const unsigned char *serviceID_store = "shanghai";

int main(int argc, char **argv) {
    int socket_TGS;
    int socket_client;
    struct sockaddr_in socket_TGS_addr;
    struct sockaddr_in socket_client_addr;
    int socket_addr_len = sizeof(struct sockaddr_in);
    char buffer[maxChar];
    
    if ((socket_TGS = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        exit(0);
    } else {
        printf("create socket: success.\n");
    }

    memset(&socket_TGS_addr, 0, sizeof(socket_TGS_addr));
    socket_TGS_addr.sin_family = AF_INET;
    socket_TGS_addr.sin_port = htons(TGS_port);
    socket_TGS_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socket_TGS, (struct sockaddr *)&socket_TGS_addr, sizeof(socket_TGS_addr)) < 0) {
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    } else {
        printf("bind socket: success.\n");
    }

    if (listen(socket_TGS, 100) == -1) {
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    } else {
        printf("listen client: success.\n");
    }

    while(1) {
        socket_client = accept(socket_TGS, (struct sockaddr *)&socket_client_addr, (socklen_t *)&socket_addr_len);
        if (socket_client < 0) {
            printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
            continue;
        } else {
            printf("accept socket: success.\n");
        }
        unsigned char messageC[maxChar];
        int messageC_len = recv(socket_client, messageC, maxChar, 0);
        messageC[messageC_len] = '\0';
        printf("接收messageC: success.\n");
        
        unsigned char messageD[maxChar];
        int messageD_len = recv(socket_client, messageD, maxChar, 0);
        messageD[messageD_len] = '\0';
        printf("接收messageD: success.\n");

        unsigned char serviceID[maxChar];
        unsigned char messageB[maxChar];
        for (int i = 0; i < messageC_len; ++i) {
            if (messageC[i] == ',') {
                serviceID[i] = '\0';
                break;
            } else {
                serviceID[i] = messageC[i];
            }
        }
        for (int i = strlen(serviceID) + 1; i < messageC_len; ++i) {
            messageB[i - (strlen(serviceID) + 1)] = messageC[i];
        }
        messageB[messageC_len - (strlen(serviceID) + 1)] = '\0';
        printf("分解消息C获得serviceID: %s\n", serviceID);
        printf("分解消息C获得messageB: %s\n", messageB);
        
        unsigned char messageB_ori[maxChar];
        uint64_t key_TGS = kstr2k64(K_TGS);
        int messageB_ori_len = decryption(messageB, messageB_ori, key_TGS);
        messageB_ori[messageB_ori_len] = '\0';
        
        unsigned char clientID[maxChar];
        unsigned char client_address[maxChar];
        time_t validity;
        unsigned char K_client_TGS[maxChar];
        sscanf(messageB_ori, "<%[^,],%[^,],%ld,%[^>]>", clientID, client_address, &validity, K_client_TGS);
        printf("用K_TGS解密消息B获得clientID: %s\n", clientID);
        printf("用K_TGS解密消息B获得client网络地址client_address: %s\n", client_address);
        printf("用K_TGS解密消息B获得票据有效期限validity: %ld\n", validity);
        printf("用K_TGS解密消息B获得K_client_TGS: %s\n", K_client_TGS);

        unsigned char messageD_ori[maxChar];
        uint64_t key_client_TGS = kstr2k64(K_client_TGS);
        int messageD_ori_len = decryption(messageD, messageD_ori, key_client_TGS);
        messageD_ori[messageD_ori_len] = '\0';
        // printf("messageD_ori:[%s] length %d\n", messageD_ori, messageD_ori_len);

        unsigned char clientID_ver[maxChar];
        time_t timestamp;
        sscanf(messageD_ori, "<%[^,],%ld>", clientID_ver, &timestamp);
        printf("用K_client_TGS解密消息D获得clientID: %s\n", clientID_ver);
        printf("用K_client_TGS解密消息D获得时间戳: %ld\n", timestamp);
        
        if (strcmp(serviceID_store, serviceID) != 0) {
            printf("serviceID wrong!\n");
            send(socket_client, "serviceID wrong!", strlen("serviceID wrong!"), 0);
            continue;
        } else {
            printf("serviceID right!\n");
        }

        if(strcmp(clientID, clientID_ver) != 0) {
            printf("clientID wrong!\n");
            send(socket_client, "clientID wrong!", strlen("clientID wrong!"), 0);
            continue;
        } else {
            printf("clientID right!\n");
        }

        if (timestamp > validity) {
            printf("请求超时.\n");
            send(socket_client, "certification expired!", strlen("certification expired!"), 0);
            continue;
        } else {
            printf("请求时间戳合法.\n");
        }

        unsigned char *K_client_SS = "keywith5"; // 只会生成一次，应该在回话过程中生成
        unsigned char ST_ori[maxChar];
        sprintf(ST_ori, "<%s,%s,%ld,%s>", clientID, client_address, validity, K_client_SS);
        printf("将clientID、client网络地址、票据有效期限和K_client_SS打包成ST的原文.\n");
        unsigned char ST[maxChar];
        uint64_t key_SS = kstr2k64(K_SS);
        int ST_len = encryption(ST_ori, ST, key_SS);
        ST[ST_len] = '\0';
        printf("用K_SS加密ST的原文获得ST.\n");
        unsigned char messageE[maxChar];
        sprintf(messageE, "%s,%s", serviceID, ST);
        printf("将serviceID和ST打包成messageE.\n");
        send(socket_client, messageE, strlen(messageE), 0);
        printf("发送messageE[原文: %s未加密]: success.\n", messageE);

        unsigned char messageF[maxChar];
        int messageF_len = encryption(K_client_SS, messageF, key_client_TGS);
        send(socket_client, messageF, messageF_len, 0);
        printf("发送messageF[原文: %s用K_client_TGS加密]: success.\n", K_client_SS);

        unsigned char quit_str[maxChar];
        int quit_len = recv(socket_client, quit_str, maxChar, 0);
        if (strcmp(quit_str, "quit") == 0) {
            printf("完成任务，TGS自动退出.\n");
            break;
        }
    }
    close(socket_TGS);
    return 0;
}
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
#define TGS_port 20202
#define SS_port 20203
#define maxChar 1024
const unsigned char *serviceID = "shanghai";

int main(int argc, char** argv) {
    // 用户输入用户ID和用户password
    unsigned char clientID[maxChar];
    char clientPassword[maxChar];
    if (argc == 3) {
        strcpy(clientID, argv[1]);
        printf("已输入用户ID: %s\n", clientID);
    } else {
        printf("请输入用户ID（长度限制1024，不得输入空格，换行完成输入）:");
        scanf("%s",clientID);
    }
    if (argc == 3) {
        strcpy(clientPassword, argv[2]);
        printf("已输入用户password: %s\n", clientPassword);
    } else {
        printf("请输入用户password（长度限制1024，不得输入空格，换行完成输入）:");
        scanf("%s",clientPassword);
    }
    unsigned char K_client_temp[16];
    unsigned char K_client[8];
    MD5(clientPassword, K_client_temp);
    afterMD5Hash(K_client_temp, K_client);
    printf("用户基于客户机程序登录阶段: success.\n");

    int socket_client;
    struct sockaddr_in socket_AS_addr;
    memset(&socket_AS_addr, 0, sizeof(socket_AS_addr));
    socket_AS_addr.sin_family = AF_INET;
    socket_AS_addr.sin_port = htons(AS_port);
    socket_AS_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 
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
        printf("client连接AS: success.\n");
    }
    
    // 发送明文消息
    send(socket_client, (char*)clientID, strlen(clientID), 0);
    printf("发送clientID: success.\n");

    // 消息a的获取，client可以解密
    unsigned char messageA[maxChar];
    int messageA_len = recv(socket_client, messageA, maxChar, 0);
    messageA[messageA_len] = '\0';
    if (strcmp(messageA, "client ID is wrong.") == 0) {
        printf("%s\n", messageA);    // 增加一个返回，用于报错
        close(socket_client);
        return -1;
    } else {
        printf("接收messageA: success.\n"); 
    }
    
    // 消息b的获取，client无法解密
    unsigned char messageB[maxChar];
    int messageB_len = recv(socket_client, messageB, maxChar, 0);
    messageB[messageB_len] = '\0';
    printf("接收messageB: success.\n", messageB, messageB_len);

    // 解密消息a，获得K_client_TGS
    uint64_t key_client = kstr2k64((char*)K_client);
    unsigned char K_client_TGS[maxChar];
    int K_client_TGS_len = decryption(messageA, K_client_TGS, key_client);
    K_client_TGS[K_client_TGS_len] = '\0';
    printf("用K_client解密消息A获得K_client_TGS: %s\n", K_client_TGS);

    // 为了方便运行，让服务器自动关闭
    send(socket_client, "quit", strlen("quit"), 0);

    close(socket_client);
    printf("客户端Client身份认证阶段: success.\n");
    sleep(2);

    struct sockaddr_in socket_TGS_addr;
    memset(&socket_TGS_addr, 0, sizeof(socket_TGS_addr));
    socket_TGS_addr.sin_family = AF_INET;
    socket_TGS_addr.sin_port = htons(TGS_port);
    socket_TGS_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((socket_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        exit(0);
    } else {
        printf("create socket: success.\n");
    }
    
    if (connect(socket_client, (struct sockaddr*)&socket_TGS_addr, sizeof(socket_TGS_addr)) < 0) {
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    } else {
        printf("client连接TGS: success.\n");
    }

    // 发送消息c给TGS
    unsigned char messageC[maxChar];
    sprintf(messageC, "%s,%s", (char*)serviceID, messageB);
    printf("将serviceID和messageB打包成messageC.\n");
    send(socket_client, messageC, strlen(messageC), 0);
    printf("发送messageC[原文: %s不加密]: success.\n", messageC);

    // 发送消息d给TGS，消息d用K_client_TGS加密
    unsigned char messageD_ori[maxChar];
    sprintf(messageD_ori, "<%s,%ld>", (char*)clientID, time(NULL));
    printf("将clientID和时间戳timestamp打包成messageD的原文.\n");
    uint64_t key_client_TGS = kstr2k64(K_client_TGS);
    unsigned char messageD[maxChar];
    int messageD_len = encryption(messageD_ori, messageD, key_client_TGS);
    messageD[messageD_len] = '\0';
    send(socket_client, messageD, messageD_len, 0);
    printf("发送messageD[原文: %s用K_client_TGS加密]: success.\n", messageD_ori);

    unsigned char messageE[maxChar];
    int messageE_len = recv(socket_client, messageE, maxChar, 0);
    if (strcmp(messageE, "serviceID wrong!") == 0 
    || strcmp(messageE, "clientID wrong!") == 0 
    || strcmp(messageE, "certification expired!") == 0 ) {
        printf("%s\n", messageE);
        close(socket_client);
        return -1;
    } else {
        printf("接收messageE: success.\n");
    }
   
    unsigned char messageF[maxChar];
    int messageF_len = recv(socket_client, messageF, maxChar, 0);
    printf("接收messageF: success\n");
    unsigned char K_client_SS[maxChar];
    int K_client_SS_len = decryption(messageF, K_client_SS, key_client_TGS);
    printf("用K_client_TGS解密消息F获得K_client_SS: %s\n", K_client_SS);

    // 为了方便运行，让服务器自动关闭
    send(socket_client, "quit", strlen("quit"), 0);

    close(socket_client);
    printf("客户机Client服务认证阶段: success.\n");
    sleep(2);

    struct sockaddr_in socket_SS_addr;
    memset(&socket_SS_addr, 0, sizeof(socket_SS_addr));
    socket_SS_addr.sin_family = AF_INET;
    socket_SS_addr.sin_port = htons(SS_port);
    socket_SS_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((socket_client = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        exit(0);
    } else {
        printf("create socket: success.\n");
    }
    
    if (connect(socket_client, (struct sockaddr*)&socket_SS_addr, sizeof(socket_SS_addr)) < 0) {
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    } else {
        printf("client连接SS: success.\n");
    }

    send(socket_client, messageE, messageE_len, 0);
    printf("发送messageE[原文: %s不加密]: success.\n", messageE);

    unsigned char messageG_ori[maxChar];
    time_t TS = time(NULL);
    sprintf(messageG_ori, "<%s,%ld>", (char*)clientID, TS);
    printf("将clientID和时间戳timestamp打包成messageG的原文.\n");
    unsigned char messageG[maxChar];
    uint64_t key_client_SS = kstr2k64(K_client_SS);
    int messageG_len = encryption(messageG_ori, messageG, key_client_SS);
    send(socket_client, messageG, messageG_len, 0);
    printf("发送messageG[原文: %s用K_client_SS加密]: success.\n", messageG_ori);

    unsigned char messageH[maxChar];
    int messageH_len = recv(socket_client, messageH, maxChar, 0);
    messageH[messageH_len] = '\0';
    if (strcmp(messageH, "serviceID wrong!") == 0 || strcmp(messageH, "clientID wrong!") == 0) {
        printf("%s\n", messageH);
        close(socket_client);
        return -1;
    } else {
        printf("接收messageH: success\n");
    }
    unsigned char messageH_ori[maxChar];
    int messageH_ori_len = decryption(messageH, messageH_ori, key_client_SS);
    messageH_ori[messageH_ori_len] = '\0';
    
    unsigned char clientID_ver[maxChar];
    time_t timestamp;
    sscanf(messageH_ori, "<%[^,],%ld>", clientID_ver, &timestamp);
    printf("用K_client_SS解密消息H获得clientID: %s\n", clientID_ver);
    printf("用K_client_SS解密消息H获得timestamp: %ld\n", timestamp);

    if(strcmp(clientID, clientID_ver) != 0) {
        printf("clientID wrong!\n");
        close(socket_client);
        return -1;
    } else {
        printf("clientID right!\n");
    }

    if (timestamp != TS+1) {
        printf("请求时间超时.\n");
        close(socket_client);
        return -1;
    } else {
        printf("请求时间戳合法.\n");
    }
    printf("客户机Client服务申请阶段: success.\n");
    printf("现在可以和SS正常通信了!\n");

    // 为了方便运行，让服务器自动关闭
    send(socket_client, "quit", strlen("quit"), 0);

    close(socket_client);
    return 0;
}
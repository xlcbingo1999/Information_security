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
#define SS_port 20000
#define AS_port 20001
#define TGS_port 20005
#define maxChar 1024

const unsigned char *K_client = "keywith1";
const unsigned char *clientID = "beijing";
const unsigned char *serviceID = "shanghai";

int main(int argc, char** argv) {
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
        printf("create connect: success.\n");
    }
    // 发送明文消息
    printf("client connects to AS: success.\n");
    send(socket_client, (char*)clientID, strlen(clientID), 0);
    printf("send client ID[%s]: success.\n", (char*)clientID);

    // 消息a的获取，client可以解密
    unsigned char messageA[maxChar];
    int messageA_len = recv(socket_client, messageA, maxChar, 0);
    messageA[messageA_len] = '\0';
    if (strcmp(messageA, "client ID is wrong.") == 0) {
        printf("%s\n", messageA);    // 增加一个返回，用于报错
        close(socket_client);
        return -1;
    } else {
        printf("receive message A[%s] length %d: success.\n", messageA, messageA_len); 
    }
    
    // 消息b的获取，client无法解密
    unsigned char messageB[maxChar];
    int messageB_len = recv(socket_client, messageB, maxChar, 0);
    messageB[messageB_len] = '\0';
    printf("receive message B[%s] length: %d: success.\n", messageB, messageB_len);

    // 解密消息a，获得K_client_TGS
    uint64_t key_client = kstr2k64((char*)K_client);
    unsigned char K_client_TGS[maxChar];
    int K_client_TGS_len = decryption(messageA, K_client_TGS, key_client);
    K_client_TGS[K_client_TGS_len] = '\0';
    printf("decrypt message A to get K_client_TGS[%s] length %d: success.\n", K_client_TGS, K_client_TGS_len);

    close(socket_client);
    printf("phase 1: success.\n");
    
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
        printf("client connects to TGS: success.\n");
    }

    // 发送消息c给TGS
    unsigned char messageC[maxChar];
    sprintf(messageC, "%s,%s", (char*)serviceID, messageB);
    printf("send messageC: %s length %d\n", messageC, strlen(messageC));
    send(socket_client, messageC, strlen(messageC), 0);
    printf("send messageC: success.\n");

    // 发送消息d给TGS，消息d用K_client_TGS加密
    unsigned char messageD_ori[maxChar];
    sprintf(messageD_ori, "<%s,%ld>", (char*)clientID, time(NULL));
    printf("send messageD origin: %s length %d\n", messageD_ori, strlen(messageD_ori));
    uint64_t key_client_TGS = kstr2k64(K_client_TGS);
    unsigned char messageD[maxChar];
    int messageD_len = encryption(messageD_ori, messageD, key_client_TGS);
    messageD[messageD_len] = '\0';
    printf("send messageD: %s length %d\n", messageD, messageD_len);
    send(socket_client, messageD, messageD_len, 0);

    unsigned char messageE[maxChar];
    int messageE_len = recv(socket_client, messageE, maxChar, 0);
    if (strcmp(messageE, "serviceID wrong!") == 0 
    || strcmp(messageE, "clientID wrong!") == 0 
    || strcmp(messageE, "certification expired!") == 0 ) {
        printf("%s\n", messageE);
        close(socket_client);
        return -1;
    } else {
        printf("receive messageE:[%s] length %d\n", messageE, messageE_len);
    }
   

    unsigned char messageF[maxChar];
    int messageF_len = recv(socket_client, messageF, maxChar, 0);
    unsigned char K_client_SS[maxChar];
    int K_client_SS_len = decryption(messageF, K_client_SS, key_client_TGS);
    printf("receive messageF K_client_SS:[%s] length %d\n", K_client_SS, K_client_SS_len);

    close(socket_client);
    printf("phase 2: success.\n");

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
        printf("create connect: success.\n");
    }

    printf("client connects to SS: success.\n");
    
    send(socket_client, messageE, messageE_len, 0);
    printf("send messageE:[%s] length %d\n", messageE, messageE_len);

    unsigned char messageG_ori[maxChar];
    time_t TS = time(NULL);
    sprintf(messageG_ori, "<%s,%ld>", (char*)clientID, TS);
    unsigned char messageG[maxChar];
    uint64_t key_client_SS = kstr2k64(K_client_SS);
    int messageG_len = encryption(messageG_ori, messageG, key_client_SS);
    send(socket_client, messageG, messageG_len, 0);
    printf("send messageG:[%s] length %d\n", messageG, messageG_len);

    unsigned char messageH[maxChar];
    int messageH_len = recv(socket_client, messageH, maxChar, 0);
    messageH[messageH_len] = '\0';
    if (strcmp(messageH, "serviceID wrong!") == 0 || strcmp(messageH, "clientID wrong!") == 0) {
        printf("%s\n", messageH);
        close(socket_client);
        return -1;
    }
    unsigned char messageH_ori[maxChar];
    int messageH_ori_len = decryption(messageH, messageH_ori, key_client_SS);
    messageH_ori[messageH_ori_len] = '\0';

    unsigned char clientID_ver[maxChar];
    time_t timestamp;
    sscanf(messageH_ori, "<%[^,],%ld>", clientID_ver, &timestamp);
    printf("messageH_ori clientID_ver:[%s] length %d\n", clientID_ver, strlen(clientID_ver));
    printf("messageH_ori timestamp:[%ld]\n", timestamp);

    if(strcmp(clientID, clientID_ver) != 0) {
        printf("clientID wrong!\n");
        close(socket_client);
        return -1;
    } else {
        printf("clientID right!\n");
    }

    if (timestamp != TS+1) {
        printf("certification expired!\n");
        close(socket_client);
        return -1;
    } else {
        printf("certification valid!\n");
    }

    printf("Now you can send request for service to SS.\n");

    close(socket_client);
    printf("phase 3: success.\n");
    return 0;
}
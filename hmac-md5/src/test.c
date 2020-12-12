#include "test.h"

int TEST_length_8() {
    printf("---------- BEGIN TEST_length_8 ---------\n");
    size_t mLen = 8;
    char message[mLen + 1];
    for (int i = 0; i < mLen; ++i) {
        message[i] = i + 'A';
    }
    message[mLen] = '\0';
    printf("origin message: \n%s\n", message);
    uint8_t shared_key[4] = "123\0";
    uint8_t right_result[16] = {0xe3, 0x3f, 0x5c, 0xa0, 0x13, 0x7f, 0x6e, 0x35,
                                0x3a, 0x2e, 0x00, 0xd8, 0x7f, 0xcf, 0xb1, 0x87};
    uint8_t *key_plus = (uint8_t *)malloc(block_count * 8);
    uint8_t *S_i = (uint8_t *)malloc(block_count * 8);
    uint8_t *S_o = (uint8_t *)malloc(block_count * 8);
    uint8_t HMAC_MD5_RESULT[17];
    hmac(message, shared_key, key_plus, S_i, S_o, HMAC_MD5_RESULT);
    printf("result:\n");
    for (int i = 0; i < 16; ++i) {
        printf("0x%02x ", HMAC_MD5_RESULT[i]);
        if (HMAC_MD5_RESULT[i] != right_result[i]) {
            printf("wrong!\n");
            free(key_plus);
            free(S_i);
            free(S_o);
            return -1;
        }
    }
    printf("\n");
    free(key_plus);
    free(S_i);
    free(S_o);
    return 0;
}

int TEST_length_512() {
    printf("---------- BEGIN TEST_length_512 ---------\n");
    size_t mLen = 512;
    char message[mLen + 1];
    for (int i = 0; i < mLen; ++i) {
        message[i] = (i % 26) + 'A' ;
    }
    message[mLen] = '\0';
    printf("origin message: \n%s\n", message);
    uint8_t shared_key[4] = "123\0";
    uint8_t right_result[16] = {0x34, 0xdd, 0x59, 0xd7, 0x5d, 0xe0, 0x8c, 0x5f,
                                0xb3, 0xc4, 0x15, 0xa2, 0x87, 0x25, 0xe3, 0x16};
    uint8_t *key_plus = (uint8_t *)malloc(block_count * 8);
    uint8_t *S_i = (uint8_t *)malloc(block_count * 8);
    uint8_t *S_o = (uint8_t *)malloc(block_count * 8);
    uint8_t HMAC_MD5_RESULT[17];
    hmac(message, shared_key, key_plus, S_i, S_o, HMAC_MD5_RESULT);
    printf("result:\n");
    for (int i = 0; i < 16; ++i) {
        printf("0x%02x ", HMAC_MD5_RESULT[i]);
        if (HMAC_MD5_RESULT[i] != right_result[i]) {
            printf("wrong!\n");
            free(key_plus);
            free(S_i);
            free(S_o);
            return -1;
        }
    }
    printf("\n");
    free(key_plus);
    free(S_i);
    free(S_o);
    return 0;
}


int TEST_length_1024() {
        printf("---------- BEGIN TEST_length_1024 ---------\n");
    size_t mLen = 1024;
    char message[mLen + 1];
    for (int i = 0; i < mLen; ++i) {
        message[i] = (i % 26) + 'A' ;
    }
    message[mLen] = '\0';
    printf("origin message: \n%s\n", message);
    uint8_t shared_key[4] = "123\0";
    uint8_t right_result[16] = {0x54, 0x12, 0xf1, 0x32, 0x99, 0xb2, 0x16, 0x5e,
                                0xed, 0xc5, 0x6c, 0x15, 0x22, 0xee, 0x9a, 0x97};
    uint8_t *key_plus = (uint8_t *)malloc(block_count * 8);
    uint8_t *S_i = (uint8_t *)malloc(block_count * 8);
    uint8_t *S_o = (uint8_t *)malloc(block_count * 8);
    uint8_t HMAC_MD5_RESULT[17];
    hmac(message, shared_key, key_plus, S_i, S_o, HMAC_MD5_RESULT);
    printf("result:\n");
    for (int i = 0; i < 16; ++i) {
        printf("0x%02x ", HMAC_MD5_RESULT[i]);
        if (HMAC_MD5_RESULT[i] != right_result[i]) {
            printf("wrong!\n");
            free(key_plus);
            free(S_i);
            free(S_o);
            return -1;
        }
    }
    printf("\n");
    free(key_plus);
    free(S_i);
    free(S_o);
    return 0;
}
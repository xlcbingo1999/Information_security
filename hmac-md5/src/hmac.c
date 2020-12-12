#include "hmac.h"

const uint8_t ipad = 0b00110110;
const uint8_t opad = 0b01011100;
const int block_count = 64;  // MD5固定为 512 / 8 = 64

/*
    生成K+
*/
void generateKplus(uint8_t *shared_key, uint8_t *key_plus) {
    for (int i = 0; i < strlen(shared_key); ++i) {
        key_plus[i] = shared_key[i];
    }
    for (int i = strlen(shared_key); i < block_count; ++i) {
        key_plus[i] = 0x00;
    }
}

/*
    生成S_i
*/
void generateS_i(uint8_t *key_plus, uint8_t *S_i) {
    for (int i = 0; i < block_count; ++i) {
        S_i[i] = key_plus[i] ^ ipad;
    }
}

/*
    生成S_o
*/
void generateS_o(uint8_t *key_plus, uint8_t *S_o) {
    for (int i = 0; i < block_count; ++i) {
        S_o[i] = key_plus[i] ^ opad;
    }
}

/*
    哈希压缩，这里为了用%s打印，所以都为8位字符数组加上'\0'
*/
void hashFunction(char *message, uint8_t *S_i, uint8_t *S_o, uint8_t *HMAC_MD5_RESULT) {
    uint8_t *con_Si_M = (uint8_t *)malloc(sizeof(uint8_t) * (block_count + strlen(message) + 1));
    for (int i = 0; i < block_count; ++i) {
        con_Si_M[i] = S_i[i];
    }
    for (int i = 0; i < strlen(message); ++i) {
        con_Si_M[block_count + i] = message[i];
    }
    con_Si_M[block_count + strlen(message)] = '\0';

    uint8_t MD5_RESULT_con_Si_M[17];
    MD5(con_Si_M, MD5_RESULT_con_Si_M);
    MD5_RESULT_con_Si_M[16] = '\0';

    uint8_t *con_So_Si_M = (uint8_t *)malloc(sizeof(uint8_t) * (block_count + 16 + 1));
    for (int i = 0; i < block_count; ++i) {
        con_So_Si_M[i] = S_o[i];
    }
    for (int i = 0; i < 16; ++i) {
        con_So_Si_M[block_count + i] = MD5_RESULT_con_Si_M[i];
    }
    con_So_Si_M[block_count + 16] = '\0';

    MD5(con_So_Si_M, HMAC_MD5_RESULT);
    HMAC_MD5_RESULT[16] = '\0';

    free(con_So_Si_M);
    free(con_Si_M);
}

/*
    总控函数
*/
void hmac(char *message, uint8_t *shared_key, uint8_t *key_plus, uint8_t *S_i, uint8_t *S_o, uint8_t *HMAC_MD5_RESULT_ptr) {
    generateKplus(shared_key, key_plus);
    generateS_i(key_plus, S_i);
    generateS_o(key_plus, S_o);
    hashFunction(message, S_i, S_o, HMAC_MD5_RESULT_ptr);
}
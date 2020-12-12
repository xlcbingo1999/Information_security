#ifndef HMAC_H
#define HMAC_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "md5.h"

extern const uint8_t ipad;
extern const uint8_t opad;
extern const int block_count;

/*
    生成K+
*/
void generateKplus(uint8_t *shared_key, uint8_t *key_plus);

/*
    生成S_i
*/
void generateS_i(uint8_t *key_plus, uint8_t *S_i);

/*
    生成S_o
*/
void generateS_o(uint8_t *key_plus, uint8_t *S_o);

/*
    哈希压缩，这里为了用%s打印，所以都为8位字符数组加上'\0'
*/
void hashFunction(char *message, uint8_t *S_i, uint8_t *S_o, uint8_t *HMAC_MD5_RESULT);


/*
    总控函数
*/
void hmac(char *message, uint8_t *shared_key, uint8_t *key_plus, uint8_t *S_i, uint8_t *S_o, uint8_t *HMAC_MD5_RESULT_ptr);

#endif HMAC_H

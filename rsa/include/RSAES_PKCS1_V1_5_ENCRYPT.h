#ifndef RSAESPKCS1V15ENCRYPT_H
#define RSAESPKCS1V15ENCRYPT_H

#include "rsa_func.h"

/* 
- RSAES_PKCS1_V1_5_ENCRYPT
- 输入：
- ciphertext: unsigned char * 返回加密后的密文
- key_N: mpz_t 公钥
- key_e: mpz_t 公钥
- origin_message: unsigned char * 原始报文
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
int RSAES_PKCS1_V1_5_ENCRYPT(unsigned char *ciphertext, mpz_t key_N, mpz_t key_e, unsigned char *origin_message);

#endif RSAESPKCS1V15ENCRYPT_H
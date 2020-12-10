#ifndef RSAESPKCS1V15DECRYPT_H
#define RSAESPKCS1V15DECRYPT_H

#include "rsa_func.h"

/* 
- RSAES_PKCS1_V1_5_DECRYPT
- 输入：
- dec_message: unsigned char * 返回解密后的报文，正常情况下和输入报文相同
- key_N: mpz_t 私钥
- key_d: mpz_t 私钥
- ciphertext: unsigned char * 加密后的报文
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
int RSAES_PKCS1_V1_5_DECRYPT(unsigned char *dec_message, mpz_t key_N, mpz_t key_d, unsigned char *ciphertext);

#endif RSAESPKCS1V15DECRYPT_H
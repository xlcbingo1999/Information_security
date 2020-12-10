#ifndef GENERATEKEY_H
#define GENERATEKEY_H

#include "rsa_func.h"

/* 
- generate_key_random
- 输入：
- key_e: 返回的公钥的e引用
- key_d: 返回的密钥的d引用
- key_N: 返回的公钥和密钥共有的N引用
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
int generate_key_random(mpz_t key_N, mpz_t key_e, mpz_t key_d, int key_size);

/* 
- generate_key_with_p_and_q
- 输入：
- key_e: 返回的公钥的e引用
- key_d: 返回的密钥的d引用
- key_N: 返回的公钥和密钥共有的N引用
- key_p: 输入的大素数p
- key_q: 输入的大素数q
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
int generate_key_with_p_and_q(mpz_t key_N, mpz_t key_e, mpz_t key_d, mpz_t key_p, mpz_t key_q);

#endif GENERATEKEY_H
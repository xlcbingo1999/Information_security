#ifndef RSAFUNC_H
#define RSAFUNC_H

#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* 
- OS2IP
- 输入：
- MESSAGE: mpz_t 返回大数
- EM: unsigned char * 8位字符数组
- MESSAGE_STR_NUM： size_t 字符数组长度
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
int OS2IP(mpz_t MESSAGE, unsigned char *EM, size_t MESSAGE_STR_NUM);

/* 
- I2OSP
- 输入：
- Message_C_ptr: unsigned char * 返回8位字符数组
- encode_small_c: mpz_t 大数
- xLen: size_t 大叔字节数
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
int I2OSP(unsigned char *Message_C_ptr, mpz_t encode_small_c, size_t xLen);

/* 
- RSAEP
- 输入：
- encrypt_small_c: mpz_t 编码结果
- n: mpz_t 底数
- key_e: mpz_t 指数
- key_N: mpz_t 求模时使用的除数
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
int RSAEP(mpz_t encrypt_small_c, mpz_t n, mpz_t key_e, mpz_t key_N);

/* 
- RSADP
- 输入：
- decrypt_small_m: mpz_t 解码结果
- c: mpz_t 底数
- key_d: mpz_t 指数
- key_N: mpz_t 求模时使用的除数
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
int RSADP(mpz_t decrypt_small_m, mpz_t c, mpz_t key_d, mpz_t key_N);

#endif
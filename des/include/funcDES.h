#ifndef FUNCDES_H
#define FUNCDES_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

/* 
- 64位数据转换为64位数组（长度64）
- 输入：
- data: uint64_t 64位数据
- boolArrayFin: uint64_t[] 64位数组（长度64）
- 输出：
- void
*/
void transToBoolArray64(uint64_t data, uint64_t *boolArrayFin);

/* 
- 32位数据转换为32位数组（长度32）
- 输入：
- data: uint32_t 32位数据
- boolArrayFin: uint32_t[] 32位数组（长度32）
- 输出：
- void
*/
void transToBoolArray32(uint32_t data, uint32_t *boolArrayFin);

/* 
- 48位数据转换为48位数组（长度48）
- 输入：
- data: uint64_t 64位数据（实际48位）
- boolArrayFin: uint64_t[] 48位数组（长度48）
- 输出：
- void
*/
void transToBoolArray48(uint64_t data, uint64_t *boolArrayFin);

/* 
- 初始置换IP
- 输入：
- data: uint64_t 64位数据
- L0_p: uint32_t[] 32位数组（长度32）
- R0_p: uint32_t[] 32位数组（长度32）
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
int initTransformIP(uint64_t data , uint32_t *L0_p, uint32_t *R0_p);

/* 
- feistel轮函数
- 输入：
- R: uint32_t 32位串
- sub_K: uint64_t 48位子密钥
- 输出：
- uint32_t: 结果 32位串
*/
uint32_t feistel(uint32_t R, uint64_t sub_K);

/* 
- 置换PC-1
- 输入：
- key_real_Array: uint64_t[] 64位key
- C0: uint64_t[] 28位C0
- D0: uint64_t[] 28位D0
- 输出：
- void
*/
void PC1_trans(uint64_t *key_real_Array, uint64_t *C0, uint64_t *D0);

/* 
- 循环左移1位
- 输入：
- C_origin: uint64_t[] C_{i}源数组
- D_origin: uint64_t[] D_{i}源数组
- C_fin: uint64_t[] C_{i}左移后数组
- D_fin: uint64_t[] D_{i}左移后数组
- 输出：
- void
*/
void shiftLeft1(uint64_t *C_origin, uint64_t *D_origin, uint64_t *C_fin, uint64_t *D_fin);

/* 
- 循环左移2位
- 输入：
- C_origin: uint64_t[] C_{i}源数组
- D_origin: uint64_t[] D_{i}源数组
- C_fin: uint64_t[] C_{i}左移后数组
- D_fin: uint64_t[] D_{i}左移后数组
- 输出：
- void
*/
void shiftLeft2(uint64_t *C_origin, uint64_t *D_origin, uint64_t *C_fin, uint64_t *D_fin);

/* 
- 压缩后置换PC-2
- 输入：
- C: uint64_t[] 28位C1-C16
- D: uint64_t[] 28位D1-D16
- 输出：
- void
*/
uint64_t PC2_compression(uint64_t *C, uint64_t *D);

/* 
- 获得48位子密钥
- 输入：
- key: uint64_t 密钥
- K: uint64_t[] 子密钥数组（48位，长度16）
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
void get48SubKey(uint64_t key, uint64_t *K);

/* 
- 加密器中的16次迭代序列
- 输入：
- L0: uint32_t 输入L0
- R0: uint32_t 输入R0
- key: uint64_t 输入密钥
- 输出：
- uint64_t: 交换置换后的64位串
*/
uint64_t T_16(uint32_t L0, uint32_t R0, uint64_t key);

/* 
- 解密器中的16次迭代序列
- 输入：
- L0: uint32_t 输入L0
- R0: uint32_t 输入R0
- key: uint64_t 输入密钥
- 输出：
- uint64_t: 交换置换后的64位串
*/
uint64_t T_16_D(uint32_t L0, uint32_t R0, uint64_t key);

/* 
- 逆置换IP^{-1}
- 输入：
- data: uint64_t 交换置换后的64位串
- 输出：
- uint64_t: 密文或明文
*/
uint64_t initTransformIPtransver(uint64_t data);

/* 
- 字符串转位数组（长度64）
- 输入：
- str: unsigned char[] 源数据的字节数组
- metadata: uint64_t[] 64位数组（长度64）
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
int str2metadata(unsigned char *str, uint64_t *metadata);

#endif
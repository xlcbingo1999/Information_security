#ifndef MD5_H
#define MD5_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define BITS_PER_BLOCK 512
#define BITS_PER_BYTE 8
#define BYTES_PER_BLOCK 64

/*
    T表
*/
extern const uint32_t T[64];


/*
    左循环移位的位数s值表格
*/
extern const int s[64];

/*
    初始向量IV
*/
extern const uint32_t IV[4];

/*
    表示寄存器内容的数组
*/
extern uint32_t CV[4]; 

uint32_t F(uint32_t b, uint32_t c, uint32_t d);
uint32_t G(uint32_t b, uint32_t c, uint32_t d);
uint32_t H(uint32_t b, uint32_t c, uint32_t d);
uint32_t I(uint32_t b, uint32_t c, uint32_t d);
uint32_t CLS(uint32_t x, int s);

/*
    将源文转化为与寄存器位对齐的padding表示
*/
void CharToInteger(uint8_t *origin_message, uint32_t *target_message, int origin_message_length);

/*
    将与寄存器位对齐的padding表示表示为字符表示
*/
void IntegerToChar(uint32_t *origin_message, uint8_t *target_message, int origin_message_length);

/*
    获取划分后的块的数量
*/
int MD5_GETBLOCKSIZE(uint8_t *message);

/*
    填充，并将结果转换为和寄存器对齐的32位无符号整数
*/
void MD5_PADDING(uint8_t *message, int block_count, uint32_t *padding_message_32);

/*
    缓冲区初始化
*/
void MD5_INIT();

/*
    循环压缩
*/
void MD5_HASH(uint32_t *X);

/*
    分块和循环压缩
*/
void MD5_DIVISION_AND_HASH(uint32_t *padding_message_32, int block_count);

/*
    总控函数
*/
void MD5(char *message, uint8_t *MD5_RESULT_CHAR);



#endif MD5_H
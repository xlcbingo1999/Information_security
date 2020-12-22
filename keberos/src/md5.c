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
/*
    T表
*/ 
const uint32_t T[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};


/*
    左循环移位的位数s值表格
*/
const int s[64] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

/*
    初始向量IV
*/
const uint32_t IV[4] = {
    0x67452301,
    0xEFCDAB89,
    0x98BADCFE,
    0x10325476,
};

/*
    表示寄存器内容的数组
*/
uint32_t CV[4]; 

uint32_t F(uint32_t b, uint32_t c, uint32_t d) {
    return (b & c) | (~b & d);
}

uint32_t G(uint32_t b, uint32_t c, uint32_t d) {
    return (b & d) | (c & ~d);
}

uint32_t H(uint32_t b, uint32_t c, uint32_t d) {
    return (b ^ c ^ d);
}

uint32_t I(uint32_t b, uint32_t c, uint32_t d) {
    return (c ^ (b | ~d));
}

uint32_t CLS(uint32_t x, int s) {
    return ((x << s) | (x >> (32 - s)));
}  // 循环左移

/*
    将源文转化为与寄存器位对齐的padding表示
*/
void CharToInteger(uint8_t *origin_message, uint32_t *target_message, int origin_message_length) {
    for (int i = 0; i < origin_message_length; ++i) {
        // 注意小端
        target_message[i] = ((origin_message[i * 4]) | (origin_message[i * 4 + 1] << 8) | (origin_message[i * 4 + 2] << 16) | (origin_message[i * 4 + 3] << 24));
    }
}

/*
    将与寄存器位对齐的padding表示表示为字符表示
*/
void IntegerToChar(uint32_t *origin_message, uint8_t *target_message, int origin_message_length) {
    for (int i = 0; i < origin_message_length; ++i) {
        // 注意小端
        target_message[i * 4] = (origin_message[i]) & 0x000000ff;
        target_message[i * 4 + 1] = (origin_message[i] >> 8) & 0x000000ff;
        target_message[i * 4 + 2] = (origin_message[i] >> 16) & 0x000000ff;
        target_message[i * 4 + 3] = (origin_message[i] >> 24) & 0x000000ff;
    }
}

/*
    获取划分后的块的数量
*/
int MD5_GETBLOCKSIZE(uint8_t *message) {
    int message_len = strlen(message);
    // 如果长度的位数对512求余大于448，需要再多填充一个块
    int padding_size;
    if ((message_len * BITS_PER_BYTE) % BITS_PER_BLOCK > 448) {
        padding_size = 2;
    } else {
        padding_size = 1;
    }
    int block_count = message_len / BYTES_PER_BLOCK + padding_size;
    return block_count;
}

/*
    填充，并将结果转换为和寄存器对齐的32位无符号整数
*/
void MD5_PADDING(uint8_t *message, int block_count, uint32_t *padding_message_32) {
    uint8_t *padding_message_char = (uint8_t *)malloc(BITS_PER_BLOCK * block_count);
    uint64_t message_len = strlen(message);  // 这里使用uint64_t的原因是为了与2^64求余
    for (int i = 0; i < message_len; ++i) {
        padding_message_char[i] = message[i];
    }
    padding_message_char[message_len] = 0x80;
    for (int i = message_len + 1; i < block_count * BYTES_PER_BLOCK; ++i) {
        padding_message_char[i] = 0x00;
    }
    CharToInteger(padding_message_char, padding_message_32, block_count * BYTES_PER_BLOCK);
    padding_message_32[16 * block_count - 2] = (uint32_t)((message_len * BITS_PER_BYTE) & 0x00000000ffffffff);
    padding_message_32[16 * block_count - 1] = (uint32_t)(((message_len * BITS_PER_BYTE) >> 32) & 0x00000000ffffffff);
    free(padding_message_char);
}

/*
    缓冲区初始化
*/
void MD5_INIT() {
    for (int i = 0; i < 4; ++i) {
        CV[i] = IV[i];
    }
}

/*
    循环压缩
*/
void MD5_HASH(uint32_t *X) {
    uint32_t CURRENT_CV[4];
    for (int i = 0; i < 4; ++i) {
        CURRENT_CV[i] = CV[i];
    }
    uint32_t TEMP_CV[4];
    // 4轮循环
    for (int i = 0; i < 4; ++i) {
        // 16次迭代运算
        for (int j = 0; j < 16; ++j) {
            if (i == 0) {
                TEMP_CV[0] = CURRENT_CV[1] + CLS((CURRENT_CV[0] + F(CURRENT_CV[1], CURRENT_CV[2], CURRENT_CV[3]) + X[j] + T[i * 16 + j]), s[i * 16 + j]);
            } else if (i == 1) {
                TEMP_CV[0] = CURRENT_CV[1] + CLS((CURRENT_CV[0] + G(CURRENT_CV[1], CURRENT_CV[2], CURRENT_CV[3]) + X[(5 * j + 1) % 16] + T[i * 16 + j]), s[i * 16 + j]);
            } else if (i == 2) {
                TEMP_CV[0] = CURRENT_CV[1] + CLS((CURRENT_CV[0] + H(CURRENT_CV[1], CURRENT_CV[2], CURRENT_CV[3]) + X[(3 * j + 5) % 16] + T[i * 16 + j]), s[i * 16 + j]);
            } else if (i == 3) {
                TEMP_CV[0] = CURRENT_CV[1] + CLS((CURRENT_CV[0] + I(CURRENT_CV[1], CURRENT_CV[2], CURRENT_CV[3]) + X[(7 * j) % 16] + T[i * 16 + j]), s[i * 16 + j]);
            }
            TEMP_CV[1] = CURRENT_CV[1];
            TEMP_CV[2] = CURRENT_CV[2];
            TEMP_CV[3] = CURRENT_CV[3];
            // 循环置换
            CURRENT_CV[0] = TEMP_CV[3];
            CURRENT_CV[1] = TEMP_CV[0];
            CURRENT_CV[2] = TEMP_CV[1];
            CURRENT_CV[3] = TEMP_CV[2];
        }
    }
    // 残差连接
    for (int i = 0; i < 4; ++i) {
        CV[i] = CURRENT_CV[i] + CV[i];
    }
}

/*
    分块和循环压缩
*/
void MD5_DIVISION_AND_HASH(uint32_t *padding_message_32, int block_count) {
    uint32_t X[16];
    for (int i = 0; i < block_count; ++i) {
        for (int j = 0; j < 16; ++j) {
            X[j] = padding_message_32[i * 16 + j]; // 得到一个512字节的块
        }
        MD5_HASH(X);
    }
}

/*
    总控函数
*/
void MD5(char *message, uint8_t *MD5_RESULT_CHAR) {
    int block_count = MD5_GETBLOCKSIZE(message);
    uint32_t *padding_message_32 = (uint32_t *)malloc(BITS_PER_BLOCK * block_count);
    MD5_PADDING(message, block_count, padding_message_32);
    MD5_INIT();
    MD5_DIVISION_AND_HASH(padding_message_32, block_count);
    IntegerToChar(CV, MD5_RESULT_CHAR, 4);
    free(padding_message_32);
}

void afterMD5Hash(unsigned char *message16, unsigned char *message8) {
    for (int i = 0; i < 8; ++i) {
        message8[i] = (message16[i] + message16[i+8]) % 0x100;
    }
}

#if 0
int main() {
    char message[8] = "beijing\0";
    uint8_t MD5_RESULT_CHAR[16];
    MD5(message, MD5_RESULT_CHAR);
    for (int i = 0; i < 16; ++i) {
        printf("0x%02x ", MD5_RESULT_CHAR[i]);
    }
    return 0;
}
#endif
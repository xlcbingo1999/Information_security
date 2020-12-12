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
void transToBoolArray64(uint64_t data, uint64_t *boolArrayFin) {
    int bit_size = sizeof(data);
    int boolArray[bit_size * 8];
    for(int i = 0; i < bit_size * 8; ++i) {
        int bit_num = data % 2;
        if(bit_num) boolArray[i] = 1;
        else boolArray[i] = 0;
        data /= 2;
    }
    for(int i = 0; i < bit_size * 8; ++i) {
        boolArrayFin[i] = boolArray[bit_size * 8 - 1 - i];
    }
}

/* 
- 32位数据转换为32位数组（长度32）
- 输入：
- data: uint32_t 32位数据
- boolArrayFin: uint32_t[] 32位数组（长度32）
- 输出：
- void
*/
void transToBoolArray32(uint32_t data, uint32_t *boolArrayFin) {
    int bit_size = sizeof(data);
    int boolArray[bit_size * 8];
    for(int i = 0; i < bit_size * 8; ++i) {
        int bit_num = data % 2;
        if(bit_num) boolArray[i] = 1;
        else boolArray[i] = 0;
        data /= 2;
    }
    for(int i = 0; i < bit_size * 8; ++i) {
        boolArrayFin[i] = boolArray[bit_size * 8 - 1 - i];
    }
}

/* 
- 48位数据转换为48位数组（长度48）
- 输入：
- data: uint64_t 64位数据（实际48位）
- boolArrayFin: uint64_t[] 48位数组（长度48）
- 输出：
- void
*/
void transToBoolArray48(uint64_t data, uint64_t *boolArrayFin) {
    int bit_size = sizeof(data);
    int boolArray[48];
    for(int i = 0; i < 48; ++i) {
        int bit_num = data % 2;
        data /= 2;
        if(bit_num) boolArray[i] = 1;
        else boolArray[i] = 0;
    }
    for(int i = 0; i < 48; ++i) {
        boolArrayFin[i] = boolArray[47 - i];
    }
}

/* 
- 初始置换IP
- 输入：
- data: uint64_t 64位数据
- L0_p: uint32_t[] 32位数组（长度32）
- R0_p: uint32_t[] 32位数组（长度32）
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
int initTransformIP(uint64_t data , uint32_t *L0_p, uint32_t *R0_p) {
    if(sizeof(data) != sizeof(uint64_t)) return -1;
    int IPTransformTable[64] = {58, 50, 42, 34, 26, 18, 10, 2,
                                60, 52, 44, 36, 28, 20, 12, 4,
                                62, 54, 46, 38, 30, 22, 14, 6,
                                64, 56, 48, 40, 32, 24, 16, 8,
                                57, 49, 41, 33, 25, 17, 9, 1,
                                59, 51, 43, 35, 27, 19, 11, 3,
                                61, 53, 45, 37, 29, 21, 13, 5,
                                63, 55, 47, 39, 31, 23, 15, 7};
    uint64_t boolArray[64];
    transToBoolArray64(data, boolArray);
    for(int i = 0; i < 32; ++i) {
        *L0_p = boolArray[IPTransformTable[i] - 1] + *L0_p * 2;
        *R0_p = boolArray[IPTransformTable[32+i] - 1] + *R0_p * 2;
    }
    return 0;
}

/* 
- feistel轮函数
- 输入：
- R: uint32_t 32位串
- sub_K: uint64_t 48位子密钥
- 输出：
- uint32_t: 结果 32位串
*/
uint32_t feistel(uint32_t R, uint64_t sub_K) {
    // 1. E-扩展
    int IPTransformTable[48] = {32, 1, 2, 3, 4, 5,
                                4, 5, 6, 7, 8, 9,
                                8, 9, 10, 11, 12, 13,
                                12, 13, 14, 15, 16, 17,
                                16, 17, 18, 19, 20, 21,
                                20, 21, 22, 23, 24, 25,
                                24, 25, 26, 27, 28, 29,
                                28, 29, 30, 31, 32, 1};
    uint32_t R_Array[32];
    uint64_t R_E_Extend_48[48];
    uint64_t sub_K_Array[48];
    uint64_t R_K_Array[48];
    transToBoolArray32(R, R_Array);
    transToBoolArray48(sub_K, sub_K_Array);
    uint32_t R_feistel_Array[48];

    // 2. 按位异或
    for(int i = 0; i < 48; ++i) {
        R_E_Extend_48[i] = R_Array[IPTransformTable[i] - 1];
        R_K_Array[i] = R_E_Extend_48[i] ^ sub_K_Array[i];
    }
    // 3. S-盒 6-4转换
    uint64_t S_box[8][64] = {
                {
                14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
                0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
                4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
                15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13
                }, 
                {
                15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
                3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
                0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
                13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9
                },
                {
                10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
                13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
                13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
                1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12
                },
                {
                7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
                13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
                10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
                3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14
                },
                {
                2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
                14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
                4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
                11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3
                },
                {
                12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
                10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
                9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
                4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13
                },
                {
                4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
                13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
                1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
                6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12
                },
                {
                13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
                1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
                7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
                2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11 
                }
            };
    uint64_t n , m;
    uint64_t S_4[8];
    uint32_t S_result = 0;
    for (int i = 0; i < 8; ++i) {
        n = R_K_Array[i * 6 + 0] * 2 + R_K_Array[i * 6 + 5];
        m = R_K_Array[i * 6 + 1] * 8 + R_K_Array[i * 6 + 2] * 4 + R_K_Array[i * 6 + 3] * 2 + R_K_Array[i * 6 + 4];
        S_4[i] = S_box[i][n * 16 + m];
        S_result = (S_result << 4) + S_4[i];
    }
    uint32_t S_result_Array[32];

    transToBoolArray32(S_result, S_result_Array);
    // 4. P-置换
    uint64_t P_trans_table[32] = {16, 7, 20, 21,
                                29, 12, 28, 17,
                                1, 15, 23, 26,
                                5, 18, 31, 10,
                                2, 8, 24, 14,
                                32, 27, 3, 9,
                                19, 13, 30, 6,
                                22, 11, 4, 25};
    uint32_t P_tran_result_Array[32];
    uint32_t P_tran_result = 0;
    for(int i = 0; i < 32; ++i) {
        P_tran_result_Array[i] = S_result_Array[P_trans_table[i] - 1];
        P_tran_result = P_tran_result * 2 + P_tran_result_Array[i];
    }
    return P_tran_result;
}

/* 
- 置换PC-1
- 输入：
- key_real_Array: uint64_t[] 64位key
- C0: uint64_t[] 28位C0
- D0: uint64_t[] 28位D0
- 输出：
- void
*/
void PC1_trans(uint64_t *key_real_Array, uint64_t *C0, uint64_t *D0) {
    int PC1_trans_table[56] = {
                            57, 49, 41, 33, 25, 17, 9,
                            1, 58, 50, 42, 34, 26, 18,
                            10, 2, 59, 51, 43, 35, 27,
                            19, 11, 3, 60, 52, 44, 36,
                            63, 55, 47, 39, 31, 23, 15,
                            7, 62, 54, 46, 38, 30, 22,
                            14, 6, 61, 53, 45, 37, 29,
                            21, 13, 5, 28, 20, 12, 4
                            };
    for(int i = 0; i < 28; ++i) {
        C0[i] = key_real_Array[PC1_trans_table[i]-1];
        D0[i] = key_real_Array[PC1_trans_table[i+28]-1];
    }
}

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
void shiftLeft1(uint64_t *C_origin, uint64_t *D_origin, uint64_t *C_fin, uint64_t *D_fin) {
    for(int i = 0; i < 28; ++i) {
        if(i == 27) {
            C_fin[i] = C_origin[0];
            D_fin[i] = D_origin[0];
        } else {
            C_fin[i] = C_origin[i+1];
            D_fin[i] = D_origin[i+1];
        }
    }
}

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
void shiftLeft2(uint64_t *C_origin, uint64_t *D_origin, uint64_t *C_fin, uint64_t *D_fin) {
    for(int i = 0; i < 28; ++i) {
        if(i == 26) {
            C_fin[i] = C_origin[0];
            D_fin[i] = D_origin[0];
        } else if(i == 27) {
            C_fin[i] = C_origin[1];
            D_fin[i] = D_origin[1];
        } else {
            C_fin[i] = C_origin[i+2];
            D_fin[i] = D_origin[i+2];
        }
    }
}

/* 
- 压缩后置换PC-2
- 输入：
- C: uint64_t[] 28位C1-C16
- D: uint64_t[] 28位D1-D16
- 输出：
- void
*/
uint64_t PC2_compression(uint64_t *C, uint64_t *D) {
    uint64_t K_array[56];
    int count = 0;
    for(int i = 0; i < 28; ++i) {
        K_array[count++] = C[i];
    }
    for(int i = 0; i < 28; ++i) {
        K_array[count++] = D[i];
    }
    int PC2_table[48] =  {14, 17, 11, 24, 1, 5,
                        3, 28, 15, 6, 21, 10,
                        23, 19, 12, 4, 26, 8,
                        16, 7, 27, 20, 13, 2,
                        41, 52, 31, 37, 47, 55,
                        30, 40, 51, 45, 33, 48,
                        44, 49, 39, 56, 34, 53,
                        46, 42, 50, 36, 29, 32};
    uint64_t K = 0;
    for(int i = 0; i < 48; ++i) {
        K = K * 2 + K_array[PC2_table[i] - 1];
    }
    return K;
}

/* 
- 获得48位子密钥
- 输入：
- key: uint64_t 密钥
- K: uint64_t[] 子密钥数组（48位，长度16）
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
void get48SubKey(uint64_t key, uint64_t *K) {
    uint64_t key_real_Array[64];
    transToBoolArray64(key, key_real_Array);
    uint64_t C0[28];
    uint64_t D0[28];
    PC1_trans(key_real_Array, C0, D0);
    uint64_t C[16][28];
    uint64_t D[16][28];
    for(int i = 0; i < 16; ++i) {
        if(i == 0) {
            shiftLeft1(C0, D0, C[0], D[0]);
        } else if (i == 1 || i == 8 || i == 15) {
            shiftLeft1(C[i-1], D[i-1], C[i], D[i]);
        } else {
            shiftLeft2(C[i-1], D[i-1], C[i], D[i]);
        }
        K[i] = PC2_compression(C[i], D[i]);
        
    }
}

/* 
- 加密器中的16次迭代序列
- 输入：
- L0: uint32_t 输入L0
- R0: uint32_t 输入R0
- key: uint64_t 输入密钥
- 输出：
- uint64_t: 交换置换后的64位串
*/
uint64_t T_16(uint32_t L0, uint32_t R0, uint64_t key) {
    uint32_t L_T[16], R_T[16];
    uint64_t K[16];
    get48SubKey(key, K);
    for(int i = 0; i < 16; ++i) {
        if(i == 0) {
            L_T[i] = R0;
            R_T[i] = L0 ^ feistel(R0, K[i]);
        } else {
            L_T[i] = R_T[i-1];
            R_T[i] = L_T[i-1] ^ feistel(R_T[i-1], K[i]);
        }
    }
    uint64_t L_long_T16 = (uint64_t)L_T[15];
    uint64_t R_long_T16 = ((uint64_t)R_T[15]) << 32;
    return L_long_T16 + R_long_T16;
}

/* 
- 解密器中的16次迭代序列
- 输入：
- L0: uint32_t 输入L0
- R0: uint32_t 输入R0
- key: uint64_t 输入密钥
- 输出：
- uint64_t: 交换置换后的64位串
*/
uint64_t T_16_D(uint32_t L0, uint32_t R0, uint64_t key) {
    uint32_t L_T[16], R_T[16];
    uint64_t K[16];
    get48SubKey(key, K);
    for(int i = 0; i < 16; ++i) {
        if(i == 0) {
            L_T[i] = R0;
            R_T[i] = L0 ^ feistel(R0, K[15-i]);
        } else {
            L_T[i] = R_T[i-1];
            R_T[i] = L_T[i-1] ^ feistel(R_T[i-1], K[15-i]);
        }
    }
    uint64_t L_long_T16 = (uint64_t)L_T[15];
    uint64_t R_long_T16 = ((uint64_t)R_T[15])<< 32;
    return L_long_T16 + R_long_T16;
}

/* 
- 逆置换IP^{-1}
- 输入：
- data: uint64_t 交换置换后的64位串
- 输出：
- uint64_t: 密文或明文
*/
uint64_t initTransformIPtransver(uint64_t data) {
    if(sizeof(data) != sizeof(uint64_t)) return -1;
    int IPTransformtransverTable[64] = {
                                40, 8, 48, 16, 56, 24, 64, 32,
                                39, 7, 47, 15, 55, 23, 63, 31,
                                38, 6, 46, 14, 54, 22, 62, 30,
                                37, 5, 45, 13, 53, 21, 61, 29,
                                36, 4, 44, 12, 52, 20, 60, 28,
                                35, 3, 43, 11, 51, 19, 59, 27,
                                34, 2, 42, 10, 50, 18, 58, 26,
                                33, 1, 41, 9, 49, 17, 57, 25
                                };
    uint64_t boolArray[64];
    uint64_t boolArrayResult = 0;
    transToBoolArray64(data, boolArray);

    for(int i = 0; i < 64; ++i) {
        boolArrayResult = boolArrayResult * 2 + boolArray[IPTransformtransverTable[i] - 1];
    }
    return boolArrayResult;
}

/* 
- 字符串转位数组（长度64）
- 输入：
- str: unsigned char[] 源数据的字节数组
- metadata: uint64_t[] 64位数组（长度64）
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
int str2metadata(unsigned char *str, uint64_t *metadata) {
    int str_length = strlen(str);
    if (str_length <= 0) return -1;
    int normal_8_byte = str_length / 8;
    int padding_number = 8 - str_length % 8;
    int count = 0;
    for(int i = 0; i < normal_8_byte; ++i) {
        for(int j = 0; j < 8; ++j) {
            metadata[i] = (metadata[i] << 8) + str[count++];
        }
    }
    for(int i = 0; i < 8 - padding_number; ++i) {
        metadata[normal_8_byte] = (metadata[normal_8_byte] << 8) + str[count++];
    }
    for(int i = 0; i < padding_number; ++i) {
        metadata[normal_8_byte] = (metadata[normal_8_byte] << 8) + padding_number;
    }
    return 0;
}

int str2uint64(unsigned char *str, uint64_t *metadata) {
    int str_length = strlen(str);
    if (str_length <= 0) return -1;
    int normal_8_byte = str_length / 8;
    int count = 0;
    for(int i = 0; i < normal_8_byte; ++i) {
        for(int j = 0; j < 8; ++j) {
            metadata[i] = (metadata[i] << 8) + str[count++];
        }
    }
    return 0;
}

/* 
- 字符串格式密钥转为64位无符号整数格式密钥
- 输入：
- str: unsigned char * 字符串格式密钥
- 输出：
- uint64_t: 64位无符号整数格式密钥
*/
uint64_t kstr2k64(unsigned char *str) {
    int str_length = strlen(str);
    if (str_length != 8) return -1;
    int count = 0;
    uint64_t metadata = 0;
    for(int j = 0; j < 8; ++j) {
        metadata = (metadata << 8) + str[count++];
    }
    return metadata;
}

/* 
- 加密器
- 输入：
- metadata: uint64_t[] 源数据的字节数组
- metadata_size: int 64位数据的长度
- secdata: uint64_t[] 加密后得到的字节数组
- key: uint64_t 密钥
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
int encoder(uint64_t *metadata, int metadata_size, uint64_t *secdata, uint64_t key) {
    for(int i = 0; i < metadata_size; ++i) {
        // 1. 初始置换IP
        uint32_t L0 = 0, R0 = 0;
        uint32_t *L0_p = &L0;
        uint32_t *R0_p = &R0;
        int result1 = initTransformIP(metadata[i] ,L0_p, R0_p);
        if(result1 < 0) return -1;

        // 2. 16轮迭代T和交换置换
        uint64_t R16L16 = T_16(L0, R0, key);

        // 3. 逆置换IP-1
        secdata[i] = initTransformIPtransver(R16L16);
    }
    return 0;
}

/* 
- 解密器
- 输入：
- secdata: uint64_t[] 加密后得到的字节数组
- metadata_size: int 64位数据的长度
- origindata: uint64_t[] 解密后得到的字节数组
- key: uint64_t 密钥
- 输出：
- int: 结果代码，用于标识该过程是否正确完成
*/
int decoder(uint64_t *secdata, int metadata_size, uint64_t *origindata, uint64_t key) {
    for(int i = 0; i < metadata_size; ++i) {
        uint32_t L0_D = 0, R0_D = 0;
        uint32_t *L0_D_p = &L0_D;
        uint32_t *R0_D_p = &R0_D;
        int result3 = initTransformIP(secdata[i] ,L0_D_p, R0_D_p);
        if(result3 < 0) return -1;

        uint64_t R16L16_D = T_16_D(L0_D, R0_D, key);
        origindata[i] = initTransformIPtransver(R16L16_D);
    }
    return 0;
}

/* 
- 加密函数
- 输入：
- str: unsigned char * 源数据的字节数组
- sec_str: unsigned char * 加密后的字节数组
- key: uint64_t 密钥
- 输出：
- int: 加密后的字节数据长度
*/
int encryption(unsigned char *str, unsigned char *sec_str, uint64_t key) {
    int input_str_length = strlen(str);   
    int uint64_size = input_str_length / 8 + 1;
    uint64_t *metadata = (uint64_t*)malloc(uint64_size * sizeof(uint64_t));
    int res1 = str2metadata(str, metadata);
    if(res1 < 0) {
        free(metadata);
        return -1;
    }

    uint64_t *secdata = (uint64_t*)malloc(uint64_size * sizeof(uint64_t));
    int res2 = encoder(metadata, uint64_size, secdata, key);
    if(res2 < 0) {
        free(metadata);
        free(secdata);
        return -1;
    } 
    for(int i = 0; i < uint64_size; ++i) {
        uint64_t tempsecdata = secdata[i];
        for(int j = 0; j < 8; ++j) {
            sec_str[i * 8 + (7 - j)] = tempsecdata % 0x100;
            tempsecdata /= 0x100;
        }
    }
    sec_str[uint64_size * 8] = '\0';
    return uint64_size * 8;
}

/* 
- 解密函数
- 输入：
- secdata: unsigned char * 解密前数据的字节数组
- origin_str: unsigned char * 解密后源数据的字节数组
- key: uint64_t 密钥
- 输出：
- int: 解密后源数据的字节数据长度
*/
int decryption(unsigned char *secdata, unsigned char *origin_str, uint64_t key) {
    int secdata_length = strlen(secdata);
    int uint64_size = secdata_length / 8;
    uint64_t *metadata = (uint64_t*)malloc(uint64_size * sizeof(uint64_t));
    int res1 = str2uint64(secdata, metadata);
    if(res1 < 0) {
        free(metadata);
        return -1;
    }

    uint64_t *origindata = (uint64_t*)malloc(uint64_size * sizeof(uint64_t));
    int res3 = decoder(metadata, uint64_size, origindata, key);
    if(res3 < 0) {
        free(metadata);
        free(origindata);
        return -1;
    }

    for(int i = 0; i < uint64_size; ++i) {
        uint64_t temporigindata = origindata[i];
        for(int j = 0; j < 8; ++j) {
            origin_str[i * 8 + (7 - j)] = temporigindata % 0x100;
            temporigindata /= 0x100;
        }
    }
    unsigned char last_str = origin_str[(uint64_size - 1) * 8 + 7];
    int last_str_index = (int)last_str;
    origin_str[(uint64_size - 1) * 8 + 8 - last_str_index] = '\0';
    free(origindata);
    return ((uint64_size - 1) * 8 + 8 - last_str_index);
}

# if 0
int main() {
    int input_str_length = 12; // 请在此处更改您所要输入的字符的长度
    unsigned char *str = (unsigned char*)malloc(input_str_length + 1);

    str[0] = 'B';
    str[1] = 'i';
    str[2] = 'a';
    str[3] = 'o';
    str[4] = 'L';
    str[5] = 'i';
    str[6] = 'n';
    str[7] = 'C';
    str[8] = 'h';
    str[9] = 'a';
    str[10] = 'n';
    str[11] = 'g';
    str[12] = '\0';
    uint64_t key = 0x3636363636363636; // 请在这里输入64位密钥
    
    unsigned char enc_result[1024] = {0};
    unsigned char *enc_result_ptr = &enc_result;
    // unsigned char *enc_result_ptr;
    int enc_length = encryption(str, enc_result_ptr, key);
    enc_result_ptr[enc_length] = '\0';
    
    printf("enc_result: %s [length: %d]\n", enc_result, strlen(enc_result));
    
    unsigned char dec_result[1024]  = {0};
    unsigned char *dec_result_ptr = &dec_result;
    // unsigned char *dec_result_ptr;
    int dec_length = decryption(enc_result, dec_result_ptr, key);
    dec_result[dec_length] = '\0';
    
    printf("dec_result: %s\n", dec_result);
    return 0;
}
#endif
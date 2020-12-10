#ifndef ENCODER_H
#define ENCODER_H
#include "funcDES.h"

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
int encoder(uint64_t *metadata, int metadata_size, uint64_t *secdata, uint64_t key);

#endif
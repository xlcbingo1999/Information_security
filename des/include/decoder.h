#ifndef DECODER_H
#define DECODER_H
#include "funcDES.h"

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
int decoder(uint64_t *secdata, int metadata_size, uint64_t *origindata, uint64_t key);

#endif 
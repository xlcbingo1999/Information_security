#include "decoder.h"

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
#include "encoder.h"

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
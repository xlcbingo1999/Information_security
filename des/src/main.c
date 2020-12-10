#include "encoder.h"
#include "decoder.h"

int main(){ 
    // 1. 输入模块---------------------------------------------------------------------------------------------------------------
    int input_str_length = 12; // 请在此处更改您所要输入的字符的长度
    unsigned char *str = (unsigned char*)malloc(input_str_length + 1);
    // 请在这里更改输入的字符串内容
    // 1.1 for循环输入模块
    // srand((unsigned)time(NULL));
    // for (int i = 0; i < input_str_length; ++i) { 
    //     str[i] = rand() % 93 + '!';
    // }
    // str[input_str_length] = '\0';
    // 1.2 手动赋值输入模块
    str[0] = 'X';
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

    // 5.打印模块------------------------------------------------------------------------------------------------------------------
    printf("\n------------------------------origin data----------------------------------\n");
    printf("origin strring: %s [length: %d]\n",str, strlen(str));
    for (int i = 0; i < input_str_length; ++i) {
        if(i == 0) printf("origin bytes: ");
        if(i % 8 == 0 && i > 0) printf("\norigin bytes: ");
        printf("0x%02x ", str[i]);
    }
    printf("\n-----------------------------------------------------------------------\n");
    
    
    // 2. 填充模块---------------------------------------------------------------------------------------------------------------
    int uint64_size = input_str_length / 8 + 1;
    uint64_t *metadata = (uint64_t*)malloc(uint64_size * sizeof(uint64_t));
    int res1 = str2metadata(str, metadata);
    if(res1 < 0) {
        free(str);
        free(metadata);
        return -1;
    }

    // 3. 加密模块---------------------------------------------------------------------------------------------------------------
    uint64_t *secdata = (uint64_t*)malloc(uint64_size * sizeof(uint64_t));
    int res2 = encoder(metadata, uint64_size, secdata, key);
    if(res2 < 0) {
        free(str);
        free(metadata);
        free(secdata);
        return -1;
    }
    
    // 5. 打印模块---------------------------------------------------------------------------------------------------------------
    printf("\n-----------------------------encrypted data----------------------------------\n");
    
    unsigned char *sec_str = (unsigned char*)malloc(uint64_size * 8 + 1);
    for(int i = 0; i < uint64_size; ++i) {
        uint64_t tempsecdata = secdata[i];
        for(int j = 0; j < 8; ++j) {
            sec_str[i * 8 + (7 - j)] = tempsecdata % 0x100;
            tempsecdata /= 0x100;
        }
        printf("secbytes: ");
        for(int j = 0; j < 8; ++j) {
            printf("0x%02x ", sec_str[i * 8 + j]);
        }
        printf("\n");
    }
    sec_str[uint64_size * 8] = '\0';
    printf("secdata: %s\n [Maybe you cannot see it fully.]", sec_str);
    printf("\n-----------------------------------------------------------------------\n");

    // 4. 解密模块---------------------------------------------------------------------------------------------------------------
    uint64_t *origindata = (uint64_t*)malloc(uint64_size * sizeof(uint64_t));
    int res3 = decoder(secdata, uint64_size, origindata, key);
    if(res3 < 0) {
        free(str);
        free(metadata);
        free(secdata);
        free(sec_str);
        free(origindata);
    }
    // 5. 打印模块---------------------------------------------------------------------------------------------------------------
    printf("\n--------------------------recover origin data----------------------------------\n");
    unsigned char *origin_str = (unsigned char*)malloc(uint64_size * 8 + 1);
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
    printf("recover origin data: %s [length: %d]\n", origin_str, strlen(origin_str));
    printf("\n-----------------------------------------------------------------------\n");

    // 检验模块----------------------------------------------------------------------------------------------------------
    int co;
    for(co = 0; co < input_str_length; ++co) {
        if(str[co] != origin_str[co]){
            printf("RESULT: WRONG!\n");
            break;
        }
    }
    if(co == input_str_length) {
        printf("RESULT: RIGHT!\n");
    }

    // 动态分配内存释放----------------------------------------------------------------------------------------------------------
    free(str);
    free(metadata);
    free(secdata);
    free(sec_str);
    free(origindata);
    free(origin_str);
}
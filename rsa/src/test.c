#include "test.h"

#include "RSAES_PKCS1_V1_5_DECRYPT.h"
#include "RSAES_PKCS1_V1_5_ENCRYPT.h"
#include "generate_key.h"

int TEST_random() {
    printf("---------- BEGIN TEST_random ---------\n");
    // 密钥生成模块
    mpz_t key_N;
    mpz_init(key_N);
    mpz_t key_e;
    mpz_init(key_e);
    mpz_t key_d;
    mpz_init(key_d);
    int result_generate_key = generate_key_random(key_N, key_e, key_d, 2048);
    if (result_generate_key != 0) {
        return -3;
    }

    // 源文 MESSAGE_STR
    size_t mLen = 12;
    unsigned char MESSAGE_STR[mLen + 1];
    for (int i = 0; i < mLen; ++i) {
        MESSAGE_STR[i] = i + 'A';
    }
    MESSAGE_STR[mLen] = '\0';
    printf("origin message:\n%s\n", MESSAGE_STR);
    unsigned char *MESSAGE_STR_ptr = &MESSAGE_STR;

    mpz_t key_temp_N;
    int k = 0;
    mpz_init_set(key_temp_N, key_N);
    while (mpz_cmp_ui(key_temp_N, (unsigned long)0) > 0) {
        mpz_fdiv_q_ui(key_temp_N, key_temp_N, 256);
        k++;
    }
    mpz_clear(key_temp_N);

    unsigned char ciphertext[k + 1];
    unsigned char *ciphertext_ptr = &ciphertext;

    int result_encrypt = RSAES_PKCS1_V1_5_ENCRYPT(ciphertext_ptr, key_N, key_e, MESSAGE_STR_ptr);
    if (result_encrypt != k) {  // 这里就当作是后一个的比较先行
        mpz_clear(key_N);
        mpz_clear(key_e);
        mpz_clear(key_d);
        return -1;
    }

    unsigned char MESSAGE_STR_D[k - 11];
    unsigned char *MESSAGE_STR_D_ptr = &MESSAGE_STR_D;
    int result_decrypt = RSAES_PKCS1_V1_5_DECRYPT(MESSAGE_STR_D_ptr, key_N, key_d, ciphertext_ptr);
    if (result_decrypt != 0) {
        mpz_clear(key_N);
        mpz_clear(key_e);
        mpz_clear(key_d);
        return -2;
    }

    int wrong_flag = 0;
    printf("\ndecrypt result:\n");
    for (int i = 0; i < mLen; ++i) {
        if (MESSAGE_STR[i] != MESSAGE_STR_D[i]) {
            wrong_flag = 1;
            break;
        } else {
            printf("%c", MESSAGE_STR_D[i]);
        }
    }
    printf("\n");
    if (wrong_flag == 1) {
        printf("WRONG!\n");
        return -3;
    }
    return 0;
}

int TEST_given_p_and_q() {
    printf("---------- BEGIN TEST_given_p_and_q ---------\n");
    // 密钥生成模块
    mpz_t key_p, key_q;
    mpz_init(key_p);
    mpz_init(key_q);
    char given_p[] = "0x00f4ac8161cefd39cf4ec8f9e11716b18a9b01bb5ef667f78053236796d5bf7b10f25c142f80b8b10aa84ae1996eea3280272f16ab13a6048ada6a264b338cf826becdb86d1f9189fcd1b09ffb1ab18c5cd796205be4a9d97819bc339fd4d5029e185917e8614e095de4c26b1dbf140ee35f918ceb8b95411d1e6dd838fed0a695\0";
    mpz_set_str(key_p, given_p, 0);
    gmp_printf("given p: %ZX\n", key_p);
    char given_q[] = "0x00e7ccf36f4368a800c5f744f82b9941b4a5f72329bc93f60511f944cb50a47dfae6cc24303a85f910ac25d77f81deb37453d359a6f43002b5b07106d2cef46d10567571e75e02f38fc075108fab1392df5f8d468790d029941f7e75f5ff0d4915c713251588fc5a469cd10625362168cc3715c35f7e94f1ac7efc00689b09a599\0";
    mpz_set_str(key_q, given_q, 0);
    gmp_printf("given q: %ZX\n", key_q);

    mpz_t key_N;
    mpz_init(key_N);
    mpz_t key_e;
    mpz_init(key_e);
    mpz_t key_d;
    mpz_init(key_d);
    int result_generate_key = generate_key_with_p_and_q(key_N, key_e, key_d, key_p, key_q);
    if (result_generate_key != 0) {
        return -3;
    }
    mpz_clear(key_p);
    mpz_clear(key_q);

    // 源文 MESSAGE_STR
    size_t mLen = 12;
    unsigned char MESSAGE_STR[mLen + 1];
    for (int i = 0; i < mLen; ++i) {
        MESSAGE_STR[i] = i + 'A';
    }
    MESSAGE_STR[mLen] = '\0';
    printf("origin message:\n%s\n", MESSAGE_STR);
    unsigned char *MESSAGE_STR_ptr = &MESSAGE_STR;

    mpz_t key_temp_N;
    int k = 0;
    mpz_init_set(key_temp_N, key_N);
    while (mpz_cmp_ui(key_temp_N, (unsigned long)0) > 0) {
        mpz_fdiv_q_ui(key_temp_N, key_temp_N, 256);
        k++;
    }
    mpz_clear(key_temp_N);

    unsigned char ciphertext[k + 1];
    unsigned char *ciphertext_ptr = &ciphertext;

    int result_encrypt = RSAES_PKCS1_V1_5_ENCRYPT(ciphertext_ptr, key_N, key_e, MESSAGE_STR_ptr);
    if (result_encrypt != k) {  // 这里就当作是后一个的比较先行
        mpz_clear(key_N);
        mpz_clear(key_e);
        mpz_clear(key_d);
        return -1;
    }

    unsigned char MESSAGE_STR_D[k - 11];
    unsigned char *MESSAGE_STR_D_ptr = &MESSAGE_STR_D;
    int result_decrypt = RSAES_PKCS1_V1_5_DECRYPT(MESSAGE_STR_D_ptr, key_N, key_d, ciphertext_ptr);
    if (result_decrypt != 0) {
        mpz_clear(key_N);
        mpz_clear(key_e);
        mpz_clear(key_d);
        return -2;
    }
    printf("\ndecrypt result:\n");
    int wrong_flag = 0;
    for (int i = 0; i < mLen; ++i) {
        if (MESSAGE_STR[i] != MESSAGE_STR_D[i]) {
            wrong_flag = 1;
            break;
        } else {
            printf("%c", MESSAGE_STR_D[i]);
        }
    }
    printf("\n");
    if (wrong_flag == 1) {
        printf("WRONG!\n");
        return -3;
    }
    return 0;
}

int TEST_random_str() {
    printf("---------- BEGIN TEST_random_str ---------\n");
    // 密钥生成模块
    mpz_t key_N;
    mpz_init(key_N);
    mpz_t key_e;
    mpz_init(key_e);
    mpz_t key_d;
    mpz_init(key_d);
    int result_generate_key = generate_key_random(key_N, key_e, key_d, 2048);
    if (result_generate_key != 0) {
        return -3;
    }

    // 源文 MESSAGE_STR
    size_t mLen = 12;
    unsigned char MESSAGE_STR[mLen + 1];
    for (int i = 0; i < mLen; ++i) {
        MESSAGE_STR[i] = i % 92 + '!';
    }
    MESSAGE_STR[mLen] = '\0';
    printf("origin message:\n%s\n", MESSAGE_STR);
    unsigned char *MESSAGE_STR_ptr = &MESSAGE_STR;

    mpz_t key_temp_N;
    int k = 0;
    mpz_init_set(key_temp_N, key_N);
    while (mpz_cmp_ui(key_temp_N, (unsigned long)0) > 0) {
        mpz_fdiv_q_ui(key_temp_N, key_temp_N, 256);
        k++;
    }
    mpz_clear(key_temp_N);

    unsigned char ciphertext[k + 1];
    unsigned char *ciphertext_ptr = &ciphertext;

    int result_encrypt = RSAES_PKCS1_V1_5_ENCRYPT(ciphertext_ptr, key_N, key_e, MESSAGE_STR_ptr);
    if (result_encrypt != k) {  // 这里就当作是后一个的比较先行
        mpz_clear(key_N);
        mpz_clear(key_e);
        mpz_clear(key_d);
        return -1;
    }

    unsigned char MESSAGE_STR_D[k - 11];
    unsigned char *MESSAGE_STR_D_ptr = &MESSAGE_STR_D;
    int result_decrypt = RSAES_PKCS1_V1_5_DECRYPT(MESSAGE_STR_D_ptr, key_N, key_d, ciphertext_ptr);
    if (result_decrypt != 0) {
        mpz_clear(key_N);
        mpz_clear(key_e);
        mpz_clear(key_d);
        return -2;
    }

    int wrong_flag = 0;
    printf("\ndecrypt result:\n");
    for (int i = 0; i < mLen; ++i) {
        if (MESSAGE_STR[i] != MESSAGE_STR_D[i]) {
            wrong_flag = 1;
            break;
        } else {
            printf("%c", MESSAGE_STR_D[i]);
        }
    }
    printf("\n");
    if (wrong_flag == 1) {
        printf("WRONG!\n");
        return -3;
    }
    return 0;
}
#include "RSAES_PKCS1_V1_5_ENCRYPT.h"

int RSAES_PKCS1_V1_5_ENCRYPT(unsigned char *ciphertext, mpz_t key_N, mpz_t key_e, unsigned char *origin_message) {
    mpz_t key_temp_N;
    int k = 0;
    mpz_init_set(key_temp_N, key_N);
    while (mpz_cmp_ui(key_temp_N, (unsigned long)0) > 0) {
        mpz_fdiv_q_ui(key_temp_N, key_temp_N, 256);
        k++;
    }
    mpz_clear(key_temp_N);

    // RSAES-PKCS1-V1_5-ENCRYPT
    // 1. length checking
    size_t mLen = strlen(origin_message);
    if (mLen > k - 11) {
        printf("message too long.\n");
        return -1;
    } else {
        // 2. EME-PKCS1-v1_5
        // a. 生成伪随机串PS，保证非0
        unsigned char PS[k - mLen - 2];
        srand((unsigned)time(0));
        for (int i = 0; i < k - mLen - 3; ++i) {
            PS[i] = (unsigned char)((rand() % 255) + 1);
        }
        PS[k - mLen - 3] = '\0';

        // b. 生成EM
        unsigned char EM[k + 1];
        EM[0] = (unsigned char)0;
        EM[1] = (unsigned char)2;
        for (int i = 0; i < k - mLen - 3; ++i) {
            EM[2 + i] = PS[i];
        }
        EM[k - mLen - 1] = (unsigned char)0;
        for (int i = 0; i < mLen; ++i) {
            EM[k - mLen + i] = origin_message[i];
        }
        EM[k] = '\0';
        // 3. RSA encryption
        // a. OS2IPn
        mpz_t n;
        mpz_init(n);
        int result_OS2IP = OS2IP(n, EM, k);
        if (result_OS2IP != 0) {
            return -2;
        }

        // b. 加密 RSAEP
        mpz_t encrypt_small_c;
        mpz_init(encrypt_small_c);
        int result_RSAEP = RSAEP(encrypt_small_c, n, key_e, key_N);
        if (result_RSAEP != 0) {
            return -3;
        }
        mpz_clear(n);

        // c. I2OSP
        int result_I2OSP = I2OSP(ciphertext, encrypt_small_c, k);
        if (result_I2OSP != 0) {
            mpz_clear(encrypt_small_c);
            return -4;
        } else {
            // printf("%s\n", ciphertext);
            int ciphertext_size = k;
            printf("\nencrypt result:\n");
            for (int i = 0; i < ciphertext_size; ++i) {
                printf("%02x ", ciphertext[i]);
            }
            printf("\n\n");
            mpz_clear(encrypt_small_c);
            return ciphertext_size;
        }
    }
}